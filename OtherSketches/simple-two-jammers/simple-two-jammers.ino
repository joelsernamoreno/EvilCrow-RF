#include "ELECHOUSE_CC1101_SRC_DRV.h"
#include "RCSwitch.h"

float freq1 = 433.92;
float freq2 = 315.00;
int power1 = 10;
int power2 = 10;

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

RCSwitch mySwitch = RCSwitch();
RCSwitch CC1101_1 = RCSwitch();
RCSwitch CC1101_2 = RCSwitch();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Configuration first CC1101");
  
  ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, csn1);
  ELECHOUSE_cc1101.setMHZ(freq1);
  ELECHOUSE_cc1101.setPA(power1);
  ELECHOUSE_cc1101.Init();
  CC1101_1.enableTransmit(gdo0_1);
  ELECHOUSE_cc1101.SetTx();
  
  delay(500);
  Serial.println("Configuration second CC1101");
  ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, csn2);
  ELECHOUSE_cc1101.setMHZ(freq2);
  ELECHOUSE_cc1101.setPA(power2);
  ELECHOUSE_cc1101.Init();
  CC1101_2.enableTransmit(gdo0_2);
  ELECHOUSE_cc1101.SetTx();
  delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:
  CC1101_1.send("ffffffff");
  CC1101_2.send("ffffffff");
}
