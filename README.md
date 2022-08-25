# EvilCrow-RF

![EvilCrow](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/Logo1.png)

**Idea, development and implementation:** Joel Serna (@JoelSernaMoreno).

**PCB design:** Ignacio Díaz Álvarez (@Nacon_96) and Forensic Security (@ForensicSec).

**Manufacturer and distributor:** April Brother (@aprbrother).

**Distributor from United Kingdom:** KSEC Worldwide (@KSEC_KC).

**Collaborators:** Little Satan, Ernesto Sánchez (@ernesto_xload), Federico Maggi (@phretor), Andrea Guglielmini (@Guglio95) and RFQuack (@rfquack).

The developers and collaborators of this project do not earn money with this. 
You can invite me for a coffee to further develop Low-Cost hacking devices. If you don't invite me for a coffee, nothing happens, I will continue developing devices.

[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/E1E614OA5)

**For sale with April Brother (shipping from China):** 

* [Aliexpress Shop](https://aliexpress.com/item/1005002476481892.html)

**For sale with KSEC Worldwide (shipping from United Kingdom):**

* Evil Crow RF: https://labs.ksec.co.uk/product/evil-crow-rf/

**Discord Group:** https://discord.gg/jECPUtdrnW

**Summary:**

1. Disclaimer
2. Introduction
3. Firmware	
	* Installation
	* First steps with Evil Crow RF
	* RX Config Example
	* RX Log Example
	* RAW TX Config Example
	* Binary TX Config Example
	* Pushbuttons Configuration
	* Tesla Charge Door Opener
	* OTA Update
	* Wi-Fi Config
	* Power management
	* Other Sketches
	* Public Demo
4. Advanced Firmware with RFQuack
	* Installation and first steps
	* RX Example
	* TX Example
	* Public Demo
5. Evil Crow RF Support

# Disclaimer

Evil Crow RF is a basic device for professionals and cybersecurity enthusiasts.

We are not responsible for the incorrect use of Evil Crow RF.

We recommend using this device for testing, learning and fun :D

**Be careful with this device and the transmission of signals. Make sure to follow the laws that apply to your country.**

![EvilCrowRF](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/evilcrowrf.png)

# Introduction

Evil Crow RF is a radiofrequency hacking device for pentest and Red Team operations, this device operates in the following radiofrequency bands:

* 300Mhz-348Mhz
* 387Mhz-464Mhz
* 779Mhz-928Mhz

Evil Crow RF has two CC1101 radiofrequency modules, these modules can be configured to transmit or receive on different frequencies at the same time.

Evil Crow RF allows the following attacks:

* Signal receiver
* Signal transmitter
* Replay attack
* Brute Force
* ...

**NOTE:** 

* All devices have been flashed with basic firmware EvilCrow-RF before shipping.
* Please do not ask me to implement new functions in this code. You can develop code for Evil Crow RF and send PR with your new code.

# Firmware

The basic firmware allows to receive and transmit signals. You can configure the two radio modules through a web panel via WiFi.

## Installation

1. Install esptool: sudo apt install esptool
2. Install pyserial: sudo pip install pyserial
3. Download and Install the Arduino IDE: https://www.arduino.cc/en/main/software
4. Download EvilCrow-RF repository: git clone https://github.com/joelsernamoreno/EvilCrow-RF.git
5. Download and Install the Arduino IDE: https://www.arduino.cc/en/main/software
6. Download the ESPAsyncWebServer library in the Arduino library directory: git clone https://github.com/me-no-dev/ESPAsyncWebServer.git
7. Download the AsyncElegantOTA library in the Arduino library directory: git clone https://github.com/ayushsharma82/AsyncElegantOTA.git
8. Download the AsyncTCP library in the Arduino library directory: git clone https://github.com/me-no-dev/AsyncTCP.git
9. Edit AsyncTCP/src/AsyncTCP.h and change the following:

* #define CONFIG_ASYNC_TCP_USE_WDT 1 to #define CONFIG_ASYNC_TCP_USE_WDT 0

10. Open Arduino IDE
11. Go to File - Preferences. Locate the field "Additional Board Manager URLs:" Add "https://dl.espressif.com/dl/package_esp32_index.json" without quotes. Click "Ok"
12. Select Tools - Board - Boards Manager. Search for "esp32". Install "esp32 by Espressif system version 1.0.4". Click "Close".
13. Open the EvilCrow-RF/EvilCrowRF-RAWv2/RAWv3.0-NewInterface/RAWv3.0-NewInterface.ino sketch
14. Select Tools:
    * Board - "ESP32 Dev Module".
    * Flash Size - "4MB (32Mb)".
    * CPU Frequency - "240MHz (WiFi/BT)".
    * Flash Frequency - "80MHz"
    * Flash Mode - "DIO"
15. Upload the code to the EvilCrow-RF device.
16. Press reset button

## First steps with EvilCrow-RF

1. Visualize the wifi networks around you and connect to the EvilCrow-RF (default SSID: RAW Replay v3).
2. Enter the password for the wifi network (default password: 123456789).
3. Open a browser and access the web panel (default IP: 192.168.4.1).
4. Go!

![Index](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/index.png)

## RX Config Example

* Module: (1 for first CC1101 module, 2 for second CC1101 module)
* Modulation: (example ASK/OOK)
* Frequency: (example 433.92)
* RxBW bandwidth: (example 58)
* Deviation: (example 0)
* Data rate: (example 5)

![RXConfig](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/rx-config.png)

**2-FSK NOTES:**

* Evil Crow RF allows 2-FSK (RX/TX) modulation, this is configured for use with CC1101 module 2. Do not use CC1101 module 1 for 2-FSK RX. 

* You can use 2-FSK TX with module 1 or with module 2. 

* Evil Crow RF allows you to receive signals at the same time on two different frequencies, but this does not work correctly if you use 2-FSK. Make sure you use module 2 for 2-FSK RX, while doing this do not use module 1 for anything or you will not receive the 2-FSK signals correctly. 

* You can receive two signals on different frequencies with ASK/OOK.

## RX Log Example

![RXLog](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/rx-log.png)

## RAW TX Config Example

* Module: (1 for first CC1101 module, 2 for second CC1101 module)
* Modulation: (example ASK/OOK)
* Transmissions: (number transmissions)
* Frequency: (example 433.92)
* RAW Data: (raw data or raw data corrected displayed in RX Log)
* Deviation: (example 0)

![TXConfig](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/tx-config.png)

## Binary TX Config Example

* Module: (1 for first CC1101 module, 2 for second CC1101 module)
* Modulation: (example ASK/OOK)
* Transmissions: (number transmissions)
* Frequency: (example 433.92)
* Binary Data: (binary data displayed in RX Log)
* Sample Pulse: (samples/symbol displayed in RX Log)
* Deviation: (example 0)

![TXBinary](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/tx-binary.png)


## Pushbuttons Configuration

![PB](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/pb.png)

* Button: (1 for first pushbutton, 2 for second pushbutton)
* Modulation: (example ASK/OOK)
* Transmissions: (number transmissions)
* Frequency: (example 433.92)
* RAW Data: (raw data or raw data corrected displayed in RX Log)
* Deviation: (example 0)

![Pushbutton](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/pushbutton.png)

## Tesla Charge Door Opener

Demo: https://www.youtube.com/watch?v=feNokjfEGgs

## OTA Update

Demo: https://www.youtube.com/watch?v=YQFNLyHu42A

## WiFi Config

Evil Crow RF is configured in AP mode with a default SSID and password. You can change the mode to STATION or AP, change SSID, change password and change Wi-Fi channel remotely from the web panel.

The changes will be stored in the device, every time you restart Evil Crow RF the new Wi-Fi settings will be applied. If you want to return to the default settings, you can delete the stored Wi-Fi configuration from the web panel.

**NOTE:** When changing the Wi-Fi configuration you have to fill in all the fields correctly, if you do not do this you bricked the device.

## Power Management

1. In normal mode, press push2 + reset, then release reset: Evil Crow RF blinks several times and goes to sleep. 
2. In sleep mode, press push2 + reset, then release reset to wake him up.

Demo: https://www.youtube.com/shorts/K_Qkss6-pEY

**NOTE:** If Evil Crow RF is sleeping and you accidentally press reset, he'll go straight back to sleep. If he isn't asleep and you press reset then he will stay awake too.

## Other Sketches

You can program Evil Crow RF with other configurations without using the basic firmware. In the OtherSketches folder you can find two examples: 

* ASK_RAW_RX.ino: Receive raw data (Only ASK)
* ASK_RAW_TX.ino: Transmit raw data (Only ASK)
* EvilCrowRF-RAWv2_Pushbutton: Two new sketches for using Evil Crow RF with push buttons: RawPushButton_Basic and
RawPushButton_Web. Read the document EvilCrowRF-RAWv2 Pushbutton.pdf for more information.

Read the code to know how to configure this for your environment.

Additionally, you can develop other sketches for Evil Crow RF and PR to this repository :)

