#include "ELECHOUSE_CC1101_SRC_DRV.h"
#include <WiFiClient.h> 
#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include "SPIFFS.h"
#include "SPI.h"
#include <WiFiAP.h>
#include "rxconfig.h"
#include "txconfig.h"

#define samplesize 1000

// Config SSID and password
const char* ssid = "RAW Replay";  // Enter your SSID here
const char* password = "123456789";  //Enter your Password here

// HTML and CSS style
const String MENU = "<body><p>Evil Crow RF v1.0</p><div id=\"header\"><div id=\"menu\"><ul><a class=\"myButton\" href=\"/\">Menu</a><a class=\"myButton\" href=\"/rxconfig\">RAW RX Config</a><a class=\"myButton\" href=\"/txconfig\">RAW TX Config</a><a class=\"myButton\" href=\"/viewlog\">RAW RX Log</a><a class=\"myButton\" href=\"/delete\">Delete Log</a></ul></div></div></style>";
const String HTML_CSS_STYLING = "<html><head><meta charset=\"utf-8\"><title>Evil Crow RF RAW</title><style>     body {     background-color: #333333;     font-family: \"Century Gothic\", Arial;     color: white;     margin: 20px;}.myButton:link, .myButton:visited {background: linear-gradient(#777777, #444444);color: white;padding: 4px;min-width: 100px;border-radius: 5px;border: 2px solid white;text-align: center;margin-right: 20px;text-decoration: none;display: inline-block;transition: 0.25s;}  .myButton:hover, .myButton:active {background: linear-gradient(#888888, #555555);border: 2px solid deepskyblue;border-radius: 10px;transform: scale(1.15);}</style></head>";

int RXPin = 26;
int Gdo0 = 25;
int txcounter = 1000; //Enter Raw Data Counter

const int minsample = 30;

unsigned long sample[samplesize];
int samplecount;
static unsigned long lastTime = 0;

String transmit = "";
long data_to_send[1000];

String tmp_module;
String tmp_frequency;
String tmp_codelen;
String tmp_setrxbw;
String tmp_mod;
int mod;
String tmp_deviation;
float deviation;
String tmp_datarate;
int datarate;
float frequency;
float setrxbw;
String raw_rx = "0";
const bool formatOnFail = true;
String webString;

// File
File logs;

WebServer controlserver(80);

