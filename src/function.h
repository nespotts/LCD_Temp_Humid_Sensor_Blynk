
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  //  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


void ConnectWifi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(WiFi.SSID());
  WiFi.persistent(false);  // May be able to set to true disables automatic saving of wifi credentials
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass);

  int t1 = millis();
  while (!Blynk.connected()) {
    int t2 = millis();
    delay(500);
    Serial.print(".");  
    if ((t2-t1) > wifiTimeout) {
      ESP.wdtDisable();
      while (true){};
    }
  }

  Serial.println("\nConnected to wifi");
  printWifiStatus();
}

void ReadDHT(String degree_scale) {
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }

  if (degree_scale == "C") {
    Serial.print("Temperature is: "); Serial.print((int)temperature); Serial.println(" Degrees C");
  } else { // in F
    temperatureF = ((float)temperature * (float)9/(float)5) + (float)32;
    Serial.print("Temperature is: "); Serial.print(temperatureF); Serial.println(" Degrees F");
  }
  Serial.print("Humidity is: "); Serial.print((int)humidity); Serial.println("%");
}

