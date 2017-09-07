float Kp = 2.00;
float Ki = 0.25;
float Kd = 0.00;

float ppp = 0.0;
float iii = 0.0;
float ddd = 0.0;

float deadband = 1.0;
float error = 0.0;
float lastError = 0.0;

float pid = 0.0;

float i_max = max_pos;
float i_min = min_pos;
float pid_max = (float)max_pos * 0.65;
void PID() {

	if (v_soll == 0) {
		error = 0;
		ppp = 0;
		iii = 0;
		ddd = 0;
	}

	if (v_soll != 0) {

	error = v_soll - v_ist;
	}

	
	if (abs(error) >= deadband) {

		ppp = error;

		iii = iii + error;
		iii = constrain(iii, i_min, i_max);

		ddd = error - lastError;
		lastError = error;

		pid = (Kp * ppp) + (Ki * iii) + (Kd * ddd);
	}
	
	if (Override_Servo == 0) {
		pos_servo_pid = constrain(pid, min_pos, pid_max);
	}
}