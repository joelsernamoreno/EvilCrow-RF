#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <RCSwitch.h>
#include <EEPROM.h>
#include "SPIFFS.h"
#include "SPI.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include "rxconfig.h"
#include "txconfig.h"
#include "bruteforce.h"

// Config SSID and password
const char* ssid = "EvilCrow-RF";  // Enter your SSID here
const char* password = "123456789";  //Enter your Password here

// HTML and CSS style
const String MENU = "<body><p>Evil Crow RF v1.0</p><div id=\"header\"><div id=\"menu\"><ul><a class=\"myButton\" href=\"/\">Menu</a><a class=\"myButton\" href=\"/rxconfig\">RX Config</a><a class=\"myButton\" href=\"/txconfig\">TX Config</a><a class=\"myButton\" href=\"/bruteforce\">Brute Force</a><a class=\"myButton\" href=\"/viewlog\">RX Log</a><a class=\"myButton\" href=\"/delete\">Delete Log</a></ul></div></div></style>";
const String HTML_CSS_STYLING = "<html><head><meta charset=\"utf-8\"><title>Evil Crow RF</title><style>     body {     background-color: #333333;     font-family: \"Century Gothic\", Arial;     color: white;     margin: 20px;}.myButton:link, .myButton:visited {background: linear-gradient(#777777, #444444);color: white;padding: 4px;min-width: 100px;border-radius: 5px;border: 2px solid white;text-align: center;margin-right: 20px;text-decoration: none;display: inline-block;transition: 0.25s;}  .myButton:hover, .myButton:active {background: linear-gradient(#888888, #555555);border: 2px solid deepskyblue;border-radius: 10px;transform: scale(1.15);}</style></head>";

// Web Server
WebServer server(80);

//CC1101 Pins
int sck = 18;
int miso = 19;
int mosi = 23;
int csn1 = 5;
int csn2 = 27;
int gdo0_1 = 2;
int gdo2_1 = 4;
int gdo0_2 = 25;
int gdo2_2 = 26;

//Pushbutton Pins
int push1 = 34;
int push2 = 35;
int push3 = 32;

//Global variables
unsigned long code;
unsigned int codelen;
unsigned int codeproto;
String tmp_module;
String tmp_frequency;
String tmp_code;
String tmp_codelen;
String tmp_setrxbw;
String tmp_firstcode;
String tmp_secondcode;
String tmp_bitlenghtattack;
int module;
float frequency;
float setrxbw;
const bool formatOnFail = true;
String webString;
int pushbutton1 = 0;
int pushbutton2 = 0;
int pushbutton3 = 0;

// File
File logs;

