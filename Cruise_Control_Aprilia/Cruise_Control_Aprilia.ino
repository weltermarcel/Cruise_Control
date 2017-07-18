#include <LiquidCrystal_I2C.h>
#include <VonSerialEinlesen.h>
#include <Servo.h>
#include <Wire.h>
#include <EEPROM.h>
#define OCTAVE_OFFSET 0
#include <DS1302.h>
#include "DHT.h"
#define DHTPIN A3                                   // what pin is the DHT22 connected to
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
DS1302 rtc(10, 11, 12);
Time t;
LiquidCrystal_I2C lcd(0x27, 20, 4); 
Servo throttle;

const int In_Sensor_Speed = 2;                      //speed sensor pin

const int In_Button_Resume = 4;                     //control between tone and servo control
const int In_Button_Increase = 5;                   //increase speed
const int In_Button_Decrease = 6;                   //decrease speed
const int In_Button_Set = 7;                        //set speed
const int Out_Servo = 8;                            //servo's control pin
const int In_Sensor_Brake = 9;                      //0 kmh -> to brake light

const int In_Pulser = 13;                           //simulate speed over Serial_Interface
const int tonePin = A1;                             //Piezo pin
const int Out_Reset = A2;                           //to upload sketch
const int Time_Delay_Reset = 3;                     //reset delay to change from serial monitor to upload interface
bool Status_First_Change = true;
bool Status_Time_Min_Push = false;
bool Status_Time_Double_Push = false;
bool Override_Servo = false;
bool Status_LCD_Backlight = true;                   //lcd's backlight control
bool Status_Cruise_Control = true;                  //default ON or OFF
bool Flag_Clock_Set = false;                        //1 to set, 0 to lock
bool Status_Serial_Interface = false;               //serial interface on or off
bool Status_Servo = false;                          //servo deattached until void loop() is once run (to avoid movement at startup)
bool Flag_Servo_Music = false;
bool Flag_Show_Temp = true;
int a = 0;                                          //in clock function so that minutes only add 1 at the time
int b = 0;                                          //in clock function so that hours only add 1 at the time
int c = 0;                                          //lcd's backlightcontrol
int d = 0;                                          //control variable for Time_Min_Push
int e = 0;                                          //control variable for acceleration
int f = 0;                                          //control variable for void velocity()
int g = 0;                                          //EEPROM address for 10 x km
int h = 2;                                          //EEPROM address for 1  x km
int i = 3;                                          //EEPROM address for 0.1x km
int j = 4;                                          //EEPROM address for 1  x m
int z = 0;                                          //EEPROM clear control
int seconds = 0;
int minutes = 0;
int hours = 0;                                      //resume speed
int hh = 18;                                        //clock adjust: hours
int mm = 7;                                         //clock adjust: minutes
int ss = 0;                                         //clock adjust: seconds
int dd = 19;                                        //clock adjust: day (give day of the week in void setup())
int mo = 10;                                        //clock adjust: month
int yy = 2015;                                      //clock adjust: year
int counter_1 = 0;                                  //EEPROM control
int pulses_pro_rotation = 1;                        //full pulses (holes/pulses_asymmetry) in a whole rotation
int v_diff_up = 2;                                  //tolerance between v_soll and v_ist to accelerate
int v_diff_down = -1;                               //tolerance beteween v_soll and v_ist to deaccelerate
int override_factor = 3;                            //pos_servo = v_ist / override_factor
int min_pos = 5;                                    //servo's min position
int max_pos = min_pos + 110;                        //servo's max position
int override_pos = min_pos + 5;                     //servo must be under this value to override its position according to speed
int tone_offset = 15;                               //to lower RPM's for song with servo
int min_tone_frequency = 600;                       //for song with servo (262)
int max_tone_frequency = 15000;                     //for song with servo (3951)
int pos_servo = min_pos;                            //servo's control position
int Override_ServoValue = 0;
int Override_ServoFactor = 0;
int step_up = 3;                                    //step on servo's new setting accelerating
int step_down = 1;                                  //step on servo's new setting deaccelerating
float pulser_time = 0;
float command = 0;                                  //functions of doubleEinlesen
float temp = 0;                                     //temperature
float hum = 0;
float Time_Switch_Temp_Hum = 2;                     //time to switch between temp and humidity
float Time_Diff_Up = 1.2;                           //time to readjust servo's position accelerating
float Time_Diff_Down = 0.6;                         //time to readjust servo's position deaccelerating
float servo_percentage = 0;                         //relative to max_position
float v_min = 20;                                   //min v_ist to allow servo's throttle control
float v_soll = 0;                                   //set speed
float v_ist = 0;                                    //actual speed
float last_v_ist = 0;                               //last actual speed for acceleration calculation
float acc = 0;                                      //acceleration in kmh/s
float max_acc_up = 2;                               //max acceleration in kmh/s
float acc_refresh = 1.5;                            //time in s to refresh acceleration
float min_pulse_time = 1.5;                         //time in s so that v_ist displays "0"
float last_speed = 0;                               //for resume function
float last_servo_pos = 0;                           //for resume function
float wheel_circumference = 2.04;                  //in meters
float dist = 0;                                     //elapsed distance since boot based on counter and wheel_circumference
float dist_total = 0;                               //distance stored in EEPROM
float pulse_time = 0;                               //time between 2 pulses in s
float serial_refresh = 0.5;                         //serial.print refresh time
float Time_Min_Push = 0.1;                          //min time to set/resume/adjust speed
float Time_Double_Push = 0.2;                       //time between 2 button clicks
float screen_refresh = 0.5;                         //time to refresh screen
float EEPROM_read_refresh = 0.5;                    //refresh time of EEPROM.read for dist_total calculation
float loop_time = 0;
float last_pulse_time = 0;                          //reference time for pulse_time
String inputString1 = "";
String inputString2 = "";
String inputString3 = "";
unsigned long last_millis_1 = 0;                    //serial.print control time
unsigned long last_millis_2 = 0;                    //pos_servo control time
unsigned long last_millis_3 = 0;                    //pos_servo control time
unsigned long last_millis_4 = 0;                    //double press control time
unsigned long last_millis_5 = 0;                    //screen control time
unsigned long last_millis_6 = 0;                    //empty
unsigned long last_millis_7 = 0;                    //minimum button press control time
unsigned long last_millis_8 = 0;                    //acceleration's minimum time to refresh last_v_ist and calculate new value
unsigned long last_millis_9 = 0;                    //EEPROM read control for dist_total calculation
unsigned long last_millis_10 = 0;                   //void velocity() control time
unsigned long last_millis_11 = 0;                   //void loop() time
unsigned long last_millis_12 = 0;                   //switch between temp and humidity

													//char *song = "The Simpsons:d=4,o=5,b=160:c.6,e6,f#6,8a6,g.6,e6,c6,8a,8f#,8f#,8f#,2g,8p,8p,8f#,8f#,8f#,8g,a#.,8c6,8c6,8c6,c6";
													//char *song = "Indiana:d=4,o=5,b=250:e,8p,8f,8g,8p,1c6,8p.,d,8p,8e,1f,p.,g,8p,8a,8b,8p,1f6,p,a,8p,8b,2c6,2d6,2e6,e,8p,8f,8g,8p,1c6,p,d6,8p,8e6,1f.6,g,8p,8g,e.6,8p,d6,8p,8g,e.6,8p,d6,8p,8g,f.6,8p,e6,8p,8d6,2c6";
													//char *song = "TakeOnMe:d=4,o=4,b=160:8f#5,8f#5,8f#5,8d5,8p,8b,8p,8e5,8p,8e5,8p,8e5,8g#5,8g#5,8a5,8b5,8a5,8a5,8a5,8e5,8p,8d5,8p,8f#5,8p,8f#5,8p,8f#5,8e5,8e5,8f#5,8e5,8f#5,8f#5,8f#5,8d5,8p,8b,8p,8e5,8p,8e5,8p,8e5,8g#5,8g#5,8a5,8b5,8a5,8a5,8a5,8e5,8p,8d5,8p,8f#5,8p,8f#5,8p,8f#5,8e5,8e5";
													//char *song = "Entertainer:d=4,o=5,b=140:8d,8d#,8e,c6,8e,c6,8e,2c.6,8c6,8d6,8d#6,8e6,8c6,8d6,e6,8b,d6,2c6,p,8d,8d#,8e,c6,8e,c6,8e,2c.6,8p,8a,8g,8f#,8a,8c6,e6,8d6,8c6,8a,2d6";
													//char *song = "Muppets:d=4,o=5,b=250:c6,c6,a,b,8a,b,g,p,c6,c6,a,8b,8a,8p,g.,p,e,e,g,f,8e,f,8c6,8c,8d,e,8e,8e,8p,8e,g,2p,c6,c6,a,b,8a,b,g,p,c6,c6,a,8b,a,g.,p,e,e,g,f,8e,f,8c6,8c,8d,e,8e,d,8d,c";
													//char *song = "Xfiles:d=4,o=5,b=125:e,b,a,b,d6,2b.,1p,e,b,a,b,e6,2b.,1p,g6,f#6,e6,d6,e6,2b.,1p,g6,f#6,e6,d6,f#6,2b.,1p,e,b,a,b,d6,2b.,1p,e,b,a,b,e6,2b.,1p,e6,2b.";
													//char *song = "Looney:d=4,o=5,b=140:32p,c6,8f6,8e6,8d6,8c6,a.,8c6,8f6,8e6,8d6,8d#6,e.6,8e6,8e6,8c6,8d6,8c6,8e6,8c6,8d6,8a,8c6,8g,8a#,8a,8f";
													//char *song = "20thCenFox:d=16,o=5,b=140:b,8p,b,b,2b,p,c6,32p,b,32p,c6,32p,b,32p,c6,32p,b,8p,b,b,b,32p,b,32p,b,32p,b,32p,b,32p,b,32p,b,32p,g#,32p,a,32p,b,8p,b,b,2b,4p,8e,8g#,8b,1c#6,8f#,8a,8c#6,1e6,8a,8c#6,8e6,1e6,8b,8g#,8a,2b";
