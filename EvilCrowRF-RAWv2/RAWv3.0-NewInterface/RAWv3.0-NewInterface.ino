#include "ELECHOUSE_CC1101_SRC_DRV.h"
#include <WiFiClient.h> 
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <SPIFFSEditor.h>
#include <EEPROM.h>
#include "SPIFFS.h"
#include "SPI.h"
#include <WiFiAP.h>
#include "rxconfig.h"
#include "txconfig.h"
#include "txbinary.h"
#include "javascript.h"
#include "style.h"
#include "btn3.h"
#include "jammer.h"
#include "index.h"
#include "wificonfig.h"
#include "txtesla.h"

#define eepromsize 4096
#define samplesize 2000

#if defined(ESP8266)
    #define RECEIVE_ATTR ICACHE_RAM_ATTR
#elif defined(ESP32)
    #define RECEIVE_ATTR IRAM_ATTR
#else
    #define RECEIVE_ATTR
#endif

// Config SSID, password and channel
const char* ssid = "RAW Replay v2";  // Enter your SSID here
const char* password = "123456789";  //Enter your Password here
const int wifi_channel = 12; //Enter your preferred Wi-Fi Channel

// HTML and CSS style
//const String MENU = "<body><p>Evil Crow RF v1.0</p><div id=\"header\"><body><nav id='menu'><input type='checkbox' id='responsive-menu' onclick='updatemenu()'><label></label><ul><li><a href='/'>Home</a></li><li><a class='dropdown-arrow'>Config</a><ul class='sub-menus'><li><a href='/txconfig'>RAW TX Config</a></li><li><a href='/txbinary'>Binary TX Config</a></li><li><a href='/rxconfig'>RAW RX Config</a></li><li><a href='/btnconfig'>Button TX Config</a></li></ul></li><li><a class='dropdown-arrow'>RX Log</a><ul class='sub-menus'><li><a href='/viewlog'>RX Logs</a></li><li><a href='/delete'>Delete Logs</a></li><li><a href='/downloadlog'>Download Logs</a></li><li><a href='/cleanspiffs'>Clean SPIFFS</a></li></ul></li><li><a href='/jammer'>Simple Jammer</a></li></ul></nav><br></div>";
const String HTML_CSS_STYLING = "<html><head><meta charset=\"utf-8\"><title>Evil Crow RF</title><link rel=\"stylesheet\" href=\"style.css\"><script src=\"lib.js\"></script></head>";

//Pushbutton Pins
int push1 = 34;
int push2 = 35;
int push3 = 32;

int led = 15;
static unsigned long Blinktime = 0;

int error_toleranz = 200;

int RXPin = 26;
int RXPin0 = 4;
int TXPin0 = 2;
int Gdo0 = 25;
const int minsample = 30;
unsigned long sample[samplesize];
unsigned long samplesmooth[samplesize];
int samplecount;
static unsigned long lastTime = 0;
String transmit = "";
long data_to_send[2000];
long data_button1[2000];
long data_button2[2000];
long data_button3[2000];
long transmit_push[2000];
String tmp_module;
String tmp_frequency;
String tmp_codelen;
String tmp_setrxbw;
String tmp_mod;
int mod;
String tmp_deviation;
float deviation;
String tmp_datarate;
String tmp_powerjammer;
int power_jammer;
int datarate;
float frequency;
float setrxbw;
String raw_rx = "0";
String jammer_tx = "0";
const bool formatOnFail = true;
String webString;
String bindata;
int samplepulse;
String tmp_samplepulse;
String tmp_transmissions;
int counter=0;
int pos = 0;
int transmissions;
int pushbutton1 = 0;
int pushbutton2 = 0;
int pushbutton3 = 0;
byte jammer[11] = {0xff,0xff,};

//BTN Sending Config
int btn_set_int;
String btn_set;
String btn1_frequency;
String btn1_mod;
String btn1_rawdata;
String btn1_deviation;
String btn1_transmission;
String btn2_frequency;
String btn2_mod;
String btn2_rawdata;
String btn2_deviation;
String btn2_transmission;
String btn3_frequency;
String btn3_mod;
String btn3_rawdata;
String btn3_deviation;
String btn3_transmission;
float tmp_btn1_deviation;
float tmp_btn2_deviation;
float tmp_btn3_deviation;
float tmp_btn1_frequency;
float tmp_btn2_frequency;
float tmp_btn3_frequency;
int tmp_btn1_mod;
int tmp_btn2_mod;
int tmp_btn3_mod;
int tmp_btn1_transmission;
int tmp_btn2_transmission;
int tmp_btn3_transmission;

