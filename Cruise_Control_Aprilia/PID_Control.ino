float Kp = 5.5;
float Ki = 5.5;
float Kd = 5.5;

float p = 0;
float i = 0;
float d = 0;


float error = 0;
float lastError = 0;

float pid = 0;

float i_max = 20; //2000
float max_pid = 50;

void loop() {

	error = v_soll - v_ist;			//Error is difference between process value and set point

	p = error;						// Proportional is just the error

	i = i + error;                  //Integral

									//Optional: create a dead band so the so integrel won't hunt back and fourth
									//if(abs(error) >  1) i = i + error;  // Integrate error if error > 1
									//if(error == 0) i = 0;               //Clear intergal if zero error

	i = constrain(i, -i_max, i_max);      //Prevent i from going to +/- infinity

	d = error - lastError;        // error differential 
	lastError = error;              // Save last error for next loop

	pid = (Kp * p) + (Ki * i) + (Kd * d);  // Do PID 
	
	pos_servo_pid = constrain(pid, 0, max_pid);            //Constrain to valid PWM range -127 to 127.  zero = stopped

}

void debug()
{
	Serial.print(error);
	Serial.print("    ");
	Serial.print(Kp);
	Serial.print(" ");
	Serial.print(Ki);
	Serial.print(" ");
	Serial.print(Kd);
	Serial.print("     ");
	Serial.print(pid);
	Serial.print("     ");
	Serial.print(p);
	Serial.print(" ");
	Serial.print(i);
	Serial.print(" ");
	Serial.print(d);
	Serial.println(" ");

}