char *song = "Bond:d=4,o=5,b=80:32p,16c#6,32d#6,32d#6,16d#6,8d#6,16c#6,16c#6,16c#6,16c#6,32e6,32e6,16e6,8e6,16d#6,16d#6,16d#6,16c#6,32d#6,32d#6,16d#6,8d#6,16c#6,16c#6,16c#6,16c#6,32e6,32e6,16e6,8e6,16d#6,16d6,16c#6,16c#7,c.7,16g#6,16f#6,g#.6";
//char *song = "MASH:d=8,o=5,b=140:4a,4g,f#,g,p,f#,p,g,p,f#,p,2e.,p,f#,e,4f#,e,f#,p,e,p,4d.,p,f#,4e,d,e,p,d,p,e,p,d,p,2c#.,p,d,c#,4d,c#,d,p,e,p,4f#,p,a,p,4b,a,b,p,a,p,b,p,2a.,4p,a,b,a,4b,a,b,p,2a.,a,4f#,a,b,p,d6,p,4e.6,d6,b,p,a,p,2b";
//char *song = "StarWars:d=4,o=5,b=45:32p,32f#,32f#,32f#,8b.,8f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32e6,8c#.6,32f#,32f#,32f#,8b.,8f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32e6,8c#6";
//char *song = "GoodBad:d=4,o=5,b=56:32p,32a#,32d#6,32a#,32d#6,8a#.,16f#.,16g#.,d#,32a#,32d#6,32a#,32d#6,8a#.,16f#.,16g#.,c#6,32a#,32d#6,32a#,32d#6,8a#.,16f#.,32f.,32d#.,c#,32a#,32d#6,32a#,32d#6,8a#.,16g#.,d#";
//char *song = "TopGun:d=4,o=4,b=31:32p,16c#,16g#,16g#,32f#,32f,32f#,32f,16d#,16d#,32c#,32d#,16f,32d#,32f,16f#,32f,32c#,16f,d#,16c#,16g#,16g#,32f#,32f,32f#,32f,16d#,16d#,32c#,32d#,16f,32d#,32f,16f#,32f,32c#,g#";
//char *song = "A-Team:d=8,o=5,b=125:4d#6,a#,2d#6,16p,g#,4a#,4d#.,p,16g,16a#,d#6,a#,f6,2d#6,16p,c#.6,16c6,16a#,g#.,2a#";
//char *song = "Flinstones:d=4,o=5,b=40:32p,16f6,16a#,16a#6,32g6,16f6,16a#.,16f6,32d#6,32d6,32d6,32d#6,32f6,16a#,16c6,d6,16f6,16a#.,16a#6,32g6,16f6,16a#.,32f6,32f6,32d#6,32d6,32d6,32d#6,32f6,16a#,16c6,a#,16a6,16d.6,16a#6,32a6,32a6,32g6,32f#6,32a6,8g6,16g6,16c.6,32a6,32a6,32g6,32g6,32f6,32e6,32g6,8f6,16f6,16a#.,16a#6,32g6,16f6,16a#.,16f6,32d#6,32d6,32d6,32d#6,32f6,16a#,16c.6,32d6,32d#6,32f6,16a#,16c.6,32d6,32d#6,32f6,16a#6,16c7,8a#.6";
//char *song = "Jeopardy:d=4,o=6,b=125:c,f,c,f5,c,f,2c,c,f,c,f,a.,8g,8f,8e,8d,8c#,c,f,c,f5,c,f,2c,f.,8d,c,a#5,a5,g5,f5,p,d#,g#,d#,g#5,d#,g#,2d#,d#,g#,d#,g#,c.7,8a#,8g#,8g,8f,8e,d#,g#,d#,g#5,d#,g#,2d#,g#.,8f,d#,c#,c,p,a#5,p,g#.5,d#,g#";
//char *song = "Gadget:d=16,o=5,b=50:32d#,32f,32f#,32g#,a#,f#,a,f,g#,f#,32d#,32f,32f#,32g#,a#,d#6,4d6,32d#,32f,32f#,32g#,a#,f#,a,f,g#,f#,8d#";
//char *song = "Smurfs:d=32,o=5,b=200:4c#6,16p,4f#6,p,16c#6,p,8d#6,p,8b,p,4g#,16p,4c#6,p,16a#,p,8f#,p,8a#,p,4g#,4p,g#,p,a#,p,b,p,c6,p,4c#6,16p,4f#6,p,16c#6,p,8d#6,p,8b,p,4g#,16p,4c#6,p,16a#,p,8b,p,8f,p,4f#";
//char *song = "MahnaMahna:d=16,o=6,b=125:c#,c.,b5,8a#.5,8f.,4g#,a#,g.,4d#,8p,c#,c.,b5,8a#.5,8f.,g#.,8a#.,4g,8p,c#,c.,b5,8a#.5,8f.,4g#,f,g.,8d#.,f,g.,8d#.,f,8g,8d#.,f,8g,d#,8c,a#5,8d#.,8d#.,4d#,8d#.";
//char *song = "LeisureSuit:d=16,o=6,b=56:f.5,f#.5,g.5,g#5,32a#5,f5,g#.5,a#.5,32f5,g#5,32a#5,g#5,8c#.,a#5,32c#,a5,a#.5,c#.,32a5,a#5,32c#,d#,8e,c#.,f.,f.,f.,f.,f,32e,d#,8d,a#.5,e,32f,e,32f,c#,d#.,c#";
//char *song = "MissionImp:d=16,o=6,b=95:32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d,32d,32d#,32e,32f,32f#,32g,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,a#,g,2d,32p,a#,g,2c#,32p,a#,g,2c,a#5,8c,2p,32p,a#5,g5,2f#,32p,a#5,g5,2f,32p,a#5,g5,2e,d#,8d";
//char *song = "Joyride:d=4,o=5,b=100:16c#7,8c#7,16p,16c#6,8a#6,16p,16a#6,16a#6,8g#6,16p,16c#6,16c#6,16c#6,16d#6,16d#6,8e#6,8e#6";
//char *song = "SuperMar:d=4,o=5,b=140:a,8f.,16c,16d,8f,f,16d,8c,8f,8f,8c6,8a.,g,8p,a,8f.,16c,16d,8f,f,16d,8c,8f,16a#,16a,16g,f,8p,8a.,8f.,8c,8a.,f,16g#,16f,8c,8g#.,g,8p,8a.,8f.,8c,8a.,f,16g#,16f,8c,c6";