// Wi-Fi config storage
int storage_status;
String tmp_config1;
String tmp_config2;
String config_wifi;
String ssid_new;
String password_new;
String tmp_channel_new;
String tmp_mode_new;
int channel_new;
int mode_new;

// Tesla
const uint16_t pulseWidth = 400;
const uint16_t messageDistance = 23;
const uint8_t transmtesla = 5;
const uint8_t messageLength = 43;

const uint8_t sequence[messageLength] = { 
  0x02,0xAA,0xAA,0xAA,  // Preamble of 26 bits by repeating 1010
  0x2B,                 // Sync byte
  0x2C,0xCB,0x33,0x33,0x2D,0x34,0xB5,0x2B,0x4D,0x32,0xAD,0x2C,0x56,0x59,0x96,0x66,
  0x66,0x5A,0x69,0x6A,0x56,0x9A,0x65,0x5A,0x58,0xAC,0xB3,0x2C,0xCC,0xCC,0xB4,0xD2,
  0xD4,0xAD,0x34,0xCA,0xB4,0xA0};

// Jammer
int jammer_pin;

// File
File logs;

AsyncWebServer controlserver(80);

void readConfigWiFi(fs::FS &fs, String path){
  File file = fs.open(path);
  
  if(!file || file.isDirectory()){
    storage_status = 0;
    return;
  }
  
  while(file.available()){
    config_wifi = file.readString();
    int file_len = config_wifi.length()+1;
    int index_config = config_wifi.indexOf('\n');
    tmp_config1 = config_wifi.substring(0, index_config-1);
    
    tmp_config2 = config_wifi.substring(index_config+1, file_len-3);
    storage_status = 1;
  }
  file.close();
}

void writeConfigWiFi(fs::FS &fs, const char * path, String message){
    File file = fs.open(path, FILE_APPEND);
    
    if(!file){
      return;
    }
    
    if(file.println(message)){
    } else {
    }
    file.close();
}

void deleteFile(fs::FS &fs, const char * path){
  //Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    //Serial.println("File deleted");
  } else {
    //Serial.println("Delete failed");
  }
}

void power_management(){
  EEPROM.begin(eepromsize);

  pinMode(push2, INPUT);
  pinMode(led, OUTPUT);

  byte z = EEPROM.read(eepromsize-2);
  if (digitalRead(push2) != LOW){
    if (z == 1){
      go_deep_sleep();
    }
      }else{
    if (z == 0){
      EEPROM.write(eepromsize-2, 1);
      EEPROM.commit();
      go_deep_sleep();
    }else{
      EEPROM.write(eepromsize-2, 0);
      EEPROM.commit();
    }
  }
}

void go_deep_sleep(){
  Serial.println("Going to sleep now");
  ELECHOUSE_cc1101.setModul(0);
  ELECHOUSE_cc1101.goSleep();
  ELECHOUSE_cc1101.setModul(1);
  ELECHOUSE_cc1101.goSleep();
  led_blink(5,250);
  esp_deep_sleep_start();
}

void led_blink(int blinkrep,int blinktimer){
  for (int i = 0; i<blinkrep; i++){
    digitalWrite(led, HIGH);
    delay(blinktimer);
    digitalWrite(led, LOW);
    delay(blinktimer);
  }
}

void poweron_blink(){
  if (millis()-Blinktime > 10000){
    digitalWrite(led, LOW);
  }
  if (millis()-Blinktime > 10100){
    digitalWrite(led, HIGH);
    Blinktime = millis();
  }
}

void sendByte(uint8_t dataByte) {
  for (int8_t bit=7; bit>=0; bit--) { // MSB
    digitalWrite(TXPin0, (dataByte & (1 << bit)) != 0 ? HIGH : LOW);
    delayMicroseconds(pulseWidth);
  }
}

void sendSignals() {
  pinMode(TXPin0,OUTPUT);
  ELECHOUSE_cc1101.setModul(0);
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setModulation(2);
  ELECHOUSE_cc1101.setMHZ(frequency);
  ELECHOUSE_cc1101.setDeviation(0);
  ELECHOUSE_cc1101.SetTx();
  
  for (uint8_t t=0; t<transmtesla; t++) {
    for (uint8_t i=0; i<messageLength; i++) sendByte(sequence[i]);
      digitalWrite(TXPin0, LOW);
      delay(messageDistance);
    }
}

