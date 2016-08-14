Expermiental firmware for Xronos Mega Clock (Four 24x16 LED Matrix screens(single color), GPS)

Uploading instructions (Windows):
Upload via FTDI or similar USB to Serial adapter (6 pin).
You can either use Arduino IDE 1.6.10 with included sketch (make sure you have all required libraries)
To upload hex firmware:
  - copy AVRDUDE version 6.3 to know location (i.e. C:\Xronos\AVR). Are required files included with Arduino IDE 1.6.10
  - save all *.bat and *.hex files to some folder on your computer
  - modify update_xronos.bat and specify AVRPath and AVRConfig paths if different.
  - Run start_here.bat and specify COM port of FTDI adapter

Issues:
  - Currently (version 05) user options settings affecting scrolling are not working yet.
  - There might be issue with RGB LED indicator not working correctly
  - When settings hours, date might change, so always check date after settings hours. If you usign GPS, clock will set itself correctly.
  - RFM Frequency not reported correctly via Serial (minor bug)

GPS and Serial:
  PLease keep in mind that GPS uses same Serial (RX) port as FTDI programmer. This doesn't affect programming clock but please be aware of following limitations:
  - When FTDI adapter is plugged in, GPS will not communicate with clock
  - When GPS function is enabled in menu, you will not see serial output from clock (disable it to see debug info)

