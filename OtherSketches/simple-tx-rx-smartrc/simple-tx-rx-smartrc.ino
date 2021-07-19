#include "ELECHOUSE_CC1101_SRC_DRV.h"
#include <EEPROM.h>
#include "SPIFFS.h"

#if defined(ESP8266)
    #define RECEIVE_ATTR ICACHE_RAM_ATTR
#elif defined(ESP32)
    #define RECEIVE_ATTR IRAM_ATTR
#else
    #define RECEIVE_ATTR
#endif

#define samplesize 1000
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
const bool formatOnFail = true;
String bindata;
long transmit_push[1000];

int counter=0;
int pos = 0;

// RX Config
int mod_rx = 2;
float setrxbw = 58;
float frequency_rx = 433.92;
float deviation_rx = 0;
float datarate = 5;
String raw_rx = "0";

// TX Config
int mod_tx = 2;
float frequency_tx = 433.92;
float deviation_tx = 0;
long data_tx[1000] = {414,1181,405,1174,381,1190,1192,391,1192,399,383,1189,386,1189,1192,386,392,1194,392,1195,391,1193,1192,390,392,1194,392,1194,391,1193,392,1194,392,1193,392,1204,382,1194,391,1193,391,1194,392,1193,1192,390,391,1193,393,12283,407,1167,393,1183,383,1178,1182,395,1197,394,392,1195,392,1196,1193,391,392,1194,392,1194,392,1194,1192,391,392,1193,391,1194,396,1189,391,1193,392,1194,391,1194,390,1194,392,1194,391,1193,392,1193,1193,391,391,1193,393,12278,401,1188,408,1182,383,1182,1191,376,1178,394,394,1195,392,1194,1193,392,392,1194,392,1194,392,1194,1192,391,393,1199,387,1194,404,1181,391,1193,392,1193,391,1194,391,1193,392,1193,391,1193,392,1193,1192,390,392,1194,393,12278,414,1177,381,1189,392,1177,1199,383,1198,394,394,1179,389,1190,1189,391,393,1195,394,1191,391,1193,1204,379,391,1195,389,1194,392,1193,392,1193,391,1193,392,1193,391,1193,392,1193,392,1193,392,1193,1192,390,391,1194,393,12278,401,1181,409,1180,386,1197,1179,385,1177,389,395,1197,393,1195,1193,403,380,1193,392,1192,391,1193,1192,391,392,1194,392,1194,392,1194,392,1193,392,1193,392,1193,392,1193,392,1193,391,1193,391,1193,1193,391,390,1194,395,12275,395,1170,395,1180,387,1188,1170,390,1199,393,387,1188,400,1191,1191,388,379,1185,397,1190,392,1194,1190,391,392,1194,392,1194,391,1193,391,1193,391,1193,391,1194,391,1193,392,1194,392,1193,392,1193,1193,392,388,1193,394,12276,413,1154,392,1171,399,1178,1195,394,1179,389,389,1192,392,1194,1193,391,391,1193,391,1194,392,1194,1192,391,392,1194,392,1194,392,1194,392,1193,391,1193,391,1193,391,1193,392,1190,391,1193,392,1193,1192,392,389,1194,393,12281,405,1166,397,1186,399,1174,1196,393,1194,392,394,1194,392,1194,1193,391,392,1194,392,1194,392,1193,1192,392,391,1194,392,1194,392,1193,391,1193,392,1194,392,1194,388,1194,393,1191,392,1193,391,1193,1192,390,392,1194,393,12278,414,1162,388,1186,390,1177,1198,383,1198,394,394,1177,389,1190,1190,392,393,1194,392,1194,392,1194,1193,391,391,1194,389,1193,392,1190,392,1193,391,1193,392,1194,388,1194,391,1193,392,1193,392,1193,1192,391,391,1194,393,12281,411,1168,402,1178,395,1177,1176,393,1194,393,393,1194,392,1194,1192,391,391,1194,392,1193,392,1194,1193,389,392,1195,389,1193,392,1191,391,1194,392,1193,391,1193,391,1193,392,1194,392,1193,391,1193,1192,391,392,1194,394,};

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
  
  for (int i = 1; i<samplecount; i++){
    Serial.print(sample[i]);
    Serial.print(",");  
  }
  Serial.println();
  Serial.println();
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
  // put your setup code here, to run once:
  Serial.begin(38400);
  EEPROM.begin(4096);
  SPIFFS.begin(formatOnFail);

  ELECHOUSE_cc1101.addSpiPin(18, 19, 23, 5, 0);
  ELECHOUSE_cc1101.addSpiPin(18, 19, 23, 27, 1);

  // Config RX
  ELECHOUSE_cc1101.setModul(0);
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setSyncMode(0);        // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
  ELECHOUSE_cc1101.setPktFormat(3);       // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX. 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins. 2 = Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX. 3 = Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.

  ELECHOUSE_cc1101.setModulation(mod_rx);      // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
  ELECHOUSE_cc1101.setRxBW(setrxbw);
  ELECHOUSE_cc1101.setMHZ(frequency_rx);
  ELECHOUSE_cc1101.setDeviation(deviation_rx);   // Set the Frequency deviation in kHz. Value from 1.58 to 380.85. Default is 47.60 kHz.
  ELECHOUSE_cc1101.setDRate(datarate);           // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!

  enableReceive();
  raw_rx = "1";

  // Config TX
  pinMode(25,OUTPUT);
  ELECHOUSE_cc1101.setModul(1);
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setModulation(mod_tx);
  ELECHOUSE_cc1101.setMHZ(frequency_tx);
  ELECHOUSE_cc1101.setDeviation(deviation_tx);
  ELECHOUSE_cc1101.SetTx();
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
      }else{
        for (int b=0; b<calculate; b++){
          Serial.print(lastbin);
        }
      }
    }
  }
  Serial.println();
  Serial.print("Samples/Symbol: ");
  Serial.println(timingdelay[0]);
  Serial.println();

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
  for (int i=0; i<smoothcount; i++){
    Serial.print(samplesmooth[i]);
    Serial.print(",");
    transmit_push[i] = samplesmooth[i];
  }
  Serial.println();
  Serial.println();
  return;
}

void loop() {
  // put your main code here, to run repeatedly:
  pinMode(25,OUTPUT);
  ELECHOUSE_cc1101.setModul(1);
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setModulation(mod_tx);
  ELECHOUSE_cc1101.setMHZ(frequency_tx);
  ELECHOUSE_cc1101.setDeviation(deviation_tx);
  ELECHOUSE_cc1101.SetTx();
  
  for (int i = 0; i<1000; i+=2){
      digitalWrite(25,HIGH);
      delayMicroseconds(data_tx[i]);
      digitalWrite(25,LOW);
      delayMicroseconds(data_tx[i+1]);
      raw_rx = "1";
    }
    
  if(raw_rx == "1") {
    if(checkReceived()){
      printReceived();
      signalanalyse();
      enableReceive();
      raw_rx = "0";
    }
  }
}
