// #define BLYNK_TEMPLATE_ID "TMPLLLP4q0VX"
// #define BLYNK_DEVICE_NAME "LCD Temperature  Humidity Sensor"
// #define BLYNK_AUTH_TOKEN "hXzUhQefoOO-RkMPTkciXj5eiIWdwZKV"

#define BLYNK_TEMPLATE_ID "TMPLLLP4q0VX"
#define BLYNK_DEVICE_NAME "WoodstoveThermostat"
#define BLYNK_AUTH_TOKEN "rXIt7yxsC1lhZh8VYo_JzcNMJFTLKn-k"

#include <Arduino.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
// #include <ESP8266HTTPClient.h>
#include <jled.h>
// #include <ESP8266mDNS.h>
// #include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "OTA.h"
#include <LiquidCrystal_I2C.h>


//******************************Setup JLed Modes***************************************
JLed blink = JLed(LED_BUILTIN).Blink(500, 500).Forever().LowActive();
JLed breathe = JLed(LED_BUILTIN).Breathe(1600).Forever().LowActive();
JLed On = JLed(LED_BUILTIN).On().LowActive();
JLed Off = JLed(LED_BUILTIN).Off().LowActive();
//*************************************************************************************

// *************************** WiFi Setup **********************************************
// Home WiFi
#define wifi_ssid  "SpottsWiFi"
#define wifi_pass  "Nspotts42194"
// Phone Hotspot
// #define wifi_ssid "natespott"
// #define wifi_pass "Nspotts42194"

int wifiTimeout = 8000;

// *************************************************************************************

// ****************************** Blynk Setup ******************************************
#define BLYNK_PRINT Serial

#include <BlynkSimpleEsp8266.h>
// char temp_auth[] = "b41c806b93a74d558a2cbcc61c82b04a";
// char stove_auth[] = "f11aca9b143a4f65abfa450369e8ff4c";
// Specify the virtualPin on this ESP8266
// WidgetBridge Bridge_to_Woodstove(V20);
BlynkTimer Timer;
// *************************************************************************************

// ********************************* DHT11 Configuration *******************************
#include <SimpleDHT.h>

int pinDHT11 = D5;
SimpleDHT11 dht11(pinDHT11);

byte temperature = 0;
float temperatureF = 0;
byte humidity = 30;
int err = SimpleDHTErrSuccess;
#include "function.h"
// *************************************************************************************

// ******************************** Logic Timing Setup *********************************
long currenttime = 0;
long temptime = 0;
long humidtime = 0;
long dht11time = 0;
int temp_interval = 5;  // 10 seconds
int humid_interval = 10;  // 5 seconds
float dht11_interval = 3.0;  // 1.5 seconds

float count = 0;
float sum = 0;
float avg = 75;
// *************************************************************************************


// ***************************** LCD Setup **************************************
/*
 The circuit:
 * LCD RS pin to digital pin D2
 * LCD Enable pin to digital pin D1
 * LCD D4 pin to digital pin D4
 * LCD D5 pin to digital pin D5
 * LCD D6 pin to digital pin D6
 * LCD D7 pin to digital pin D7
 * LCD R/W pin to ground
 * 10K Pot: ends to +5V and ground, wiper to LCD VO pin (pin 3)
 * A to 5V
 * K to Collector of transistor
 * Emitter of transistor to GND
 * Base -> 2k res. -> D3
*/

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int brightnessPin = D8;
// SDA -> D2
// SCL -> D1
LiquidCrystal_I2C lcd(0x27,16,2);

int setpoint = 0;

long standby_interval = 3000;   // ms
long standby_timer = 0;
int standby_state = 0;
int standby_brightness = 100;

int uppin = D7;
bool upstate = LOW;
int downpin = D6;
bool downstate = LOW;

long adjust_temp_timer = 0;
// Standby Timer
long adjust_temp_interval = 2000;   // ms
bool adjust_setpoint_flag = false;

long fast_button_wait_timer = 0;
long fast_button_wait_interval = 800; // ms
long fast_button_timer = 0;
long fast_button_interval = 100;  // ms

long slow_button_wait_timer = 0;
long slow_button_wait_interval = 10;

long send_Blynk_timer = 0;
long send_Blynk_Interval = 1000;

int firewood_needed = 0;
int firewood_needed_time = 0;

long t1 = 0;
long t2 = millis();

#include "LCD_Functions.h"
// ********************************************************************************


#include "Callbacks.h"

BLYNK_CONNECTED() {
  Serial.println("Syncing All");
  Blynk.syncAll();
  t1 = millis();
  // Bridge_to_Woodstove.setAuthToken(stove_auth);
}


void setup() {

  Serial.begin(115200);
  while (! Serial);
  t2 = millis();

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // pinMode(uppin, INPUT);
  // pinMode(downpin, INPUT);
  pinMode(brightnessPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(uppin), UpPinISR, RISING);
  attachInterrupt(digitalPinToInterrupt(downpin), DownPinISR, RISING);
  // Blynk.begin(BLYNK_AUTH_TOKEN, wifi_ssid, wifi_pass, IPAddress(159,65,55,83), 8080); // use IP address of blynk.cloud, use port 8080
  // Blynk.begin(BLYNK_AUTH_TOKEN, wifi_ssid, wifi_pass, "ny3.blynk.cloud", 80); // use IP address of blynk.cloud, use port 8080
  Blynk.begin(BLYNK_AUTH_TOKEN, wifi_ssid, wifi_pass, IPAddress(64,225,16,22), 8080); // use IP address of blynk.cloud, use port 80
  printWifiStatus();

  OTA_Functions();

  // initialize the lcd 
  lcd.init();     
  // Activate Backlight - Not sure if necessary
  lcd.backlight();
}


void loop() {
  currenttime = millis();

  // if (currenttime - t2 > 10000) {
  //   Serial.println("Not connected to blynk server, resetting");
  //   ESP.wdtDisable();
  //   while (true){};
  // }

  // Temperature and Humidity Update Logic is within LCDFunction()
  LCDFunction();
  ArduinoOTA.handle();
  Blynk.run();
  
  if (!Blynk.connected()) {
    Serial.println("Wifi Disconnected");
    Off.Update();
    // Try to reconnect to preconfigured WiFi indefinitely (restarts module every 8s to refresh)
    ConnectWifi();
  }
  else {
    breathe.Update();
  }

}

