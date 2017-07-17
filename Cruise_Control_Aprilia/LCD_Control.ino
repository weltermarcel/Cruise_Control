void LCD_Control() {

	if (millis() - last_millis_5 >= screen_refresh * 1000) {
		//lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Speed: ");
		lcd.print(v_ist, 0);
		if (v_ist < 1000) {
			lcd.print("    ");
		}
		lcd.setCursor(15, 0);
		if ((rtc.getTime().hour) < 10) {
			lcd.print(" ");
		}
		lcd.print(rtc.getTime().hour);
		lcd.print(":");
		if ((rtc.getTime().min) < 10) {
			lcd.print("0");
		}
		lcd.print(rtc.getTime().min);
		lcd.setCursor(0, 1);
		lcd.print("S.Set: ");
		if (Status_Cruise_Control == false) {
			lcd.print("-");
		}
		else {
			lcd.print(v_soll, 0);
		}
		if (v_soll < 1000) {
			lcd.print("  ");
		}
		lcd.setCursor(12, 1);
		if (millis() <= Time_Switch_Temp_Hum * 1000 || millis() - last_millis_12 >= Time_Switch_Temp_Hum * 1000) {
			if (Flag_Show_Temp == true) {
				Flag_Show_Temp = false;
				last_millis_12 = millis();
				temp = dht.readTemperature();
				lcd.print("T:");
				if (temp < -9) {
					lcd.print(temp, 1);
				}
				if (temp < 0) {
					lcd.print(" ");
					lcd.print(temp, 1);
				}
				if (temp < 10) {
					lcd.print("  ");
				}
				else {
					lcd.setCursor(15, 1);
				}
				lcd.print(temp, 1);
				lcd.setCursor(19, 1);
				lcd.print("C");
			}
			if (millis() - last_millis_12 >= Time_Switch_Temp_Hum * 1000 && Flag_Show_Temp == false) {
				Flag_Show_Temp = true;
				last_millis_12 = millis();
				hum = dht.readHumidity();
				lcd.print("H: ");
				lcd.print(hum, 1);
				lcd.setCursor(19, 1);
				lcd.print("%");
			}
		}

		lcd.setCursor(0, 2);
		lcd.print("Dist:");
		lcd.setCursor(0, 3);
		lcd.print(dist);
		lcd.print("/");
		lcd.print(dist_total, 1);
		lcd.print("     ");
		lcd.setCursor(12, 2);
		lcd.print("E. Time:");
		if (hours > 0) {
			lcd.setCursor(13, 3);
			lcd.print(hours);
			lcd.print(":");
		}
		lcd.setCursor(15, 3);
		if (minutes < 10) {
			lcd.print(0);
		}
		lcd.print(minutes);
		lcd.print(":");
		if (seconds < 10) {
			lcd.print(0);
		}
		lcd.print(seconds);
		last_millis_5 = millis();
	}
}