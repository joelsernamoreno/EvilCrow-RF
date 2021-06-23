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
#include "txbinary.h"

#define samplesize 1000

// Config SSID and password
const char* ssid = "RAW Replay v2";  // Enter your SSID here
const char* password = "123456789";  //Enter your Password here

// HTML and CSS style
const String MENU = "<body><p>Evil Crow RF v1.0</p><div id=\"header\"><div id=\"menu\"><ul><a class=\"myButton\" href=\"/\">Menu</a><a class=\"myButton\" href=\"/rxconfig\">RAW RX Config</a><a class=\"myButton\" href=\"/txconfig\">RAW TX Config</a><a class=\"myButton\" href=\"/txbinary\">Binary TX Config</a><a class=\"myButton\" href=\"/viewlog\">RAW RX Log</a><a class=\"myButton\" href=\"/delete\">Delete Log</a></ul></div></div></style>";
const String HTML_CSS_STYLING = "<html><head><meta charset=\"utf-8\"><title>Evil Crow RF RAW</title><style>     body {     background-color: #333333;     font-family: \"Century Gothic\", Arial;     color: white;     margin: 20px;}.myButton:link, .myButton:visited {background: linear-gradient(#777777, #444444);color: white;padding: 4px;min-width: 100px;border-radius: 5px;border: 2px solid white;text-align: center;margin-right: 20px;text-decoration: none;display: inline-block;transition: 0.25s;}  .myButton:hover, .myButton:active {background: linear-gradient(#888888, #555555);border: 2px solid deepskyblue;border-radius: 10px;transform: scale(1.15);}</style></head>";

int error_toleranz = 200;

int RXPin = 26;
int Gdo0 = 25;
const int minsample = 30;
unsigned long sample[samplesize];
unsigned long samplesmooth[samplesize];
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
String bindata;
int samplepulse;
String tmp_samplepulse;
String tmp_transmissions;
int transmissions;

// File
File logs;

WebServer controlserver(80);

