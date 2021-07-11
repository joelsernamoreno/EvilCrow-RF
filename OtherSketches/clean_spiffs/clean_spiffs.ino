#include <EEPROM.h>
#include "SPIFFS.h"
#include "SPI.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>
WebServer server(80);

const bool formatOnFail = true;

// File
File logs;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(4096);
  SPIFFS.begin(formatOnFail);
  delay(2000);
  logs = SPIFFS.open("/logs.txt", "a+");
  logs.close();
  logs = SPIFFS.open("/logs.txt", "w");
  SPIFFS.remove("/");
  logs.close();
  logs = SPIFFS.open("/", "w");
  SPIFFS.remove("/logs.txt");
  logs.close();
}

void loop() {
  // put your main code here, to run repeatedly:

}
