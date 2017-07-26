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

const int operationMode = 2;						//1 = incremental, 2 = PID
float Time_PID_Loop = 0.5;							//PID Loop time [s]

const int In_Sensor_Speed = 2;                      //speed sensor pin
const int Out_Pulser = 3;                           //simulate speed over Serial_Interface
const int In_Button_Resume = 4;                     //control between tone and servo control
const int In_Button_Increase = 5;                   //increase speed
const int In_Button_Decrease = 6;                   //decrease speed
const int In_Button_Set = 7;                        //set speed
const int Out_Servo = 8;                            //servo's control pin
const int In_Sensor_Brake = 9;                      //0 kmh -> to brake light
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
bool Status_Serial_Interface = true;               //serial interface on or off
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
int max_pos = 155;						            //servo's max position
int override_pos = min_pos + 5;                     //servo must be under this value to override its position according to speed
int tone_offset = 15;                               //to lower RPM's for song with servo
int min_tone_frequency = 600;                       //for song with servo (262)
int max_tone_frequency = 15000;                     //for song with servo (3951)
int pos_servo = min_pos;                            //servo's control position
int pos_servo_pid = min_pos;
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
float serial_refresh = 1.0;                         //serial.print refresh time
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
unsigned long last_millis_13 = 0;					//PID loop control

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
	pinMode(Out_Pulser, OUTPUT);
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

	if (millis() - last_millis_13 >= Time_PID_Loop * 1000) {
		last_millis_13 = millis();
		PID();
	}

	SerialEvent();
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