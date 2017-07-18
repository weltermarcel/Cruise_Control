float Kp = 5.5;
float Ki = 5.5;
float Kd = 5.5;

float ppp = 0;
float iii = 0;
float ddd = 0;


float error = 0;
float lastError = 0;

float pid = 0;

float i_max = 20; //2000
float pid_max = 50;

void PID() {

	error = v_soll - v_ist;			//Error is difference between process value and set point

	ppp = error;					// Proportional is just the error

	iii = iii + error;              //Integral

									//Optional: create a dead band so the so integrel won't hunt back and fourth
									//if(abs(error) >  1) i = i + error;  // Integrate error if error > 1
									//if(error == 0) i = 0;               //Clear intergal if zero error

	iii = constrain(iii, -i_max, i_max);      //Prevent i from going to +/- infinity

	ddd = error - lastError;        // error differential 
	lastError = error;              // Save last error for next loop

	pid = (Kp * ppp) + (Ki * iii) + (Kd * ddd);  // Do PID 
	
	if (Override_Servo == 0) {

		pos_servo_pid = constrain(pid, min_pos, pid_max);            //Constrain
	}

}

void debug() {

	Serial.print("error: ");
	Serial.println(error);
	Serial.print("Kp: ");
	Serial.println(Kp);
	Serial.print("Ki: ");
	Serial.println(Ki);
	Serial.print("Kd: ");
	Serial.println(Kd);
	Serial.print("pid: ");
	Serial.println(pid);
	Serial.print("p: ");
	Serial.println(ppp);
	Serial.print("i: ");
	Serial.println(iii);
	Serial.print("d: ");
	Serial.println(ddd);
	Serial.println();

}