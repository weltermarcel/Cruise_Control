void Serial_Interface() {

	if (Serial.available() > 0) {
		command = intEinlesen("command: ");
	}
	if (command == 1) {
		Serial.println();
		Serial.println("Commands:");
		Serial.println("1: Commands list");
		Serial.println("2: Status_Serial_Interface");
		Serial.println("3: clear EEPROM");
		Serial.println("4: pulser time");
		Serial.println("98: play song with servo");
		Serial.println("99: auto reset");
		command = 0;
	}
	if (command == 2) {
		Status_Serial_Interface = doubleEinlesen("Status_Serial_Interface: ");
		command = 0;
	}
	if (command == 3) {
		for (z = 0; z < 512; z++)
			EEPROM.write(z, 0);
		Serial.println("EEPROM cleared");
		command = 0;
	}
	if (command == 4) {
		pulser_time = doubleEinlesen("pulser_time: ");
		command = 0;
	}
	if (command == 98) {
		Flag_Servo_Music = true;
		play_rtttl(song);
		command = 0;
		Flag_Servo_Music = false;
	}
	if (command == 99) {
		delay(Time_Delay_Reset * 1000);
		digitalWrite(Out_Reset, LOW);
		command = 0;
	}
	if (Status_Serial_Interface == true) {
		if (millis() - last_millis_1 >= serial_refresh * 1000) {
			Serial.print("Time: ");
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
			last_millis_1 = millis();
		}
	}
}