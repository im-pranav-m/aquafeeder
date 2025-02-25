// LIBRARIES FOR THE CODE
#include <Arduino.h>
#include "uRTCLib.h" 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ElegantOTA.h>
#include <Servo.h>
#include <ESP8266mDNS.h> 

// CONNECTIONS
Servo feederServo;
const int RED = D7;
const int BLUE = D4;
const int SERVO = D8;
const int touch_sensor = D5;//Input sensor
const int uploadSwitch = D6;

#include "config.h"

ESP8266WebServer server(80);

// SERVO ANGLE
const int max_angle = 180; // Enter the maximum rotation you want. Default one.
const int min_angle = 70;   // 70 degrees
int number_of_pumps = 3;    // Should be at least 1.

uRTCLib rtc(0x68); 

int numb_deploy_time = sizeof(deploy_time) / sizeof(deploy_time[0]); // Calculate the length of the array...

String RTCHOUR, RTCMIN, RTCTIME;

void connections() {
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  feederServo.attach(SERVO, 500, 2400); 
  pinMode(touch_sensor, INPUT);
  pinMode(uploadSwitch,INPUT_PULLUP);
}

void dispense() {
  int angle;
  int pump;

  for (pump = 0; pump < number_of_pumps; pump++) {
    // Sweep the servo from 180 to 70 degrees
    for (angle = max_angle; angle >= min_angle; angle -= pumping_speed) {
      feederServo.write(angle); 
      delay(15);
    }
    for (angle = min_angle; angle <= max_angle; angle += pumping_speed) { 
      feederServo.write(angle); 
      delay(15); 
    }
  }
  pump = 0;
  delay(6e4);
}

void setup(void) {
  digitalWrite(BLUE,HIGH);
  digitalWrite(RED,LOW);

  Serial.begin(115200);
  URTCLIB_WIRE.begin();
  rtc.set_model(URTCLIB_MODEL_DS1307);

  connections();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  feederServo.write(180);

  if (setTimeRTC) {rtc.set(0, rtcminute, rtchour, dayOfWeek, dayOfMonth, month, year); }
  Serial.println(" ");
  if (MDNS.begin("aquafeed")) {
    Serial.println("MDNS has started");
  } else {
    Serial.println("MDNS ERROR!");
  }

  unsigned long startMillis = millis();
  unsigned long duration = 10;

  // Read the time from RTC Module
  while (millis() - startMillis < duration) {
    rtc.refresh(); 
    if (rtc.hour() < 10) {RTCHOUR = "0" + String(rtc.hour());} else {RTCHOUR = String(rtc.hour());}
    if (rtc.minute() < 10) {RTCMIN = "0" + String(rtc.minute());} else {RTCMIN = String(rtc.minute());}
    RTCTIME = RTCHOUR + ":" + RTCMIN;
    Serial.println("Current Time: " + RTCTIME); 
  }

  //Check the dispense time
  for (int check_time = 0; check_time < numb_deploy_time; check_time++) {
    if (RTCTIME == deploy_time[check_time]) {
      Serial.println("Time to dispense!");
      dispense();  
    }
  }
  
  Serial.print("Sensor Value : ");
  Serial.println(digitalRead(touch_sensor));
  if(digitalRead(touch_sensor) == 1){
    digitalWrite(RED,HIGH);
    dispense();
    digitalWrite(RED,LOW);
  }
  
if(digitalRead(uploadSwitch)== 0){
    //Setup Elegant OTA stuff
  ElegantOTA.begin(&server);
  server.begin();
  Serial.println("Elegant OTA is Running up!!");}

else{ESP.deepSleep(6e7);}
    digitalWrite(BLUE,LOW);
}
  
void loop(void) {
    digitalWrite(BLUE,LOW);
  digitalWrite(RED,HIGH);

    server.handleClient();
    MDNS.update();
    ElegantOTA.loop();
    Serial.println("Entered to Uploading mode!");
}
