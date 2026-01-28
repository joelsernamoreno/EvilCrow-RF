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
	* Home
	* RX Config
	* Log Viewer
	* TX Config
	* Config
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
3. Download and install the latest version of Arduino IDE (v2.3.6): https://www.arduino.cc/en/main/software
4. Download EvilCrow-RF repository: git clone https://github.com/joelsernamoreno/EvilCrow-RF.git
5. Download the ESPAsyncWebServer library in the Arduino library directory: git clone https://github.com/ESP32Async/ESPAsyncWebServer.git
6. Download the ElegantOTA library in the Arduino library directory: git clone https://github.com/ayushsharma82/ElegantOTA.git
7. Edit ElegantOTA/src/ElegantOTA.h and chage the following:

* #define ELEGANTOTA_USE_ASYNC_WEBSERVER 0 to #define ELEGANTOTA_USE_ASYNC_WEBSERVER 1

8. Download the AsyncTCP library in the Arduino library directory: git clone https://github.com/ESP32Async/AsyncTCP.git
9. Open Arduino IDE
10. Go to File - Preferences. Locate the field "Additional Board Manager URLs:" Add "https://espressif.github.io/arduino-esp32/package_esp32_index.json" without quotes. Click "Ok"
11. Select Tools - Board - Boards Manager. Search for "esp32". Install "esp32 by Espressif system version 3.3.2". Click "Close".
12. Open the EvilCrow-RF/firmware/firmware/firmware.ino sketch
13. Select Tools:
    * Board - "ESP32 Dev Module".
    * Flash Size - "4MB (32Mb)".
    * CPU Frequency - "80MHz (WiFi/BT)".
    * Flash Frequency - "40MHz"
    * Flash Mode - "DIO"
14. Upload the code to the Evil Crow RF device
15. Press reset button

## First steps with Evil Crow RF

1. Set up a Wi-Fi AP with your mobile phone:
	* **SSID:** Evil Crow RF
	* **Password:** 123456789ECRFv1
2. Connect your laptop to the same Wi-Fi network.
3. Open a browser and access the web panel: http://evilcrow-rf.local/

**Note:** If you cannot access the web panel, use the IP address assigned to Evil Crow RF or follow below steps **only if you are running Linux OS:**
 * check if avahi-deamon is installed and running on your PC. You can do this with executing "sudo systemctl status avahi-daemon" in terminal
 * If service is not running, install it using your package manager (apt, yum, dnf, Packman, rpm,...)
 * After successful installation, start avahi-daemon service with "sudo systemctl start avahi-daemon && sudo systemctl enable avahi-daemon"
 * In case evilcrow-rf.local is still not reachable, use http://"IP address", where "IP address" is IP assigned to Evil Crow RF.
 
## Home

The Home page shows interesting information about the device.

![Home](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/home.png)

## RX Config

The RX Config page allows you to configure the CC101 modules for receiving signals. The received signals are displayed in the Log Viewer.

* Module: (1 for first CC1101 module, 2 for second CC1101 module)
* Modulation: (example ASK/OOK)
* Frequency: (example 433.92)
* Rx bandwidth: (example 200)
* Deviation: (example 0)
* Data rate: (example 5)

![RX](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/rx.png)

## Log Viewer

![RXLog](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/rx-log.png)

## TX Config

The TX Config page allows you to transmit a raw data signal or enable/disable the jammer.

![TXCONFIG](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/txconfig.png)

* **TX Raw Data:**

* Module: (1 for first CC1101 module, 2 for second CC1101 module)
* Modulation: (example ASK/OOK)
* Frequency: (example 433.92)
* RAW Data: (raw data or raw data corrected displayed in Log Viewer)
* Deviation: (example 0)

![TXRAW](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/txraw.png)

* **Jammer:**

* Module: (1 for first CC1101 module, 2 for second CC1101 module)
* Frequency: (example: 433.92)
* Jammer Power: (example: 12)

![JAMMER](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/jammer.png)

## Config

The Config page allows you to change the Wi-Fi configuration.

**NOTE:** Evil Crow RF is configured in STATION MODE. You can change the ssid and password from the web panel.

The changes will be stored in the device, every time you restart Evil Crow RF the new Wi-Fi settings will be applied. If you want to return to the default settings, you can delete the stored Wi-Fi configuration from the web panel.

![CONFIG](https://github.com/joelsernamoreno/EvilCrow-RF/blob/main/images/configwifi.png)

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
