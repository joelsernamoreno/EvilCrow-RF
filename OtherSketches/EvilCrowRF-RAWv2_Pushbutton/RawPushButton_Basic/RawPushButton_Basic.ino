#include "ELECHOUSE_CC1101_SRC_DRV.h"
#define samplesize 1000

// Config RX
float freq = 433.92; // Set frequency
int mod = 2; // Set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
float setrxbw = 812;
float deviation = 0;  // Set the Frequency deviation in kHz. Value from 1.58 to 380.85. Default is 47.60 kHz.
int datarate = 5; // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!

int error_toleranz = 200;

// Rx and Tx pins
int RXPin = 26;
int Gdo0 = 25;

//Pushbutton Pins
int push1 = 34;
int push2 = 35;
int push3 = 32;
int pushbutton1 = 0;
int pushbutton2 = 0;
int pushbutton3 = 0;

const int minsample = 30;
unsigned long sample[samplesize];
unsigned long samplesmooth[samplesize];
int samplecount;
static unsigned long lastTime = 0;
String raw_rx = "0";

// Transmit
unsigned long transmit[samplesize];
String bindata = "";
long data_to_send[1000];

void TXConfigRAW() {
  int counter=0;
  int pos = 0;
  pinMode(25,OUTPUT);
  ELECHOUSE_cc1101.setSpiPin(18, 19, 23, 27);
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setModulation(mod);
  ELECHOUSE_cc1101.setMHZ(freq);
  ELECHOUSE_cc1101.setDeviation(deviation);
  delay(400);
  ELECHOUSE_cc1101.SetTx();

  for (int i = 0; i<1000; i+=2){
    digitalWrite(25,HIGH);
    delayMicroseconds(data_to_send[i]);
    digitalWrite(25,LOW);
    delayMicroseconds(data_to_send[i+1]);
    Serial.print(data_to_send[i]);
    Serial.print(",");
  }
}

void RXConfig() {
  ELECHOUSE_cc1101.setSpiPin(18, 19, 23, 27);

  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setSyncMode(0);        // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
  ELECHOUSE_cc1101.setPktFormat(3);       // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX. 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins. 2 = Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX. 3 = Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.

  ELECHOUSE_cc1101.setModulation(mod);      // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
  ELECHOUSE_cc1101.setRxBW(setrxbw);
  ELECHOUSE_cc1101.setMHZ(freq);
  ELECHOUSE_cc1101.setDeviation(deviation);   // Set the Frequency deviation in kHz. Value from 1.58 to 380.85. Default is 47.60 kHz.
  ELECHOUSE_cc1101.setDRate(datarate);           // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!
  raw_rx = "1";
  enableReceive();
}

void setup() {
  Serial.begin(38400);
  pinMode(push1, INPUT);
  pinMode(push2, INPUT);
  pinMode(push3, INPUT);
}

void loop() {
  
  pushbutton1 = digitalRead(push1);
  pushbutton2 = digitalRead(push2);
  pushbutton3 = digitalRead(push3);
  
  if(raw_rx == "1") {
    if(checkReceived()) {
      printReceived();
      signalanalyse();
      enableReceive();
    } 
  }
  if (pushbutton1 == LOW) {
    RXConfig();
  }

  if (pushbutton2 == LOW) {
    TXConfigRAW();
  }

  if (pushbutton3 == LOW) {
    Serial.println("3");
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
  for (int i = 1; i<samplecount; i++){
    Serial.print(sample[i]);
    Serial.print(",");  
  }
  Serial.println();
  Serial.println();
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
    data_to_send[i] = samplesmooth[i];
  }
  
  Serial.println();
  Serial.println();
  for (int i=0; i<smoothcount; i++){
    Serial.print(data_to_send[i]);
    //Serial.print(",");
  }
  Serial.println();
  Serial.println();
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