void setup() {

  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  EEPROM.begin(4096);
  SPIFFS.begin(formatOnFail);

  logs = SPIFFS.open("/logs.txt", "a+");

  controlserver.on("/", []() {
    controlserver.send(200, "text/html", HTML_CSS_STYLING + MENU);
  });

  controlserver.on("/rxconfig", []() {
    controlserver.send_P(200, "text/html", RXConfig);
  });

  controlserver.on("/txconfig", []() {
    controlserver.send_P(200, "text/html", TXConfig);
  });

  controlserver.on("/settx", [](){
    raw_rx = "0";
    tmp_frequency = controlserver.arg("frequency");
    transmit = controlserver.arg("rawdata");
    tmp_deviation = controlserver.arg("deviation");
    tmp_mod = controlserver.arg("mod");

    if (controlserver.hasArg("configmodule")) {
      int counter=0;
      int pos = 0;
      frequency = tmp_frequency.toFloat();
      deviation = tmp_deviation.toFloat();
      mod = tmp_mod.toInt();
      pinMode(25,OUTPUT);
      ELECHOUSE_cc1101.setSpiPin(18, 19, 23, 27);
      ELECHOUSE_cc1101.Init();
      ELECHOUSE_cc1101.setModulation(mod);
      ELECHOUSE_cc1101.setMHZ(frequency);
      ELECHOUSE_cc1101.setDeviation(deviation);
      delay(400);
      ELECHOUSE_cc1101.SetTx();
      
      for (int i = 0; i<transmit.length(); i++){
        if (transmit.substring(i, i+1) == ","){
          data_to_send[counter]=transmit.substring(pos, i).toInt();
          pos = i+1;
          counter++;
        }
      }

      for (int i = 0; i<counter; i+=2){
        digitalWrite(25,HIGH);
        delayMicroseconds(data_to_send[i]);
        digitalWrite(25,LOW);
        delayMicroseconds(data_to_send[i+1]);
        Serial.print(data_to_send[i]);
        Serial.print(",");
      }
      Serial.println();
      }
  });

  controlserver.on("/setrx", [](){
    tmp_frequency = controlserver.arg("frequency");
    tmp_setrxbw = controlserver.arg("setrxbw");
    tmp_mod = controlserver.arg("mod");
    tmp_deviation = controlserver.arg("deviation");
    tmp_datarate = controlserver.arg("datarate");
    if (controlserver.hasArg("configmodule")) {
      frequency = tmp_frequency.toFloat();
      setrxbw = tmp_setrxbw.toFloat();
      mod = tmp_mod.toInt();
      deviation = tmp_deviation.toFloat();
      datarate = tmp_datarate.toInt();
      
      ELECHOUSE_cc1101.setSpiPin(18, 19, 23, 27);

      ELECHOUSE_cc1101.Init();
      ELECHOUSE_cc1101.setSyncMode(0);        // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
      ELECHOUSE_cc1101.setPktFormat(3);       // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX. 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins. 2 = Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX. 3 = Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.

      ELECHOUSE_cc1101.setModulation(mod);      // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
      ELECHOUSE_cc1101.setRxBW(setrxbw);
      ELECHOUSE_cc1101.setMHZ(frequency);
      ELECHOUSE_cc1101.setDeviation(deviation);   // Set the Frequency deviation in kHz. Value from 1.58 to 380.85. Default is 47.60 kHz.
      ELECHOUSE_cc1101.setDRate(datarate);           // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!

      enableReceive();
      raw_rx = "1";
    }
  });

  controlserver.on("/viewlog", [](){
    logs.close();
    String serverlog;
    serverlog += controlserver.arg(0);
    logs = SPIFFS.open("/logs.txt");
    webString = logs.readString();
    logs.close();
    logs = SPIFFS.open("/logs.txt", "a+");
    controlserver.send(200, "text/html", HTML_CSS_STYLING+ "View Log: "+serverlog +"\n-----\n"+webString+"</pre></h2>");
  });

  controlserver.on("/delete", [](){
    logs.close();
    logs = SPIFFS.open("/logs.txt", "w");
    controlserver.send(200, "text/plain", "File cleared!" );
    webString="";
    logs.close();
    logs = SPIFFS.open("/logs.txt", "a+");
  });

  controlserver.begin();
}

bool checkReceived(void){
  delay(1);
  if (samplecount >= minsample && micros()-lastTime >100000){
    detachInterrupt(RXPin);
    return 1;
  }else{
    return 0;
  }
}

void enableReceive(){
  pinMode(RXPin,INPUT);
  RXPin = digitalPinToInterrupt(RXPin);
  ELECHOUSE_cc1101.SetRx();
  samplecount = 0;
  attachInterrupt(RXPin, receiver, CHANGE);
}

void receiver() {
  const long time = micros();
  const unsigned int duration = time - lastTime;

  if (duration > 100000){
  samplecount = 0;
  }

  sample[samplecount++] = duration;

  if (samplecount>=samplesize){
    detachInterrupt(RXPin);
    checkReceived();
  }
  lastTime = time;
}

void loop() {
  controlserver.handleClient();

  if(raw_rx == "1") {
    if(checkReceived()){
      Serial.print("Count=");
      Serial.println(samplecount);
      Serial.println("");
      for (int i = 1; i<samplecount; i++){
        Serial.print(sample[i]);
        Serial.print(",");
        logs.print(sample[i]);
        logs.print(",");  
      }
      logs.println(" ///////// ");
      enableReceive();
      //ELECHOUSE_cc1101.setSidle();
    }
  } 
}
