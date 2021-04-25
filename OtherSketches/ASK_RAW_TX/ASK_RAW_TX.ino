#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <RCSwitch.h>

int Gdo0 = 2;
int counter = 500; //Enter Raw Data Counter
long transmit[500] = {273,698,986,330,}; // Enter Raw Data to transmit

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(Gdo0,OUTPUT);
  ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, csn1);
  ELECHOUSE_cc1101.setMHZ(433.92);
  ELECHOUSE_cc1101.Init();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(3000);
  ELECHOUSE_cc1101.SetTx();

  for (int i = 0; i<counter; i+=2){
    digitalWrite(Gdo0,HIGH);
    delayMicroseconds(transmit[i]);
    digitalWrite(Gdo0,LOW);
    delayMicroseconds(transmit[i+1]);
  }
  ELECHOUSE_cc1101.SetRx();
}
