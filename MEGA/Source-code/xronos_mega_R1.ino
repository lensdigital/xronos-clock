/* =============================================================================================
* Xronos Mega Clock R1 by Lensdigital.com (2016)
* See www.xronosclock.com for project details, schematics, etc.
* Large wall smart clock with four 24x16 LED Matrix panels
* ATMega1284p based. Features: 
*  Ambient light sensor
*  Temperature sensor
*  RFM transciever
*  IR Remote reciever
*  GPS Reciever support (for automatic time setting)
* Currently only USA Timezones supported (automatic DST adjustment)
* Note: Some settings changes (i.e. RFM, GPS, TimeZone) will cause (planned) processor reset 
* V01 (2016-07-26): First production (non beta version).
* V02 (2016-07-30): Var for RFM Frequency, print out radio details at startup
* V03 (2016-07-31): Radio options menu. Some menu reorganization. Watchdog Reset (if clock is frozen) functionality.
* V04 (2016-08-10): Move everything to header
* V05 (2016-08-13): Fix bug with 12hr mode (hourFormat12). Added indicator status onscreen. Setup User options to controll temperature/humidity scrolling. UTC Timezone added. Added IR
 =============================================================================================
*/
#define firmware_ver 105 // Current Firmware version

#include "xronos_mega.h" // Contains most  variables and definitions

void setup ()  
{
 wdt_enable(WDTO_8S); // Watchdog will reboot clock if it's stuck for more than 8seconds
// initEPROM();
 getEEPROMSettings ();
 SerialGPS.begin(9600);
 if (!GPSEnabled) Serial.begin(115200);
  
   // Print FW Version
    char welcome[15];
    int ver=firmware_ver;
    byte first=ver/100; // Thousand
    byte scnd =(ver%100)/10;
    byte third=ver%10;
    snprintf(welcome, sizeof(welcome),"Firmware:V%d.%d%d",first,scnd,third); 
    Serial.println (welcome);
 
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if(timeStatus()!= timeSet) 
     Serial.println  (F("Unable to sync with the RTC"));
  else
     Serial.println  (F("RTC has set the system time"));    
  ht1632_setup();  // Setup display (uses flow chart from page 17 of sure datasheet)
  
 // wave.volume=sndVol; // Change System Sound Volume
 // WaveShieldInit();
  TempInit();
  // Set initial brightness
  if (brightness==0) autoBrightness();
  else
    setBrightness(brightness);
  radioInit();// INitializes RFM chip
  if (isIRPresent)  irrecv.enableIRIn(); // Start the IR receiver. Comment out if IR not present
  startup(); // Show welcoming screen
  //Serial.println (FreeRam());
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  //delay (3000); // Get GPS time to get location
} 
  
void loop (){ 
  wdt_reset(); // Resets Watchdog so clock doesn't reboot (keepalive)
  
  showBigTime(GREEN);
  autoBrightness ();
  scrollDate(0,0);  
  showTemp();
  //showTempExt();
  showHumidityExt();
  buttonProc();
  //okDate=true;
  showMsg(0);
  IR_process();
  getGPSTIme();
  recieveTemp();
  indicatorStatus();
  
}
  

void getTstMsg() {
  if (radio.ReceiveComplete())
  {
    if (radio.CRCPass())
    {
      Serial.print ("Recieved: ");
      for (byte i = 0; i < radio.GetDataLen(); i++)
       Serial.print((char)radio.Data[i]);

      if (radio.ACKRequested())
      {
        radio.SendACK();
        Serial.println("");
        Serial.print("ACK sent");
      }
    }
    else
      Serial.print("BAD-CRC");
    
    Serial.println();
  }
}
