void standby() {
  if ((currenttime - standby_timer) >= standby_interval && standby_state == 0) 
  {
    if (firewood_needed == 1){
      analogWrite(brightnessPin, standby_brightness);
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Cur Temp: "); lcd.print((int)avg); lcd.print(char(178)); lcd.print("F");
    lcd.setCursor(0,1);
    lcd.print("Set Temp: "); lcd.print((int)setpoint); lcd.print(char(178)); lcd.print("F");

    standby_timer = currenttime;
    standby_state = 1;
  }
  else if ((currenttime - standby_timer) >= standby_interval && standby_state == 1) 
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Cur Humd: "); lcd.print((int)humidity); lcd.print("%");
    standby_timer = currenttime;
    if (firewood_needed == 0) {
      standby_state = 0;
    } else {
      standby_state = 2;
    }
  }
  else if ((currenttime - standby_timer) >= standby_interval && standby_state == 2)
  {
    analogWrite(brightnessPin, 240);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Firewood Needed");
    lcd.setCursor(0,1);
    lcd.print("For "); lcd.print(firewood_needed_time); lcd.print(" Minutes");
    standby_timer = currenttime;
    standby_state = 0;
  }
}



void ReadButtons() {
  if (digitalRead(uppin) == HIGH) {
    // Check whether it is time to start fast incrementing temperature
    if ((currenttime - fast_button_wait_timer) >= fast_button_wait_interval) {
      // Increment temperature every certain interval
      if ((currenttime - fast_button_timer) >= fast_button_interval) {
        fast_button_timer = millis();
        adjust_temp_timer = millis();
        send_Blynk_timer = millis();
        setpoint++;
      }
    }
    upstate = HIGH;
  }
  else {
    upstate = LOW;
  }

  if (digitalRead(downpin) == HIGH) {
    downstate = HIGH;
    // Check whether it is time to start fast incrementing temperature
    if ((currenttime - fast_button_wait_timer) >= fast_button_wait_interval) {
      // Increment temperature every certain interval
      if ((currenttime - fast_button_timer) >= fast_button_interval) {
        fast_button_timer = millis();
        adjust_temp_timer = millis();
        send_Blynk_timer = millis();
        setpoint--;
      }
    }
    downstate = HIGH;
  }
  else {
    downstate = LOW;
  }
}


void UpPinISR() {
  Serial.println("Up Button Pressed");
  setpoint++;
  adjust_temp_timer = millis();
  fast_button_wait_timer = millis();
  send_Blynk_timer = millis();
  adjust_setpoint_flag = true;
}

void DownPinISR() {
  Serial.println("Down Button Pressed");
  setpoint--;
  adjust_temp_timer = millis();
  fast_button_wait_timer = millis();
  send_Blynk_timer = millis();
  adjust_setpoint_flag = true;
}


void DisplayTemps() {
  // Function to Display Current Temps for use while setting setpoint
      // lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Cur Temp: "); lcd.print((int)avg); lcd.print(char(178)); lcd.print("F  ");
      lcd.setCursor(0,1);
      lcd.print("Set Temp: "); lcd.print((int)setpoint); lcd.print(char(178)); lcd.print("F  ");
}

void SendBlynkUpdates() {
  if ((currenttime - send_Blynk_timer) >= send_Blynk_Interval) {
    // Send Temperature Updates after they have been changed Locally

    if (adjust_setpoint_flag) {
      Blynk.virtualWrite(V4, setpoint);
      adjust_setpoint_flag = false;
    }
    // if (upstate == HIGH) {
    //   // upstate = LOW;
    //   Blynk.virtualWrite(V4, setpoint);
    // }
    // if (downstate == HIGH) {
    //   // downstate = LOW;
    //   Blynk.virtualWrite(V4, setpoint);
    // }
  }
}

void LCDFunction() {
  ReadButtons();
  SendBlynkUpdates();
  if ((currenttime - adjust_temp_timer) >= adjust_temp_interval) {
    standby();
    if (firewood_needed == 0) {
      analogWrite(brightnessPin, standby_brightness);
    }


    // Read DHT sensor
    if ((currenttime - dht11time) >= (dht11_interval*1000)) {
      ReadDHT("F");
      sum = sum + temperatureF;
      count++;
      dht11time = millis();
    }
    // Send temp update
    if ((currenttime - temptime) >= (temp_interval*1000)) {
      avg = sum/count;
      count = 0;
      sum = 0;
      // Send Temperature to App
      Serial.print("Sending temperature update: "); Serial.println(avg);
      Blynk.virtualWrite(V0, avg);
      // Necessary to send Temperature to Damper Controller
      // Bridge_to_Woodstove.virtualWrite(V0, avg);
      temptime = millis();
    }
    // Send Humidity Update
    if ((currenttime - humidtime) >= (humid_interval*1000)) {
      // Send Humidity to App
      Blynk.virtualWrite(V1, humidity);
      humidtime = millis();
    }

  }
  else {       
    DisplayTemps();
    analogWrite(brightnessPin, 1023);
  }
}