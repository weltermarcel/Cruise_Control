void EEPROM_Control() {

	if (counter_1 >= (50 * pulses_pro_rotation)) {
		counter_1 = counter_1 - (50 * pulses_pro_rotation);
		EEPROM.write(j, (EEPROM.read(j) + 1));
		EEPROM.write(i, (EEPROM.read(i) + 1));
	}
	if (EEPROM.read(i) == 10) {
		EEPROM.write(h, (EEPROM.read(h) + 1));
		EEPROM.write(i, 0);
	}
	if (EEPROM.read(j) == 50) {
		EEPROM.write(i, (EEPROM.read(i) + 1));
		EEPROM.write(j, 0);
	}
	if (EEPROM.read(h) == 10) {
		EEPROM.write(g, (EEPROM.read(g) + 1));
		EEPROM.write(h, 0);
	}
	if (digitalRead(In_Sensor_Brake) == LOW && digitalRead(In_Button_Increase) == LOW && digitalRead(In_Button_Decrease) == LOW && v_ist == 0) {
		for (z = 0; z < 512; z++)
			EEPROM.write(z, 0);
		Serial.println("EEPROM cleared");
	}
	if (millis() - last_millis_9 >= EEPROM_read_refresh * 1000) {
		dist_total = ((EEPROM.read(g)) * 10) + (EEPROM.read(h)) + ((float)(EEPROM.read(i)) / 10);
		last_millis_9 = millis();
	}
}