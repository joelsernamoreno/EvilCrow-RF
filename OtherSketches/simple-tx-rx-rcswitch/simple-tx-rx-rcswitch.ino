#include "ELECHOUSE_CC1101_SRC_DRV.h"
#include "RCSwitch.h"

// TX
unsigned long code_tx = 1642498;
unsigned int codelen_tx = 24;
float freq_tx = 433.92;

// RX
unsigned long code;
unsigned int codelen;
unsigned int codeproto;
float freq_rx = 433.92;

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
  Serial.println("Configuration first CC1101 (RX)");
  
  ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, csn1);
  ELECHOUSE_cc1101.setMHZ(freq_rx);
  ELECHOUSE_cc1101.Init();
  CC1101_1.enableReceive(gdo2_1);
  ELECHOUSE_cc1101.SetRx();
  
  delay(500);
  Serial.println("Configuration second CC1101 (TX)");
  ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, csn2);
  ELECHOUSE_cc1101.setMHZ(freq_tx);
  ELECHOUSE_cc1101.Init();
  CC1101_2.enableTransmit(gdo0_2);
  ELECHOUSE_cc1101.SetTx();
  delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:
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

    mySwitch.resetAvailable();
  }
 
  CC1101_2.send(code_tx, codelen_tx);
}