## Public Demo:

* Replay attack with pushbuttons: https://twitter.com/JoelSernaMoreno/status/1343573202967126022
* Simple Brute Force: https://twitter.com/JoelSernaMoreno/status/1344798890516770817
* Hacking a X-RAY Machine with WHIDelite & EvilCrowRF: https://lucabongiorni.medium.com/hacking-a-x-ray-machine-with-whidelite-evilcrowrf-74b871f8e23b

# Advanced Firmware with RFQuack

RFQuack is the only versatile RF-analysis tool that quacks!

It's a library firmware that allows you to sniff, manipulate, and transmit data over the air. Consider it as the hardware-agnostic and developer-friendly version of the great YardStick One, which is based on the CC1101 radio chip. Similarly to RFCat, RFQuack has a console-based, Python-scriptable client that allows you to set parameters, receive, transmit, and so on.

RFQuack is compatible with Evil Crow RF :D

## Installation and first steps

1. Download the RFQuack repository: git clone https://github.com/rfquack/RFQuack.git

2. Go to RFQuack directory: cd RFQuack

3. Edit build.env with this content:

![Content](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/config-rfquack.png)

4. Execute the following command: make docker-build-nc && make build

5. Connect Evil Crow RF to your computer and flash RFQuack with the following command: PORT=/dev/ttyUSB0 make flash

