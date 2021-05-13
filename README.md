# EvilCrow-RF

![EvilCrow](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/Logo1.png)

**Idea, development and implementation:** Joel Serna (@JoelSernaMoreno).

**PCB design:** Ignacio Díaz Álvarez (@Nacon_96) and Forensic Security (@ForensicSec).

**Manufacturer and distributor:** April Brother (@aprbrother).

**Collaborators:** Little Satan, Ernesto Sánchez (@ernesto_xload), Federico Maggi (@phretor), Andrea Guglielmini (@Guglio95) and RFQuack (@rfquack).

The developers and collaborators of this project do not earn money with this. 
You can invite me for a coffee to further develop Low-Cost hacking devices. If you don't invite me for a coffee, nothing happens, I will continue developing devices.

[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/E1E614OA5)

**For Sale at:** 

* [Aliexpress Shop](https://aliexpress.com/item/1005002476481892.html)

**Summary:**

1. Disclaimer
2. Introduction
3. Basic Firmware
	* Installation
	* First steps with EvilCrow-RF
	* RX Config Example
	* RX Log Example
	* TX Example
	* Brute Force Example
	* Pushbuttons Configuration
	* Other Sketches
	* Public Demo
4. Evil Crow RF RAW
	* RAW RX Config Example
	* RAW Log Example
	* RAW TX Config Example
5. Advanced Firmware with RFQuack
	* Installation and first steps
	* RX Example
	* TX Example
	* Public Demo
6. Evil Crow RF Support

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

# Basic Firmware

The basic firmware allows to receive and transmit basic signals. You can configure the two radio modules through a web panel via WiFi.

* **RX:** Configure modules and frequency for reception.
* **TX:** Configure modules, frequency, code and bit length to transmit.
* **Bruteforce:** Configure frequency, start code and bit length to brute force. 

## Installation

1. Install esptool: sudo apt install esptool
2. Install pyserial: sudo pip install pyserial
3. Download and Install the Arduino IDE: https://www.arduino.cc/en/main/software
4. Download EvilCrow-RF repository: git clone https://github.com/joelsernamoreno/EvilCrow-RF.git
5. Copy the SmartRC-CC1101-Driver-Lib library included in the EvilCrow-RF repository into Arduino library directory
6. Copy the rc-switch library included in the EvilCrow-RF repository into Arduino library directory
7. Open Arduino IDE
8. Go to File - Preferences. Locate the field "Additional Board Manager URLs:" Add "https://dl.espressif.com/dl/package_esp32_index.json" without quotes. Click "Ok"
9. Select Tools - Board - Boards Manager. Search for "esp32". Install "esp32 by Espressif system version 1.0.4". Click "Close".
10. Open the EvilCrow-RF.ino sketch
11. Select Tools:
    * Board - "ESP32 Dev Module".
    * Flash Size - "4MB (32Mb)".
    * CPU Frequency - "240MHz (WiFi/BT)".
    * Flash Frequency - "80MHz"
    * Flash Mode - "DIO"
12. Upload the code to the EvilCrow-RF device.
13. Press reset button

## First steps with EvilCrow-RF

1. Visualize the wifi networks around you and connect to the EvilCrow-RF (default SSID: EvilCrow-RF).
2. Enter the password for the wifi network (default password: 123456789).
3. Open a browser and access the web panel (default IP: 192.168.4.1).
4. Go!

## RX Config Example

* Module: 1 or 2 (1 for first CC1101 module, 2 for second CC1101 module)
* Frequency (example 433.92)
* RxBW bandwidth (Example 58)

![RXConfig](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/rx-config.png)

## RX Log Example

![RXLog](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/rx-log.png)

## TX Example

* Module: 1 or 2 (1 for first CC1101 module, 2 for second CC1101 module)
* Frequency (example 433.92)
* Code (example 1642498)
* Bit Length (example 24)

![TXConfig](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/tx-config.png)

## Brute Force Example

* Frequency (example 433.92)
* Start Code (example 1642490)
* Bit Length (example 24)

![BruteForce](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/brute-force.png)

## Pushbuttons Configuration

![PB](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/pb.png)

Configure actions for the pushbuttons. Edit the EvilCrow-RF.ino sketch with your new code:

![Pushbutton](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/pushbutton.png)

## Other Sketches

You can program Evil Crow RF with other configurations without using the basic firmware. In the OtherSketches folder you can find two examples: 

* ASK_RAW_RX.ino: Receive raw data (Only ASK)
* ASK_RAW_TX.ino: Transmit raw data (Only ASK)

Read the code to know how to configure this for your environment.

Additionally, you can develop other sketches for Evil Crow RF and PR to this repository :)

## Public Demo:

* Replay attack with pushbuttons: https://twitter.com/JoelSernaMoreno/status/1343573202967126022
* Simple Brute Force: https://twitter.com/JoelSernaMoreno/status/1344798890516770817
* Hacking a X-RAY Machine with WHIDelite & EvilCrowRF: https://lucabongiorni.medium.com/hacking-a-x-ray-machine-with-whidelite-evilcrowrf-74b871f8e23b

# Evil Crow RF RAW

Firmware for receiving and transmitting raw data (Web Panel):

* SSID: RAW Replay
* Password: 123456789
* IP: 192.168.4.1

## RAW RX Config Example

* Frequency (example: 433.92)

* RxBW bandwidth (example: 812)

* Modulation (example: 2) // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.

* Desviation (example: 30.00) // Set the Frequency deviation in kHz. Value from 1.58 to 380.85. Default is 47.60 kHz.

* Data Rate (example: 5) // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!

Example RAW RX Config ASK/OOK:

![RAWRXConfig](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/rawrx.png)

## RAW Log Example

![RAWLog](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/rawlog.png)

## RAW TX Config Example

* Frequency (example: 433.92)
* Modulation (example: 2)
* RAW DATA (example: 347,23,65,23,54,56,....)
* Deviation (example: 30.00)

Example RAW RX Config ASK/OOK:

![RAWTXConfig](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/rawtx.png)

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

You can open issue or send me a message via twitter (@JoelSernaMoreno).
