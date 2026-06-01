# ESP32-HC12
ESP32 Nano and HC12 module to send time to another micro controller via the RX port at 9600 baud.

Time is send in the format Thhmmss. (T121500, quarter past 12)

To update time on my [ATMEGA word clock designs](https://github.com/ednieuw/Woordklok-witte-LEDs) an Arduino Nano ESP32 is used to receive time with WFI/NTP and send it with a HC-12 S14438 433 MHz Long-range wireless serial module to the ATMEGA.

The sketch is derived from [a larger common ESP32 communication project](https://github.com/ednieuw/ESP32Communications). 

The ESP32 communication project was stripped from my [Nano ESP32 word clock project](https://github.com/ednieuw/Arduino-ESP32-Nano-Wordclock) that contains (in 2026) the complete manual and instructions

The Reset settings of this sketch, version V029, are optimised for the HC-12 time sender that will be set default ON after a reset.




![alt text](PCB-HC12-1.jpg)

![alt text](IMG_6051(1)-1.JPG)

![alt text](IMG_6050(1)-1.JPG)

