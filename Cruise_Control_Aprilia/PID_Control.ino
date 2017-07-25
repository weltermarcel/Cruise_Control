float Kp = 0.5;
float Ki = 0.5;
float Kd = 0.5;

float increment_small = 0.05;
float increment_big = 2;

float ppp = 0.0;
float iii = 0.0;
float ddd = 0.0;

float deadband = 1.0;
float error = 0.0;
float lastError = 0.0;

float pid = 0.0;

float i_max = max_pos;
float i_min = 0.0;
float pid_max = (float)max_pos * 0.65;
void PID() {

	if (v_soll == 0) {
		error = 0;
		ppp = 0;
		iii = 0;
		ddd = 0;
	}

	if (v_soll != 0) {

		/*if (v_ist < 55) {
		Kp = 1.2;
		Ki = 0.8;
		Kd = 0.3;
		}

		if (v_ist >= 55) {
		Kp = 1.2;
		Ki = 1.0;
		Kd = 0.3;
		}*/

	error = v_soll - v_ist;
	
	}

	ppp = error;					// Proportional is just the error
	
	if (abs(error) >= deadband) {
		iii = iii + error;
	}

	iii = constrain(iii, i_min, i_max);      //Prevent i from going to +/- infinity

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