void force_reset() {
  esp_task_wdt_init(1,true);
  esp_task_wdt_add(NULL);
  while(true);
}

void TXConfigRAW() {
  raw_rx = "0";
  pinMode(25,OUTPUT);
  ELECHOUSE_cc1101.setModul(1);
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setModulation(mod);
  ELECHOUSE_cc1101.setMHZ(frequency);
  ELECHOUSE_cc1101.setDeviation(deviation);
  ELECHOUSE_cc1101.SetTx();

  for (int i = 0; i<1000; i+=2){
    digitalWrite(25,HIGH);
    delayMicroseconds(transmit_push[i]);
    digitalWrite(25,LOW);
    delayMicroseconds(transmit_push[i+1]);
    Serial.print(transmit_push[i]);
    Serial.print(",");
  }
  ELECHOUSE_cc1101.setSidle();
}

bool checkReceived(void){
  
  delay(1);
  if (samplecount >= minsample && micros()-lastTime >100000){
    detachInterrupt(RXPin0);
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
  logs.println('\n');
  logs.println('\n');
  logs.print("Count=");
  logs.println(samplecount);
  logs.println("<br>");
  logs.println('\n');
  
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
  logs.println('\n');
  logs.println('\n');
}

void RECEIVE_ATTR receiver() {
  const long time = micros();
  const unsigned int duration = time - lastTime;

  if (duration > 100000){
    samplecount = 0;
  }

  if (duration >= 100){
    sample[samplecount++] = duration;
  }

  if (samplecount>=samplesize){
    detachInterrupt(RXPin0);
    detachInterrupt(RXPin);
    checkReceived();
  }

  if (mod == 0 && tmp_module == "2") {
    if (samplecount == 1 and digitalRead(RXPin) != HIGH){
      samplecount = 0;
    }
  }
  
  lastTime = time;
}

void enableReceive(){
  pinMode(RXPin0,INPUT);
  RXPin0 = digitalPinToInterrupt(RXPin0);
  ELECHOUSE_cc1101.SetRx();
  samplecount = 0;
  attachInterrupt(RXPin0, receiver, CHANGE);
  pinMode(RXPin,INPUT);
  RXPin = digitalPinToInterrupt(RXPin);
  ELECHOUSE_cc1101.SetRx();
  samplecount = 0;
  attachInterrupt(RXPin, receiver, CHANGE);
}

void setup() {

  Serial.begin(38400);
  power_management();
  SPIFFS.begin(formatOnFail);
  
  readConfigWiFi(SPIFFS,"/configwifi.txt");
  ssid_new = tmp_config1;
  password_new = tmp_config2;
  readConfigWiFi(SPIFFS,"/configmode.txt");
  tmp_channel_new = tmp_config1;
  tmp_mode_new = tmp_config2;
  channel_new = tmp_channel_new.toInt();
  mode_new = tmp_mode_new.toInt();
  
  if (storage_status == 0) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password,wifi_channel,8);
  }

  if(storage_status == 1) {
    if(mode_new == 1) {
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssid_new.c_str(), password_new.c_str(),channel_new,8);
    }
    else if(mode_new == 2) {
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid_new.c_str(), password_new.c_str());
    } 
  }

  delay(2000);
  
  EEPROM.begin(4096);
  pinMode(push1, INPUT);
  pinMode(push2, INPUT);
  pinMode(push3, INPUT);

  logs = SPIFFS.open("/logs.txt", "a+");

  controlserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", Index);
  });

  controlserver.on("/rxconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", RXConfig);
  });

  controlserver.on("/txconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", TXConfig);
  });

  controlserver.on("/txbinary", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", TXBinary);
  });

  controlserver.on("/btnconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", ButtonConf);
  });

  controlserver.on("/jammer", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", Jammer);
  });

  controlserver.on("/wificonfig", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", WiFiConfig);
  });

  controlserver.on("/txtesla", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", TxTesla);
  });

  controlserver.on("/stopjammer", HTTP_POST, [](AsyncWebServerRequest *request){
    jammer_tx = "0";
    request->send(200, "text/html", HTML_CSS_STYLING + "<script>alert(\"Stop OK\")</script>");
    ELECHOUSE_cc1101.setSidle(); 
  });

  controlserver.on("/setjammer", HTTP_POST, [](AsyncWebServerRequest *request){
    raw_rx = "0";
    tmp_module = request->arg("module");
    tmp_frequency = request->arg("frequency");
    tmp_powerjammer = request->arg("powerjammer");

    if (request->hasArg("configmodule")) {
      frequency = tmp_frequency.toFloat();
      power_jammer = tmp_powerjammer.toInt();

      Serial.println("Start");

      if (tmp_module == "1") {
        pinMode(2,OUTPUT);
        ELECHOUSE_cc1101.setModul(0);
        ELECHOUSE_cc1101.Init();
        ELECHOUSE_cc1101.setMHZ(frequency);
        ELECHOUSE_cc1101.setPA(power_jammer);
        ELECHOUSE_cc1101.SetTx();
        Serial.println("Module 1");
      }
      
      if (tmp_module == "2") {
        pinMode(25,OUTPUT);
        ELECHOUSE_cc1101.setModul(1);
        ELECHOUSE_cc1101.Init();
        ELECHOUSE_cc1101.setMHZ(frequency);
        ELECHOUSE_cc1101.setPA(power_jammer);
        ELECHOUSE_cc1101.SetTx();
        Serial.println("Module 2");
      }
      jammer_tx = "1"; 
      request->send(200, "text/html", HTML_CSS_STYLING + "<script>alert(\"Jammer OK\")</script>");
    }  
  });

  controlserver.on("/settx", HTTP_POST, [](AsyncWebServerRequest *request){
    raw_rx = "0";
    tmp_module = request->arg("module");
    tmp_frequency = request->arg("frequency");
    transmit = request->arg("rawdata");
    tmp_deviation = request->arg("deviation");
    tmp_mod = request->arg("mod");
    tmp_transmissions = request->arg("transmissions");

    if (request->hasArg("configmodule")) {
      int counter=0;
      int pos = 0;
      frequency = tmp_frequency.toFloat();
      deviation = tmp_deviation.toFloat();
      mod = tmp_mod.toInt();
      transmissions = tmp_transmissions.toInt();

      for (int i = 0; i<transmit.length(); i++){
        if (transmit.substring(i, i+1) == ","){
          data_to_send[counter]=transmit.substring(pos, i).toInt();
          pos = i+1;
          counter++;
        }
      }

      if (tmp_module == "1") {
        pinMode(2,OUTPUT);
        ELECHOUSE_cc1101.setModul(0);
        ELECHOUSE_cc1101.Init();
        ELECHOUSE_cc1101.setModulation(mod);
        ELECHOUSE_cc1101.setMHZ(frequency);
        ELECHOUSE_cc1101.setDeviation(deviation);
        //delay(400);
        ELECHOUSE_cc1101.SetTx();

        for (int r = 0; r<transmissions; r++) {
          for (int i = 0; i<counter; i+=2){
            digitalWrite(2,HIGH);
            delayMicroseconds(data_to_send[i]);
            digitalWrite(2,LOW);
            delayMicroseconds(data_to_send[i+1]);
            Serial.print(data_to_send[i]);
            Serial.print(",");
          }
          delay(2000); //Set this for the delay between retransmissions
        }        
      }

      else if (tmp_module == "2") {
        pinMode(25,OUTPUT);
        ELECHOUSE_cc1101.setModul(1);
        ELECHOUSE_cc1101.Init();
        ELECHOUSE_cc1101.setModulation(mod);
        ELECHOUSE_cc1101.setMHZ(frequency);
        ELECHOUSE_cc1101.setDeviation(deviation);
        //delay(400);
        ELECHOUSE_cc1101.SetTx();

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
      }     
       Serial.println();
       request->send(200, "text/html", HTML_CSS_STYLING + "<script>alert(\"Signal has been transmitted\")</script>");
       ELECHOUSE_cc1101.setSidle();
    }
  });

  controlserver.on("/settxtesla", HTTP_POST, [](AsyncWebServerRequest *request){
    raw_rx = "0";
    tmp_frequency = request->arg("frequency");

    if (request->hasArg("configmodule")) {
      frequency = tmp_frequency.toFloat();
      sendSignals();  
      request->send(200, "text/html", HTML_CSS_STYLING + "<script>alert(\"Signal has been transmitted\")</script>");
      ELECHOUSE_cc1101.setSidle();
    }
  });

  controlserver.on("/settxbinary", HTTP_POST, [](AsyncWebServerRequest *request){
    raw_rx = "0";
    tmp_module = request->arg("module");
    tmp_frequency = request->arg("frequency");
    bindata = request->arg("binarydata");
    tmp_deviation = request->arg("deviation");
    tmp_mod = request->arg("mod");
    tmp_samplepulse = request->arg("samplepulse");
    tmp_transmissions = request->arg("transmissions");

    if (request->hasArg("configmodule")) {
      int counter=0;
      int pos = 0;
      frequency = tmp_frequency.toFloat();
      deviation = tmp_deviation.toFloat();
      mod = tmp_mod.toInt();
      samplepulse = tmp_samplepulse.toInt();
      transmissions = tmp_transmissions.toInt();

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

      if (tmp_module == "1") {
        pinMode(2,OUTPUT);
        ELECHOUSE_cc1101.setModul(0);
        ELECHOUSE_cc1101.Init();
        ELECHOUSE_cc1101.setModulation(mod);
        ELECHOUSE_cc1101.setMHZ(frequency);
        ELECHOUSE_cc1101.setDeviation(deviation);
        //delay(400);
        ELECHOUSE_cc1101.SetTx();

        for (int r = 0; r<transmissions; r++) {
          for (int i = 0; i<count_binconvert; i+=2){
            digitalWrite(2,HIGH);
            delayMicroseconds(data_to_send[i]);
            digitalWrite(2,LOW);
            delayMicroseconds(data_to_send[i+1]);
          }
          delay(2000); //Set this for the delay between retransmissions    
        }
      }

      else if (tmp_module == "2") {
        pinMode(25,OUTPUT);
        ELECHOUSE_cc1101.setModul(1);
        ELECHOUSE_cc1101.Init();
        ELECHOUSE_cc1101.setModulation(mod);
        ELECHOUSE_cc1101.setMHZ(frequency);
        ELECHOUSE_cc1101.setDeviation(deviation);
        //delay(400);
        ELECHOUSE_cc1101.SetTx();  

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
      request->send(200, "text/html", HTML_CSS_STYLING + "<script>alert(\"Signal has been transmitted\")</script>");
      ELECHOUSE_cc1101.setSidle();
    }
  });

  controlserver.on("/setrx", HTTP_POST, [](AsyncWebServerRequest *request){
    tmp_module = request->arg("module");
    Serial.print("Module: ");
    Serial.println(tmp_module);
    tmp_frequency = request->arg("frequency");
    tmp_setrxbw = request->arg("setrxbw");
    tmp_mod = request->arg("mod");
    tmp_deviation = request->arg("deviation");
    tmp_datarate = request->arg("datarate");
    if (request->hasArg("configmodule")) {
      frequency = tmp_frequency.toFloat();
      setrxbw = tmp_setrxbw.toFloat();
      mod = tmp_mod.toInt();
      Serial.print("Modulation: ");
      Serial.println(mod);
      deviation = tmp_deviation.toFloat();
      datarate = tmp_datarate.toInt();

      if (tmp_module == "1") {
        ELECHOUSE_cc1101.setModul(0);
        ELECHOUSE_cc1101.Init();
      }

      else if (tmp_module == "2") {
        ELECHOUSE_cc1101.setModul(1);
        ELECHOUSE_cc1101.Init();

        if(mod == 2) {
          ELECHOUSE_cc1101.setDcFilterOff(0);
        }

        if(mod == 0) {
          ELECHOUSE_cc1101.setDcFilterOff(1);
        }
      }

      ELECHOUSE_cc1101.setSyncMode(0);        // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
      ELECHOUSE_cc1101.setPktFormat(3);       // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX. 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins. 2 = Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX. 3 = Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.

      ELECHOUSE_cc1101.setModulation(mod);      // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
      ELECHOUSE_cc1101.setRxBW(setrxbw);
      ELECHOUSE_cc1101.setMHZ(frequency);
      ELECHOUSE_cc1101.setDeviation(deviation);   // Set the Frequency deviation in kHz. Value from 1.58 to 380.85. Default is 47.60 kHz.
      ELECHOUSE_cc1101.setDRate(datarate);           // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!

      enableReceive();
      raw_rx = "1";
      request->send(200, "text/html", HTML_CSS_STYLING + "<script>alert(\"RX Config OK\")</script>");
    }
  });

  controlserver.on("/setbtn", HTTP_POST, [](AsyncWebServerRequest *request){
    btn_set = request->arg("button");
    btn_set_int = btn_set.toInt();
    raw_rx = "0";
    
    if (btn_set_int == 1){
      btn1_rawdata = request->arg("rawdata");
      btn1_deviation = request->arg("deviation");
      btn1_frequency = request->arg("frequency");
      btn1_mod = request->arg("mod");
      btn1_transmission = request->arg("transmissions");
      counter=0;
      int pos = 0;
      for (int i = 0; i<btn1_rawdata.length(); i++){
        if (btn1_rawdata.substring(i, i+1) == ","){
          data_button1[counter]=btn1_rawdata.substring(pos, i).toInt();
          pos = i+1;
          counter++;
        }
      }
    }
    
    if (btn_set_int == 2){
      btn2_rawdata = request->arg("rawdata");
      btn2_deviation = request->arg("deviation");
      btn2_frequency = request->arg("frequency");
      btn2_mod = request->arg("mod");
      btn2_transmission = request->arg("transmissions");
      counter=0;
      int pos = 0;
      for (int i = 0; i<btn2_rawdata.length(); i++){
        if (btn2_rawdata.substring(i, i+1) == ","){
          data_button2[counter]=btn2_rawdata.substring(pos, i).toInt();
          pos = i+1;
          counter++;
        }
      }
    }
    request->send(200, "text/html", HTML_CSS_STYLING + "<script>alert(\"Button Config OK\")</script>");
  });

  controlserver.on("/viewlog", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/logs.txt", "text/html");
  });

  controlserver.on("/cleanspiffs", HTTP_GET, [](AsyncWebServerRequest *request){
    logs.close();
    logs = SPIFFS.open("/logs.txt", "w");
    SPIFFS.remove("/");
    logs.close();
    logs = SPIFFS.open("/", "w");
    SPIFFS.remove("/logs.txt");
    logs.close();
    request->send(200, "text/html", HTML_CSS_STYLING+ "<body onload=\"JavaScript:AutoRedirect()\">"
    "<br><h2>SPIFFS and logs cleared!<br>You will be redirected in 5 seconds.</h2></body>" );
    logs = SPIFFS.open("/logs.txt", "a+");
    logs.println("Viewlog:");
    logs.println("<br>");
  });

  controlserver.on("/downloadlog", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/logs.txt", String(), true);
  });

  controlserver.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request){
    logs.close();
    logs = SPIFFS.open("/logs.txt", "w");
    request->send(200, "text/html", HTML_CSS_STYLING+ "<body onload=\"JavaScript:AutoRedirect()\">"
    "<br><h2>File cleared!<br>You will be redirected in 5 seconds.</h2></body>" );
    webString="";
    logs.close();
    logs = SPIFFS.open("/logs.txt", "a+");
    logs.println("Viewlog:");
    logs.println("<br>");
  });

  controlserver.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/css", Style);
  });

  controlserver.on("/lib.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/javascript", JavaScript);
  });

  controlserver.on("/setwificonfig", HTTP_POST, [](AsyncWebServerRequest *request) {
    String ssid_value = request->arg("ssid");
    String password_value = request->arg("password");
    String channel_value = request->arg("channel");
    String mode_value = request->arg("mode");
    
    if (request->hasArg("configmodule")) {
      deleteFile(SPIFFS, "/configwifi.txt");
      deleteFile(SPIFFS, "/configmode.txt");
      writeConfigWiFi(SPIFFS, "/configwifi.txt", ssid_value);
      writeConfigWiFi(SPIFFS, "/configwifi.txt", password_value);
      writeConfigWiFi(SPIFFS, "/configmode.txt", channel_value);
      writeConfigWiFi(SPIFFS, "/configmode.txt", mode_value);
      WiFi.disconnect();
      force_reset();
    }
  });

  controlserver.on("/deletewificonfig", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasArg("configmodule")) {
      deleteFile(SPIFFS, "/configwifi.txt");
      deleteFile(SPIFFS, "/configmode.txt");
      force_reset();
    }
  });

  AsyncElegantOTA.begin(&controlserver);
  controlserver.begin();

  ELECHOUSE_cc1101.addSpiPin(18, 19, 23, 5, 0);
  ELECHOUSE_cc1101.addSpiPin(18, 19, 23, 27, 1);
  logs.println("Viewlog:");
  logs.println("<br>");
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
  
  int firstsample = signaltimings[0];
  
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

  if (firstsample == sample[1] and firstsample < timingdelay[0]){
    sample[1] = timingdelay[0];
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
  logs.println('\n');
  logs.println('\n');
  Serial.println();
  Serial.print("Samples/Symbol: ");
  Serial.println(timingdelay[0]);
  Serial.println();
  logs.println('\n');
  logs.print("Samples/Symbol: ");
  logs.println(timingdelay[0]);
  logs.println("<br>");
  logs.println('\n');

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
  logs.println('\n');
  logs.println("Rawdata corrected:");
  for (int i=0; i<smoothcount; i++){
    Serial.print(samplesmooth[i]);
    Serial.print(",");
    transmit_push[i] = samplesmooth[i];
    logs.print(samplesmooth[i]);
    logs.print(",");
  }
  Serial.println();
  Serial.println();
  logs.println("<br>");
  logs.println('\n');
  return;
}

