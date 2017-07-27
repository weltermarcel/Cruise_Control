int i_input_string = 0;
int last_input_string = i_input_string;
int string_int_1;
int string_int_2;
int string_int_3;
float string_float_1;
float string_float_2;
float string_float_3;
bool flag_input_complete = false;
String Input_String[15];

void SerialEvent() {																//checks if serial information is available and compiles it in an array

	if (Serial.available() > 0 && flag_input_complete != true) {

		char inChar = (char)Serial.read();
		if (inChar == '(' || inChar == ')' || inChar == '\n' || inChar == ',') {
			last_input_string = i_input_string;
			i_input_string = 4;
		}
		Input_String[i_input_string] += inChar;
		if (i_input_string == 4) {
			i_input_string = last_input_string + 1;
		}
		if (inChar == '\n') {
			flag_input_complete = true;
		}
	}
	Serial_Control();																//calls Serial_Control after new information is received
}

void Serial_Control() {																//compares the received information with possible commands

	if (Input_String[0] == "help" && flag_input_complete == true) {					//lists all recognizable commands when sent over the virtual com port

		Serial.println();
		Serial.println("Commands:");
		Serial.println();
		Serial.println("serial_debugging(0/1)");
		Serial.println("clear memory");
		Serial.println("pulser_time(float)");
		Serial.print("servo_override(0/1, pos["); Serial.print(min_pos); Serial.print(","); Serial.print(max_pos); Serial.println("]");
		Serial.println("kp(float)");
		Serial.println("ki(float)");
		Serial.println("kd(float)");
		Serial.println("i_max(float)");
		Serial.println("pid_max(float)");
		Serial.println("reset");
	}

	/*	if (Input_String[0] == "serial_debugging" && flag_input_complete == true) {
	string_int_1 = Input_String[1].toInt();
	Status_Serial_Debugging = string_int_1;
	}

	if (Input_String[0] == "clear memory" && flag_input_complete == true) {
	for (z = 0; z < 512; z++)
	EEPROM.write(z, 0);
	}

	if (Input_String[0] == "pulser_time" && flag_input_complete == true) {
	string_float_1 = Input_String[1].toFloat();
	pulser_time = string_float_1;
	}

	if (Input_String[0] == "reset" && flag_input_complete == true) {
	delay(Time_Delay_Reset * 1000);
	digitalWrite(Out_Reset, LOW);
	}

	if (Input_String[0] == "servo_override" && flag_input_complete == true) {
	string_int_1 = Input_String[1].toInt();
	string_int_2 = Input_String[2].toInt();
	Override_Servo = string_int_1;
	pos_servo = string_int_2;
	pos_servo_pid = string_int_2;
	}

	*/

	if (Input_String[0] == "kp" && flag_input_complete == true) {
		string_float_1 = Input_String[1].toFloat();
		Kp = string_float_1;
	}

	if (Input_String[0] == "kp_plus" && flag_input_complete == true) {
		Kp = Kp + increment_small;
	}

	if (Input_String[0] == "kp_minus" && flag_input_complete == true) {
		Kp = Kp - increment_small;
	}

	if (Input_String[0] == "ki_plus" && flag_input_complete == true) {
		Ki = Ki + increment_small;
	}

	if (Input_String[0] == "ki_minus" && flag_input_complete == true) {
		Ki = Ki - increment_small;
	}

	if (Input_String[0] == "kd_plus" && flag_input_complete == true) {
		Kd = Kd + increment_small;
	}

	if (Input_String[0] == "kd_minus" && flag_input_complete == true) {
		Kd = Kd - increment_small;
	}

	if (Input_String[0] == "i_min_plus" && flag_input_complete == true) {
		i_min = i_min + increment_big;
	}

	if (Input_String[0] == "i_min_minus" && flag_input_complete == true) {
		i_min = i_min - increment_big;
	}

	if (Input_String[0] == "pid_max_plus" && flag_input_complete == true) {
		pid_max = pid_max + increment_small;
	}

	if (Input_String[0] == "pid_max_minus" && flag_input_complete == true) {
		pid_max = pid_max - increment_small;
	}

	if (Input_String[0] == "ki" && flag_input_complete == true) {
		string_float_1 = Input_String[1].toFloat();
		Ki = string_float_1;
	}

	if (Input_String[0] == "kd" && flag_input_complete == true) {
		string_float_1 = Input_String[1].toFloat();
		Kd = string_float_1;
	}

	if (Input_String[0] == "i_max" && flag_input_complete == true) {
		string_float_1 = Input_String[1].toFloat();
		i_max = string_float_1;
	}

	if (Input_String[0] == "pid_max" && flag_input_complete == true) {
		string_float_1 = Input_String[1].toFloat();
		pid_max = string_float_1;
	}

	if (flag_input_complete) {														//deletes information on Input_String array

		Input_String[0] = "";
		Input_String[1] = "";
		Input_String[2] = "";
		Input_String[3] = "";
		Input_String[4] = "";
		i_input_string = 0;
		flag_input_complete = false;
	}
}


void Serial_Debugging() {

	if (Status_Serial_Debugging == true) {
		if (millis() - last_millis_1 >= serial_refresh * 1000) {
			last_millis_1 = millis();
			/*Serial.print("Time: ");
			Serial.print(rtc.getTime().hour, DEC);
			Serial.print(": ");
			Serial.print(rtc.getTime().min, DEC);
			Serial.print(": ");
			Serial.println(rtc.getTime().sec, DEC);
			Serial.print("EEPROM 0: ");
			Serial.println(EEPROM.read(0));
			Serial.print("EEPROM 1: ");
			Serial.println(EEPROM.read(1));
			Serial.print("EEPROM 2: ");
			Serial.println(EEPROM.read(2));
			Serial.print("EEPROM 3: ");
			Serial.println(EEPROM.read(3));
			Serial.print("EEPROM 4: ");
			Serial.println(EEPROM.read(4));
			Serial.print("Run time: ");
			Serial.println(millis());
			Serial.print("Actual dist: ");
			Serial.println(dist);
			Serial.print("Total  dist: ");
			Serial.println(dist_total);
			Serial.print("v_ist: ");
			Serial.println(v_ist);
			Serial.print("v_min: ");
			Serial.println(v_min);
			Serial.print("last_speed: ");
			Serial.println(last_speed);
			Serial.print("pulser_time: ");
			Serial.println(pulser_time, 3);
			Serial.print("loop_time: ");
			Serial.println(loop_time, 3);
			Serial.println();
			last_millis_1 = millis();*/
			PID_debug();

			if (operationMode == 1) {
				Serial.print("pos_servo: ");
				Serial.println(pos_servo);
				Serial.println();
			}

			else {
				Serial.print("v_ist: ");
				Serial.println(v_ist);
				Serial.print("v_soll: ");
				Serial.println(v_soll);
				Serial.print("pos_servo_pid: ");
				Serial.println(pos_servo_pid);
				Serial.println();
			}
		}
	}
}



void PID_debug() {

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