int notes[] = { 0,
262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951
};

void setup() {

	digitalWrite(Out_Reset, HIGH);
#define isdigit(n) (n >= '0' && n <= '9')
	dht.begin();
	lcd.init();
	lcd.backlight();
	Serial.begin(115200);
	pinMode(In_Sensor_Brake, INPUT_PULLUP);
	pinMode(In_Button_Increase, INPUT_PULLUP);
	pinMode(In_Button_Decrease, INPUT_PULLUP);
	pinMode(In_Button_Set, INPUT_PULLUP);
	pinMode(In_Button_Resume, INPUT_PULLUP);
	pinMode(Out_Reset, OUTPUT);
	pinMode(In_Sensor_Speed, INPUT);
	pinMode(In_Pulser, OUTPUT);
	attachInterrupt(0, velocity, RISING);
	if (Flag_Clock_Set == true) {
		rtc.halt(false);
		rtc.writeProtect(false);
		rtc.setDOW(WEDNESDAY);
		rtc.setTime(hh, mm, ss);
		rtc.setDate(dd, mo, yy);
	}
	Serial.println("Ready");
}

void loop() {

	//buttons control
	if (Status_Time_Double_Push == true) {
		Status_Time_Double_Push = false;
	}
	if (digitalRead(In_Button_Decrease) == HIGH && digitalRead(In_Button_Increase) == HIGH && digitalRead(In_Button_Resume) == HIGH && digitalRead(In_Button_Set) == HIGH && digitalRead(In_Sensor_Brake) == HIGH) {
		last_millis_4 = millis();
		last_millis_7 = millis();
		Status_First_Change = true;
		Status_Time_Min_Push = false;
	}
	if ((digitalRead(In_Button_Decrease) == LOW || digitalRead(In_Button_Increase) == LOW || digitalRead(In_Button_Resume) == LOW || digitalRead(In_Button_Set) == LOW || digitalRead(In_Sensor_Brake) == LOW) && Status_First_Change == true && millis() - last_millis_7 > Time_Min_Push * 1000) {
		Status_Time_Min_Push = true;
		last_millis_4 = millis();
		last_millis_7 = millis();
	}
	if ((digitalRead(In_Button_Decrease) == LOW || digitalRead(In_Button_Increase) == LOW || digitalRead(In_Button_Resume) == LOW || digitalRead(In_Button_Set) == LOW || digitalRead(In_Sensor_Brake) == LOW) && Status_Time_Min_Push == true && millis() - last_millis_4 > Time_Double_Push * 1000) {
		Status_Time_Double_Push = true;
		last_millis_4 = millis();
	}

	//void velocity() control
	if (millis() - last_millis_8 >= acc_refresh * 1000) {
		acc = (v_ist - last_v_ist) / ((millis() - last_millis_8) / 1000);
		last_v_ist = v_ist;
		last_millis_8 = millis();
	}
	if ((millis() - last_pulse_time) / 1000 > min_pulse_time || pulse_time == 0) {
		last_v_ist = 0;
		v_ist = 0;
		acc = 0;
	}

	//timer control
	seconds = (millis() / 1000) % 60;
	if (millis() > 1000 && seconds == 0 && a == 1) {
		minutes++;
		a = 0;
	}
	if (minutes == 60 && b == 1) {
		hours++;
		minutes = 0;
		b = 0;
	}
	if (seconds != 0) {
		a = 1;
	}
	if (minutes != 0) {
		b = 1;
	}

	//clock settings
	if (millis() - last_millis_4 >= Time_Double_Push * 1000 && digitalRead(In_Button_Set) == LOW && digitalRead(In_Button_Increase) == LOW) {
		hh = rtc.getTime().hour, DEC;
		mm = rtc.getTime().min, DEC;
		ss = 0;
		rtc.setTime(hh, mm + 1, ss);
		if (mm == 59) {
			rtc.setTime(hh + 1, 0, 0);
		}
		last_millis_4 = millis();
	}
	if (millis() - last_millis_4 >= Time_Double_Push * 1000 && digitalRead(In_Button_Set) == LOW && digitalRead(In_Button_Decrease) == LOW) {
		hh = rtc.getTime().hour, DEC;
		mm = rtc.getTime().min, DEC;
		ss = 0;
		rtc.setTime(hh, mm - 1, ss);
		if (mm == 0) {
			rtc.setTime(hh - 1, 59, 0);
		}
		last_millis_4 = millis();
	}

	if ((v_ist < v_min || Status_Cruise_Control == false) && Override_Servo == false) {
		pos_servo = min_pos;
		v_soll = 0;
		last_servo_pos = min_pos;
	}

	servo_percentage = ((pos_servo - min_pos) / (max_pos - min_pos)) * 100;

	if (pos_servo > max_pos) {
		pos_servo = max_pos;
	}
	loop_time = micros() - last_millis_11;
	last_millis_11 = micros();

	//servo control
	if (Status_Servo == false && millis() > 2000) {
		throttle.attach(Out_Servo);
		Status_Servo = true;
	}
	if (((millis() - last_millis_2) >= (Time_Diff_Up * 1000)) && acc < max_acc_up && Override_Servo == false) {
		last_millis_2 = millis();
		if (v_soll - v_ist >= v_diff_up && pos_servo < max_pos) {
			pos_servo = pos_servo + step_up;
		}
	}
	if (((millis() - last_millis_3) >= (Time_Diff_Down * 1000)) && Override_Servo == false) {
		last_millis_3 = millis();
		if (v_soll - v_ist <= v_diff_down && pos_servo > min_pos) {
			pos_servo = pos_servo - step_down;
		}
	}

	Serial_Interface();
	EEPROM_Control();
	LCD_Control();
	Hardware_Control();

}

void velocity() {

	dist = dist + (wheel_circumference / pulses_pro_rotation) / 1000;
	pulse_time = (millis() - last_pulse_time) / 1000;
	counter_1++;
	last_pulse_time = millis();
	last_millis_10 = millis();
	v_ist = (((wheel_circumference / pulses_pro_rotation) / pulse_time) * 3.6);
}