void loop() {
  poweron_blink();
  pushbutton1 = digitalRead(push1);
  pushbutton2 = digitalRead(push2);
  pushbutton3 = digitalRead(push3);

  if(raw_rx == "1") {
    if(checkReceived()){
      printReceived();
      signalanalyse();
      enableReceive();
    }
  }

  if(jammer_tx == "1") {
    raw_rx = "0";
    
    if (tmp_module == "1") {
      for (int i = 0; i<12; i+=2){
        digitalWrite(2,HIGH);
        delayMicroseconds(jammer[i]);
        digitalWrite(2,LOW);
        delayMicroseconds(jammer[i+1]);
      }
    }
    else if (tmp_module == "2") {
      for (int i = 0; i<12; i+=2){
        digitalWrite(25,HIGH);
        delayMicroseconds(jammer[i]);
        digitalWrite(25,LOW);
        delayMicroseconds(jammer[i+1]);
      }
    }
  }

  if (pushbutton1 == LOW) {
    raw_rx = "0";
    tmp_btn1_deviation = btn1_deviation.toFloat();
    tmp_btn1_mod = btn1_mod.toInt();
    tmp_btn1_frequency = btn1_frequency.toFloat();
    tmp_btn1_transmission = btn1_transmission.toInt();
    pinMode(25,OUTPUT);
    ELECHOUSE_cc1101.setModul(1);
    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setModulation(tmp_btn1_mod);
    ELECHOUSE_cc1101.setMHZ(tmp_btn1_frequency);
    ELECHOUSE_cc1101.setDeviation(tmp_btn1_deviation);
    //delay(400);
    ELECHOUSE_cc1101.SetTx();

    for (int r = 0; r<tmp_btn1_transmission; r++) {
        for (int i = 0; i<counter; i+=2){
          digitalWrite(25,HIGH);
          delayMicroseconds(data_button1[i]);
          digitalWrite(25,LOW);
          delayMicroseconds(data_button1[i+1]);
          Serial.print(data_button1[i]);
          Serial.print(",");
        }
        delay(2000); //Set this for the delay between retransmissions
      }
     Serial.println();
     ELECHOUSE_cc1101.setSidle();
  }

  //delay(500);

  if (pushbutton2 == LOW) {
    raw_rx = "0";
    tmp_btn2_deviation = btn2_deviation.toFloat();
    tmp_btn2_mod = btn2_mod.toInt();
    tmp_btn2_frequency = btn2_frequency.toFloat();
    tmp_btn2_transmission = btn2_transmission.toInt();
    pinMode(25,OUTPUT);
    ELECHOUSE_cc1101.setModul(1);
    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setModulation(tmp_btn2_mod);
    ELECHOUSE_cc1101.setMHZ(tmp_btn2_frequency);
    ELECHOUSE_cc1101.setDeviation(tmp_btn2_deviation);
    //delay(400);
    ELECHOUSE_cc1101.SetTx();

    for (int r = 0; r<tmp_btn2_transmission; r++) {
        for (int i = 0; i<counter; i+=2){
          digitalWrite(25,HIGH);
          delayMicroseconds(data_button2[i]);
          digitalWrite(25,LOW);
          delayMicroseconds(data_button2[i+1]);
          Serial.print(data_button2[i]);
          Serial.print(",");
        }
        delay(2000); //Set this for the delay between retransmissions
      }
     Serial.println();
     ELECHOUSE_cc1101.setSidle();
  }

  //delay(500);

  if (pushbutton3 == LOW) {
    raw_rx = "0";
    TXConfigRAW();
  }
}
