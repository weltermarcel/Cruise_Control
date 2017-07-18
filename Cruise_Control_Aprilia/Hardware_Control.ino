void Hardware_Control() {

	//Pulser control
	if ((millis() - last_millis_10 >= pulser_time * 1000) && pulser_time != 0) {
		digitalWrite(In_Pulser, !digitalRead(In_Pulser));
		last_millis_10 = millis();
	}

	//buttons control
	if (digitalRead(In_Sensor_Brake) == LOW && digitalRead(In_Button_Increase) == LOW && Status_Cruise_Control == true && ((Status_Time_Double_Push == true || Status_First_Change == true) && Status_Time_Min_Push == true)) {
		Status_First_Change = false;
		Status_Cruise_Control = false;
		last_millis_4 = millis();
	}
	if (digitalRead(In_Sensor_Brake) == LOW && digitalRead(In_Button_Increase) == LOW && Status_Cruise_Control == false && ((Status_Time_Double_Push == true || Status_First_Change == true) && Status_Time_Min_Push == true)) {
		Status_First_Change = false;
		Status_Cruise_Control = true;
		last_millis_4 = millis();
	}
	if (digitalRead(In_Sensor_Brake) == LOW && digitalRead(In_Button_Set) == LOW && Status_LCD_Backlight == true && ((Status_Time_Double_Push == true || Status_First_Change == true) && Status_Time_Min_Push == true)) {
		Status_First_Change = false;
		lcd.noBacklight();
		Status_LCD_Backlight = false;
		last_millis_4 = millis();
	}
	if (digitalRead(In_Sensor_Brake) == LOW && digitalRead(In_Button_Set) == LOW && Status_LCD_Backlight == false && ((Status_Time_Double_Push == true || Status_First_Change == true) && Status_Time_Min_Push == true)) {
		Status_First_Change = false;
		lcd.backlight();
		Status_LCD_Backlight = true;
		last_millis_4 = millis();
	}
	if (digitalRead(In_Sensor_Brake) == LOW && digitalRead(In_Button_Resume) == LOW && v_ist == 0 && ((Status_Time_Double_Push == true || Status_First_Change == true) && Status_Time_Min_Push == true)) {
		Status_First_Change = false;
		play_rtttl(song);
	}
	if (digitalRead(In_Sensor_Brake) == LOW && (Status_Time_Double_Push == true || Status_First_Change == true) && Status_Time_Min_Push == true) {
		Status_First_Change = false;
		if (pos_servo > min_pos) {
			last_servo_pos = pos_servo;
			last_speed = v_soll;
		}
		v_soll = 0;
	}
	if (v_soll < v_min) {
		v_soll = 0;
	}
	if (v_soll == 0 && Override_Servo == false) {
		pos_servo = min_pos;
		pos_servo_pid = min_pos;
	}
	if (digitalRead(In_Button_Increase) == LOW && (Status_Time_Double_Push == true || Status_First_Change == true) && Status_Time_Min_Push == true) {
		Status_First_Change = false;
		v_soll++;
	}
	if (digitalRead(In_Button_Decrease) == LOW && (Status_Time_Double_Push == true || Status_First_Change == true) && Status_Time_Min_Push == true) {
		Status_First_Change = false;
		v_soll--;
	}
	if (digitalRead(In_Button_Set) == LOW && ((Status_Time_Double_Push == true || Status_First_Change == true) && Status_Time_Min_Push == true)) {
		Status_First_Change = false;
		v_soll = v_ist;
		pos_servo = min_pos + (v_ist / override_factor);
	}
	if (digitalRead(In_Button_Resume) == LOW && pos_servo < override_pos && ((Status_Time_Double_Push == true || Status_First_Change == true) && Status_Time_Min_Push == true)) {
		Status_First_Change = false;
		v_soll = last_speed;
		pos_servo = last_servo_pos;
	}

	pos_servo = constrain(pos_servo, min_pos, max_pos);
	pos_servo_pid = constrain(pos_servo_pid, min_pos, max_pos);

	if (operationMode == 1) {
		
		throttle.write(pos_servo);
	}

	if(operationMode == 2) {

		throttle.write(pos_servo_pid);
	}

}