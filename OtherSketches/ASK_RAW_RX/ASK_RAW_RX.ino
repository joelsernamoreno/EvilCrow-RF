#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <RCSwitch.h>
#define SAMPLESIZE 500

int RxPin = 4; //receive pin
float freq = 433.92;
float start_freq = 433;
float stop_freq = 434;
int rssi_threshold = -70;
int count_threshold = 50;
int time_threshold = 70;

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

static int interruptPin = 0;
int state = 0;
long compare_freq;
float mark_freq;
int rssi;
int mark_rssi=-100;
int counter = 0;
static unsigned long lastTime = 0;
static unsigned long resetTime;
bool Receive = 0;
int MODE = 2;
String Print;
static unsigned int timings[SAMPLESIZE];

RCSwitch mySwitch = RCSwitch();
RCSwitch CC1101_1 = RCSwitch();
RCSwitch CC1101_2 = RCSwitch();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(RxPin,INPUT);
  interruptPin = digitalPinToInterrupt(RxPin);
  ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, csn1);
  ELECHOUSE_cc1101.setRxBW(58);
  ELECHOUSE_cc1101.setMHZ(freq);
  ELECHOUSE_cc1101.Init();
  CC1101_1.enableReceive(gdo2_1);
  ELECHOUSE_cc1101.SetRx();
  Serial.println("Raw data Receiver Ready!");
  Serial.print("Freq: ");
  Serial.println(freq);
}

void loop() {
  // put your main code here, to run repeatedly:
  commands();
  print_received();
  scan_frquency();
}

void commands(){
  
  if (Serial.available()){
    String c = Serial.readString();
    if (c=="scan\n"){
      MODE = 1;
    }
  }
}

void scan_frquency(){
  
  if (MODE == 1){
    serial_print("---------------------\nFrequency scan Mode\n---------------------");
    set_state(2);
    ELECHOUSE_cc1101.setMHZ(freq);
    rssi = ELECHOUSE_cc1101.getRssi();

  if (rssi>rssi_threshold){
  if (rssi > mark_rssi){
    mark_rssi = rssi;  
    mark_freq = freq;
  }
}
  freq+=0.01;

  if (freq>stop_freq){
    freq=start_freq;

  if (mark_rssi>rssi_threshold){
    long fr = mark_freq*100;
  if (fr == compare_freq){
    Serial.print("Freq: ");
    Serial.println(mark_freq);
    Serial.print("Rssi: ");
    Serial.println(mark_rssi);
    ELECHOUSE_cc1101.setMHZ(mark_freq);
    mark_rssi=-100;
    compare_freq = 0;
    mark_freq = 0;
    MODE = 2;
    delay(1000);
    }else{
    compare_freq = mark_freq*100;
    freq = mark_freq -0.10;
    mark_freq=0;
    mark_rssi=-100;
    }
  }
}
}
}

void print_received(){

  if (MODE == 2){
    serial_print("---------------------\nReceive Mode\n---------------------");
    set_state(1);
  if (Receive == 1){
    Serial.print("Counter: ");
    Serial.println(counter);
  for (unsigned int i = 1; i< counter; i++) {
    Serial.print( timings[i] );
    Serial.print(",");
  }
    Serial.println();
    delay(500);
    Receive = 0;
    counter = 0;
    set_state(1);
    }
  }
}

void serial_print(String s){
  if (s != Print){
    Print = s;
    Serial.println(s);
  }
}

void set_state(int s){
  if (s == 1 && s != state){
    state = s;
    attachInterrupt(interruptPin, handleInterrupt, CHANGE);
  }
  if (s == 2 && s != state){
    state = s;
    detachInterrupt(interruptPin);
  }
}

void handleInterrupt() {
  static unsigned long lastTime = 0;
  const long time = micros();
  const unsigned int duration = time - lastTime;

  if (millis()-resetTime > time_threshold){
    if (counter > count_threshold){
      Receive = 1;
      set_state(2);
      }else{
      counter=0;
      Receive = 0; 
    }
  }

  if (duration > 100){
    timings[counter] = duration;
    counter++;
  }

  if (counter >= SAMPLESIZE){
    Receive = 1;
    set_state(2);
  }

  lastTime = time;
  resetTime = millis();
}
