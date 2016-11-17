While there's no release date for Xronos 3 clock (I'm having some serious conflicts with Audio and RFM parts), I rewrote most of the code and it's now compatible with Xronos V2!
*** Firmware V2 is no longer suppored to maitananed ***
What's new in V3 firmware:
- Supports both DS3231 and DS1307 RTCs chips
- DST and all US Timezones are now (finally!) supported (init EEPROM to write all TZ rules).
- Supports both RFM12b and RFM69w Trancievers (although RFM69w can cause clock to lock up due to SPI conflic with SD library)
- Radio settings (such as frequency, node IDs, etc.) can now be changed in Setup menu
- Most definitions moved to xronos3.h file for cleaner look
- Si7021 temperature and humidity internal sensor now supported
- Button logic was rewritten, holding INCR button for 2 seconds will activate accelerated incrementation in settings of the time, alarm, etc.
To use code for Xronos 2.1 clocks, just uncomment "#define XRONOS2" from main file as well as HT1632.h and WavePinDefs.h (part of modified WaveHC library)
You must initialize EEPROM to write all Timezone rules to it. Do it by either uncommenting initEEPROM() in setup function or thru the menu.
If using RFM12b library with Xronos 2 Clock, please repalce RFM12b.h file with file included in this repository
You will also need a timezone library (https://github.com/JChristensen/Timezone).

NOTICE: To compile code you need Arduino IDE 1.6.10 or higher. It won't work in previous versions.
To upload firmware hex file directly, you will need AVRDUDE version 6.3!
