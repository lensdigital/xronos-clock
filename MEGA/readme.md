Expermiental firmware for Xronos Mega Clock (Four 24x16 LED Matrix screens(single color), GPS)

Issues:
  - Currently (version 05) user options settings affecting scrolling are not working yet.
  - There might be issue with RGB LED indicator not working correctly
  - When settings hours, date might change, so always check date after settings hours. If you usign GPS, clock will set itself correctly.
  - RFM Frequency not reported correctly via Serial (minor bug)

GPS and Serial:
  PLease keep in mind that GPS uses same Serial (RX) port as FTDI programmer. This doesn't affect programming clock but please be aware of following limitations:
  - When FTDI adapter is plugged in, GPS will not communicate with clock
  - When GPS function is enabled in menu, you will not see serial output from clock (disable it to see debug info)