6. Disconnect and connect Evil Crow RF again

7. Download the RFQuack-cli repository: git clone https://github.com/rfquack/RFQuack-cli.git

8. Go to RFQuack-cli directory: cd RFQuack-cli

9. Execute the following command: make docker-build

10. Execute the following command: docker run --device /dev/ttyUSB0 --rm -it rfquack/cli:latest tty -P /dev/ttyUSB0

![Cli](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/cli-rfquack.png)

## RX Example

In the RFQuack console run the following commands:

1. q.radioA.set_modem_config(modulation="OOK", carrierFreq=433.920, syncWords=b"", useCRC=False, bitRate=1.7*2, rxBandwidth=58)
2. q.radioA.set_packet_len(isFixedPacketLen=True, packetLen=100)
3. q.radioA.rx()

This is a simple example, read the documentation for information: https://github.com/rfquack/RFQuack

## TX Example

This example performs a replay attack with the signal received in the RX example.
In the RFQuack console run the following commands:

1. len(q.data)
2. q.radioA.tx()
3. q.radioA.send(data=q.data[0].data)

This is a simple example, read the documentation for information: https://github.com/rfquack/RFQuack

## Public Demo

* RX example: https://twitter.com/JoelSernaMoreno/status/1354854848299208706

# Evil Crow RF Support

* You can ask in the Discord group: https://discord.gg/jECPUtdrnW
* You can open issue or send me a message via twitter (@JoelSernaMoreno).