RCSwitch mySwitch = RCSwitch();
RCSwitch CC1101_1 = RCSwitch();
RCSwitch CC1101_2 = RCSwitch();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
  //ELECHOUSE_cc1101.setClb(1,9,11); //315MHz calibration
  //ELECHOUSE_cc1101.setClb(2,6,9); //433MHz calibration
  delay(500);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  pinMode(push1, INPUT);
  pinMode(push2, INPUT);
  pinMode(push3, INPUT);

  EEPROM.begin(4096);
  SPIFFS.begin(formatOnFail);

  logs = SPIFFS.open("/logs.txt", "a+");

  server.on("/", []() {
    server.send(200, "text/html", HTML_CSS_STYLING + MENU);
  });

  server.on("/rxconfig", []() {
    server.send_P(200, "text/html", RXConfig);
  });

  server.on("/txconfig", []() {
    server.send_P(200, "text/html", TXConfig);
  });

  server.on("/bruteforce", []() {
    server.send_P(200, "text/html", BruteForce);
  });

  server.on("/setrx", [](){
    tmp_module = server.arg("module");
    tmp_frequency = server.arg("frequency");
    tmp_setrxbw = server.arg("setrxbw");
    if (server.hasArg("configmodule")) {
      module = tmp_module.toInt();
      frequency = tmp_frequency.toFloat();
      setrxbw = tmp_setrxbw.toFloat();
      
      if(module == 1) {
        ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, csn1);
        ELECHOUSE_cc1101.setMHZ(frequency);
        ELECHOUSE_cc1101.setRxBW(setrxbw);
        ELECHOUSE_cc1101.Init();
        CC1101_1.enableReceive(gdo2_1);
        ELECHOUSE_cc1101.SetRx();
        Serial.print("Module: ");
        Serial.println(module);
        Serial.print("Frequency: ");
        Serial.println(frequency);
      }
      if(module == 2) {
        ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, csn2);
        ELECHOUSE_cc1101.setMHZ(frequency);
        ELECHOUSE_cc1101.setRxBW(setrxbw);
        ELECHOUSE_cc1101.Init();
        CC1101_2.enableReceive(gdo2_2);
        ELECHOUSE_cc1101.SetRx();
        Serial.print("Module: ");
        Serial.println(module);
        Serial.print("Frequency: ");
        Serial.println(frequency);
      }
    }
  });

  server.on("/settx", [](){
    tmp_module = server.arg("module");
    tmp_frequency = server.arg("frequency");
    tmp_code = server.arg("code");
    tmp_codelen = server.arg("bitlenght");
    if (server.hasArg("configmodule")) {
      module = tmp_module.toInt();
      frequency = tmp_frequency.toFloat();
      code = strtol(tmp_code.c_str(),NULL,0);
      codelen = tmp_codelen.toInt();

      if(module == 1) {
        //CC1101_2.disableReceive();
        ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, csn1);
        ELECHOUSE_cc1101.setMHZ(frequency);
        ELECHOUSE_cc1101.Init();
        CC1101_1.enableTransmit(gdo0_1);
        ELECHOUSE_cc1101.SetTx();
        CC1101_1.send(code, codelen);
        Serial.print("Frequency: ");
        Serial.println(frequency);
        Serial.print("Code: ");
        Serial.println(code);
        Serial.print("BIN: ");
        Serial.println(code,BIN);
        Serial.print("Codelen: ");
        Serial.println(codelen);
      }
      if(module == 2) {
        //CC1101_1.disableReceive();
        ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, csn2);
        ELECHOUSE_cc1101.setMHZ(frequency);
        ELECHOUSE_cc1101.Init();
        CC1101_2.enableTransmit(gdo0_2);
        ELECHOUSE_cc1101.SetTx();
        CC1101_2.send(code, codelen);
        Serial.print("Code: ");
        Serial.println(code);
        Serial.print("BIN: ");
        Serial.println(code,BIN);
        Serial.print("Codelen: ");
        Serial.println(codelen);
      }
    }
  });

  server.on("/setbruteforce", [](){
    tmp_frequency = server.arg("frequency");
    tmp_code = server.arg("code");
    tmp_codelen = server.arg("bitlenght");
    if (server.hasArg("bruteforce")) {
      frequency = tmp_frequency.toFloat();
      code = strtol(tmp_code.c_str(),NULL,0);
      codelen = tmp_codelen.toInt();
      //CC1101_2.disableReceive();
      delay(1000),
      ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, csn1);
      ELECHOUSE_cc1101.setMHZ(frequency);
      ELECHOUSE_cc1101.Init();
      CC1101_1.enableTransmit(gdo0_1);
      ELECHOUSE_cc1101.SetTx();     
      for(int y = 0; y < 99999999999; y++){
        CC1101_1.send(code, codelen);
        code++;
        delay(100);
      }
    }
  });

  server.on("/viewlog", [](){
    logs.close();
    String serverlog;
    serverlog += server.arg(0);
    logs = SPIFFS.open("/logs.txt");
    webString = logs.readString();
    logs.close();
    logs = SPIFFS.open("/logs.txt", "a+");
    server.send(200, "text/html", HTML_CSS_STYLING+ "View Log: "+serverlog +"\n-----\n"+webString+"</pre></h2>");
  });

  server.on("/delete", [](){
    logs.close();
    logs = SPIFFS.open("/logs.txt", "w");
    server.send(200, "text/plain", "File cleared!" );
    webString="";
    logs.close();
    logs = SPIFFS.open("/logs.txt", "a+");
  });

  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  pushbutton1 = digitalRead(push1);
  pushbutton2 = digitalRead(push2);
  pushbutton3 = digitalRead(push3);
  
  if (mySwitch.available()) {    
    code=mySwitch.getReceivedValue();
    codelen=mySwitch.getReceivedBitlength();
    codeproto=mySwitch.getReceivedProtocol();
    Serial.print("Received: ");
    Serial.print(code);
    Serial.print(" BIN: ");
    Serial.print( code ,BIN);
    Serial.print(" / ");
    Serial.print("Codelen:" );
    Serial.print( codelen );
    Serial.print("bits");
    Serial.print("Protocol: ");
    Serial.println( codeproto );
    
    logs.print("Received: ");
    logs.print(code);
    logs.print(" BIN: ");
    logs.print( code ,BIN);
    logs.print(" / ");
    logs.print("Codelen: " );
    logs.print( codelen );
    logs.print(" bits");
    logs.print(" Protocol: ");
    logs.println( codeproto );

    mySwitch.resetAvailable();
  }

  if (pushbutton1 == LOW) {
    Serial.println("1");
  }

  if (pushbutton2 == LOW) {
    Serial.println("2");
  }

  if (pushbutton3 == LOW) {
    Serial.println("3");
  }
}