void setup() {

  Serial.begin(38400);

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

  controlserver.on("/txbinary", []() {
    controlserver.send_P(200, "text/html", TXBinary);
  });

  controlserver.on("/settx", [](){
    raw_rx = "0";
    tmp_frequency = controlserver.arg("frequency");
    transmit = controlserver.arg("rawdata");
    tmp_deviation = controlserver.arg("deviation");
    tmp_mod = controlserver.arg("mod");
    tmp_transmissions = controlserver.arg("transmissions");

    if (controlserver.hasArg("configmodule")) {
      int counter=0;
      int pos = 0;
      frequency = tmp_frequency.toFloat();
      deviation = tmp_deviation.toFloat();
      mod = tmp_mod.toInt();
      transmissions = tmp_transmissions.toInt();
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

      for (int r = 0; r<transmissions; r++) {
        for (int i = 0; i<counter; i+=2){
          digitalWrite(25,HIGH);
          delayMicroseconds(data_to_send[i]);
          digitalWrite(25,LOW);
          delayMicroseconds(data_to_send[i+1]);
          Serial.print(data_to_send[i]);
          Serial.print(",");
        }
        delay(2000); //Set this for the delay between retransmissions
      } 
      Serial.println();
      }
  });

  controlserver.on("/settxbinary", [](){
    raw_rx = "0";
    tmp_frequency = controlserver.arg("frequency");
    bindata = controlserver.arg("binarydata");
    tmp_deviation = controlserver.arg("deviation");
    tmp_mod = controlserver.arg("mod");
    tmp_samplepulse = controlserver.arg("samplepulse");
    tmp_transmissions = controlserver.arg("transmissions");

    if (controlserver.hasArg("configmodule")) {
      int counter=0;
      int pos = 0;
      frequency = tmp_frequency.toFloat();
      deviation = tmp_deviation.toFloat();
      mod = tmp_mod.toInt();
      samplepulse = tmp_samplepulse.toInt();
      transmissions = tmp_transmissions.toInt();
      pinMode(25,OUTPUT);
      ELECHOUSE_cc1101.setSpiPin(18, 19, 23, 27);
      ELECHOUSE_cc1101.Init();
      ELECHOUSE_cc1101.setModulation(mod);
      ELECHOUSE_cc1101.setMHZ(frequency);
      ELECHOUSE_cc1101.setDeviation(deviation);
      delay(400);
      ELECHOUSE_cc1101.SetTx();
      
      for (int i=0; i<1000; i++){
        data_to_send[i]=0;
      }

      bindata.replace(" ","");
      bindata.replace("\n","");
      bindata.replace("Pause:","");
      int count_binconvert=0;
      String lastbit_convert="1";

      for (int i = 0; i<bindata.length()+1; i++){
        if (lastbit_convert != bindata.substring(i, i+1)){
          if (lastbit_convert == "1"){
            lastbit_convert="0";
          }else if (lastbit_convert == "0"){
            lastbit_convert="1";
          }
          count_binconvert++;
        }
    
        if (bindata.substring(i, i+1)=="["){
          data_to_send[count_binconvert]= bindata.substring(i+1,bindata.indexOf("]",i)).toInt();
          lastbit_convert="0";
          i+= bindata.substring(i,bindata.indexOf("]",i)).length();
        }else{
          data_to_send[count_binconvert]+=samplepulse;
        }
      }

      for (int i = 0; i<count_binconvert; i++){
        Serial.print(data_to_send[i]);
        Serial.print(",");
      }

      for (int r = 0; r<transmissions; r++) {
        for (int i = 0; i<count_binconvert; i+=2){
          digitalWrite(25,HIGH);
          delayMicroseconds(data_to_send[i]);
          digitalWrite(25,LOW);
          delayMicroseconds(data_to_send[i+1]);
        }
        delay(2000); //Set this for the delay between retransmissions    
      }
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

void loop() {
  controlserver.handleClient();

  if(raw_rx == "1") {
    if(checkReceived()){
      printReceived();
      signalanalyse();
      enableReceive();
    }
  }
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

void printReceived(){
  
  Serial.print("Count=");
  Serial.println(samplecount);
  logs.println("<br>");
  logs.println("<br>");
  logs.print("Count=");
  logs.println(samplecount);
  logs.println("<br>");
  
  for (int i = 1; i<samplecount; i++){
    Serial.print(sample[i]);
    Serial.print(",");
    logs.print(sample[i]);
    logs.print(",");   
  }
  Serial.println();
  Serial.println();
  logs.println("<br>");
  logs.println("<br>");
}

void enableReceive(){
  
  pinMode(RXPin,INPUT);
  RXPin = digitalPinToInterrupt(RXPin);
  ELECHOUSE_cc1101.SetRx();
  samplecount = 0;
  attachInterrupt(RXPin, receiver, CHANGE);
}

void signalanalyse(){
  #define signalstorage 10

  int signalanz=0;
  int timingdelay[signalstorage];
  float pulse[signalstorage];
  long signaltimings[signalstorage*2];
  int signaltimingscount[signalstorage];
  long signaltimingssum[signalstorage];
  long signalsum=0;

  for (int i = 0; i<signalstorage; i++){
    signaltimings[i*2] = 100000;
    signaltimings[i*2+1] = 0;
    signaltimingscount[i] = 0;
    signaltimingssum[i] = 0;
  }

  for (int i = 1; i<samplecount; i++){
    signalsum+=sample[i];
  }

  for (int p = 0; p<signalstorage; p++){

  for (int i = 1; i<samplecount; i++){
    if (p==0){
      if (sample[i]<signaltimings[p*2]){
        signaltimings[p*2]=sample[i];
      }
    }else{
      if (sample[i]<signaltimings[p*2] && sample[i]>signaltimings[p*2-1]){
        signaltimings[p*2]=sample[i];
      }
    }
  }

  for (int i = 1; i<samplecount; i++){
    if (sample[i]<signaltimings[p*2]+error_toleranz && sample[i]>signaltimings[p*2+1]){
      signaltimings[p*2+1]=sample[i];
    }
  }

  for (int i = 1; i<samplecount; i++){
    if (sample[i]>=signaltimings[p*2] && sample[i]<=signaltimings[p*2+1]){
      signaltimingscount[p]++;
      signaltimingssum[p]+=sample[i];
    }
  }
  }

  signalanz=signalstorage;
  for (int i = 0; i<signalstorage; i++){
    if (signaltimingscount[i] == 0){
      signalanz=i;
      i=signalstorage;
    }
  }

  for (int s=1; s<signalanz; s++){
  for (int i=0; i<signalanz-s; i++){
    if (signaltimingscount[i] < signaltimingscount[i+1]){
      int temp1 = signaltimings[i*2];
      int temp2 = signaltimings[i*2+1];
      int temp3 = signaltimingssum[i];
      int temp4 = signaltimingscount[i];

      signaltimings[i*2] = signaltimings[(i+1)*2];
      signaltimings[i*2+1] = signaltimings[(i+1)*2+1];
      signaltimingssum[i] = signaltimingssum[i+1];
      signaltimingscount[i] = signaltimingscount[i+1];

      signaltimings[(i+1)*2] = temp1;
      signaltimings[(i+1)*2+1] = temp2;
      signaltimingssum[i+1] = temp3;
      signaltimingscount[i+1] = temp4;
    }
  }
  }

  for (int i=0; i<signalanz; i++){
    timingdelay[i] = signaltimingssum[i]/signaltimingscount[i];
  }

  bool lastbin=0;
  for (int i=1; i<samplecount; i++){
    float r = (float)sample[i]/timingdelay[0];
    int calculate = r;
    r = r-calculate;
    r*=10;
    if (r>=5){calculate+=1;}
    if (calculate>0){
      if (lastbin==0){
        lastbin=1;
      }else{
      lastbin=0;
    }
      if (lastbin==0 && calculate>8){
        Serial.print(" [Pause: ");
        Serial.print(sample[i]);
        Serial.println(" samples]");
        logs.print(" [Pause: ");
        logs.print(sample[i]);
        logs.println(" samples]");
      }else{
        for (int b=0; b<calculate; b++){
          Serial.print(lastbin);
          logs.print(lastbin);
        }
      }
    }
  }
  logs.println("<br>");
  logs.println("<br>");
  Serial.println();
  Serial.print("Samples/Symbol: ");
  Serial.println(timingdelay[0]);
  Serial.println();
  logs.println();
  logs.print("Samples/Symbol: ");
  logs.println(timingdelay[0]);
  logs.println("<br>");

  int smoothcount=0;
  for (int i=1; i<samplecount; i++){
    float r = (float)sample[i]/timingdelay[0];
    int calculate = r;
    r = r-calculate;
    r*=10;
    if (r>=5){calculate+=1;}
    if (calculate>0){
      samplesmooth[smoothcount] = calculate*timingdelay[0];
      smoothcount++;
    }
  }
  Serial.println("Rawdata corrected:");
  Serial.print("Count=");
  Serial.println(smoothcount+1);
  logs.print("Count=");
  logs.println(smoothcount+1);
  logs.println("<br>");
  logs.println("Rawdata corrected:");
  for (int i=0; i<smoothcount; i++){
    Serial.print(samplesmooth[i]);
    Serial.print(",");
    logs.print(samplesmooth[i]);
    logs.print(",");
  }
  Serial.println();
  Serial.println();
  logs.println("<br>");
  return;
}

void receiver() {
  const long time = micros();
  const unsigned int duration = time - lastTime;

  if (duration > 100000){
    samplecount = 0;
  }

  if (duration >= 100){
    sample[samplecount++] = duration;
  }

  if (samplecount>=samplesize){
    detachInterrupt(RXPin);
    checkReceived();
  }
  lastTime = time;
}
