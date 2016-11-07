<<<<<<< HEAD
/***********************************************************************
* August 08,2016 LensDigital 
* See www.xronosclock.com for project details, schematics, etc.
* Main features in V3: 
* Support for both Xronos 2 and upcoming Xronos 3 clocks
* DST and Timezones support
* Supports both RFM12b and RFM69W (Xronos 3).
* NOTE:  
* TO use with Xronos 2 UNCOMMENT #define XRONOS2 here,  HT1632.h and WavePinDefs.h (part of WaveHC library) files (for Xronos 3, comment them out)
* 
* Known issues: 
* - When changing hours during timesetting, date might change due to possible bug in Time or Timezone library. Always check day after setting hours.
* 
* V01: Fix hours not showing correct time in 12hr mode and AM dot
* V02: Fix date (was showing UTC)
* V03: UTC Timezone (no DTS)
* V04: Switch back to RFM12b library
* V05: Fix Radio stats, fix QuickMenu (alarm not annouced), fix Radio Frequency display
* V06: Show date in Radio Stats if it hasn't updated in long time
*/
#define XRONOS2 // Comment out for Xronos 3

#include "xronos3.h"
#define firmware_ver 306 // Current Firmware version

void setup ()  
{
  Serial.begin(115200);
  //initEPROM();
  getEEPROMSettings ();
  wdt_enable(WDTO_8S); // Watchdog will reboot clock if it's stuck for more than 8seconds
  // Init Input buttons
  for (byte i=0;i<3;i++) pinMode(buttonPins[i], INPUT_PULLUP);
  // Print FW Version
  showFWVersion();
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if(timeStatus()!= timeSet) 
     putstring_nl("Unable to sync with the RTC");
  else
     putstring_nl("RTC has set the system time");    
  ht1632_setup();  // Setup LED Deisplay
  // Uncomment following two lines and modify to set time. After setting time, commend them and re-upload sketch
  //setTime(13,04,0,9,11,12); // Set time on Arduino (hr,min,sec,day,month,yr). Use with RTC.set(now());
  //RTC.set(now()); // Write Time data to RTC Chip. Use with previous command
   // ========= Read Settings from EEPROM ===============================
  
 TempInit();
 // Set initial brightness
 if (brightness==0) autoBrightness();
 else
  setBrightness(brightness);
 wave.volume=sndVol; // Change System Sound Volume
 WaveShieldInit();
 radioInit();
 //if (isRadioPresent) radio.Initialize(NODEID, RF12_915MHZ, NETWORKID);  
 if (isIRPresent)  irrecv.enableIRIn(); // Start the IR receiver. Comment out if IR not present
  startup(); // Show welcoming screen
 Serial.println (FreeRam());
 //radio.Sleep();
}


// =====================================================================================
// >>>>>>>>>>>>>>>>>>>>>>>>> MAIN LOOP <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// =====================================================================================
void loop ()
{
 wdt_reset(); // Resets Watchdog so clock doesn't reboot (keepalive)
 showBigTime(clockColor);
 procAlarm(0);
 procAlarm(1);
 buttonProc();
 rearmAlrm(0);
 rearmAlrm(1);
 infoDisplay();
 autoBrightness ();
 recieveTemp();
 IR_process();
 reEnableRadio();
 indicatorStatus();
 
}
// =====================================================================================
// >>>>>>>>>>>>>>>>>>>>>>>>> END MAIN LOOP <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// =====================================================================================

/*
void myDebug(){
  //Serial.print ("soundAlarm=");Serial.println(soundAlarm);
   //Serial.print ("interruptAlarm=");Serial.println (interruptAlrm); 
  // Serial.print ("SnozeTime="); Serial.println (snoozeTime); 
}


*/
// ===================================================================
// * READ EEPROM Settings *
// ===================================================================
void getEEPROMSettings () {
  byte alrmTst;
  for (byte i=0;i<2;i++) {
    alarmon[i]=EEPROM.read(alarmOnOffLoc[i]); // read Alarm off/Daily/Weekday from EEPROM
    alrmHH[i]=EEPROM.read(alarmHHLoc[i]);  // Read Alarm Hours from EEPROM
    alrmMM[i]=EEPROM.read(alarmMMLoc[i]); // Read Alarm Minutes from EEPROM
    alrmToneNum[i]=EEPROM.read(alarmToneLoc[i]); // Read Alarm Tone number from EEPROM
    // Check if custom alarm schdule is set
    alrmTst=alarmon[i]<<1; // Shift left one to get rid of 1st bit
    alrmTst=alrmTst>>1; // Shift right one to get rid of 1st bit
    if ( (alrmTst == 124) || (alrmTst == 127) ) isSettingAlrmCust[i]=false; // i.e. Daily or Weekday alarm is set
    else isSettingAlrmCust[i]=true; // It's set to custom day
  }
  time12hr=EEPROM.read(mode24HRLoc); // Read time mode (12/24 hours) from EEPROM
  brightness=EEPROM.read(brightLoc); // Read Brightness setting from EEPROM
  clockColor=EEPROM.read(clockColorLoc); // Read Clock Color from EEPROM
  if (clockColor!=RED && clockColor!=GREEN && clockColor!=ORANGE) clockColor=ORANGE;// Failsafe for when EEPROM location is blank or corrupted, so clock will be visible
  clockFont=EEPROM.read(clockFontLoc); // Read Alarm Tone number from EEPROM
  sFX=EEPROM.read(sFXLoc); // Read Sound FX on/of for menu system
  tempUnit=EEPROM.read(tempUnitLoc); // Read temp unit
  sndVol=EEPROM.read(sndVolLoc); // Read Sound Volume
  infoFreq=EEPROM.read(infoFreqLoc); // Read infodisplay options
  infoOptions=EEPROM.read(infoOptionsLoc); // Read infodisplay options
  sayOptions=EEPROM.read(sayOptionsLoc); // Read say prompt options
  doStartup=EEPROM.read(doStartupLoc); // Read say prompt options
  tmpOffset=EEPROM.read (tmpOffsetLoc); // Read Temperature offset
  if ( RFM_Enabled ) isRadioPresent=EEPROM.read (radioOnLoc);
  else isRadioPresent=false;
  if (!isRadioPresent) { // Disable saying ext temperature/humidity
    if (sayOptions & SO_OutTemp) sayOptions = sayOptions ^ SO_OutTemp;
    if (sayOptions & SO_OutHum) sayOptions = sayOptions ^ SO_OutHum;
  }
  if (EEPROMReadInt (clockVerLoc) != firmware_ver) { //Write current firmware version to EEPROM if it's different from stored
    putstring_nl("Writing new Fw version");
    EEPROMWriteInt (clockVerLoc,firmware_ver); 
    delay (15); 
  }
  if ( IR_PRESENT ) isIRPresent=EEPROM.read (IROnLoc); // IR reciever setting
  else isIRPresent=false;
  RF_Frequency=EEPROM.read(RF_FreqLoc); // Frequency (0=915Mhz, 1=433Mhz, 2=868Mhz)
  
  RF_Node=EEPROM.read (RF_NodeLoc); // Read Node ID
  RF_Gateway=EEPROM.read (RF_GatewayLoc); // Read Gateway
  RF_Network=EEPROM.read (RF_NetworkLoc); // Read Network ID
  RF_SensorID=EEPROM.read (RF_SensorLoc); // Read Sensor Node ID
  RF_Encrypt=EEPROM.read(RF_EncryptLoc); // Encryption on/off
}

// ===================================================================
// * Decode IR Codes *
// Comment out if IR reciever not present
// ===================================================================
void IR_process () {
  if (!isIRPresent) return;
  if (irrecv.decode(&results)) {
    switch (results.value) {
      case IR_ON:
      //Serial.println ("Recieved ON/OFF");
      lastButtonTime=millis()+ BOUNCE_TIME_BUTTON;
      processMenuButton();
      break;
      case IR_PLUS:
      //Serial.println ("Recieved PLUS");
      decrement=false;
      lastButtonTime=millis()+ BOUNCE_TIME_BUTTON;
      processIncButton();
      break;
      case IR_MINUS:
      //Serial.println ("Recieved MINUS");
      decrement=true;
      lastButtonTime=millis()+ BOUNCE_TIME_BUTTON;
      processIncButton();
      break;
      case IR_UP:
      //Serial.println ("Recieved UP");
      lastButtonTime=millis()+ BOUNCE_TIME_BUTTON;
      processSetButton();
      break;
      case IR_DOWN:
      //Serial.println ("Recieved DOWN");
      lastButtonTime=millis()+ BOUNCE_TIME_BUTTON;
      processSetButton();
      break;
      case IR_ENTER: // Talk All Items
      //Serial.println ("Recieved ENTER");
      lastButtonTime=millis()+BOUNCE_TIME_QUICK;
      //buttonReleased=true;
      //last_ms=millis()+heldTime;
      butActivated[INC_BUTTON]=true;
      buttonsStatus[INC_BUTTON] = B_HELD;
      quickMenu();
      break;
      case IR_TALK: // Start Talk function
      //Serial.println ("Recieved MUTE");
      lastButtonTime=millis()+BOUNCE_TIME_QUICK;
      //buttonReleased=true;
     // last_ms=millis();
      butActivated[INC_BUTTON]=true;
      buttonsStatus[INC_BUTTON] = B_RELEASED;
      quickMenu();
      break;

    //Serial.println(results.value, HEX);
    
    }
    irrecv.resume(); // Receive the next value
  }
}

void showFWVersion(){
  char welcome[15];
    int ver=EEPROMReadInt (clockVerLoc); // Read 3 digit version number
    byte first=ver/100; // Thousand
    byte scnd =(ver%100)/10;
    byte third=ver%10;
    snprintf(welcome, sizeof(welcome),"Firmware:V%d.%d%d",first,scnd,third); 
    Serial.println (welcome);
}
=======
/***********************************************************************
* August 08,2016 LensDigital 
* See www.xronosclock.com for project details, schematics, etc.
* Main features in V3: 
* Support for both Xronos 2 and upcoming Xronos 3 clocks
* DST and Timezones support
* Supports both RFM12b and RFM69W (Xronos 3).
* NOTE:  
* TO use with Xronos 2 UNCOMMENT #define XRONOS2 here,  HT1632.h and WavePinDefs.h (part of WaveHC library) files (for Xronos 3, comment them out)
* 
* Known issues: 
* - When changing hours during timesetting, date might change due to possible bug in Time or Timezone library. Always check day after setting hours.
* 
* V01: Fix hours not showing correct time in 12hr mode and AM dot
* V02: Fix date (was showing UTC)
* V03: UTC Timezone (no DTS)
* V04: Switch back to RFM12b library
* V05: Fix Radio stats, fix QuickMenu (alarm not annouced), fix Radio Frequency display
*/
#define XRONOS2 // Comment out for Xronos 3

#include "xronos3.h"
#define firmware_ver 305 // Current Firmware version

void setup ()  
{
  Serial.begin(115200);
  //initEPROM();
  getEEPROMSettings ();
  wdt_enable(WDTO_8S); // Watchdog will reboot clock if it's stuck for more than 8seconds
  // Init Input buttons
  for (byte i=0;i<3;i++) pinMode(buttonPins[i], INPUT_PULLUP);
  // Print FW Version
  showFWVersion();
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if(timeStatus()!= timeSet) 
     putstring_nl("Unable to sync with the RTC");
  else
     putstring_nl("RTC has set the system time");    
  ht1632_setup();  // Setup LED Deisplay
  // Uncomment following two lines and modify to set time. After setting time, commend them and re-upload sketch
  //setTime(13,04,0,9,11,12); // Set time on Arduino (hr,min,sec,day,month,yr). Use with RTC.set(now());
  //RTC.set(now()); // Write Time data to RTC Chip. Use with previous command
   // ========= Read Settings from EEPROM ===============================
  
 TempInit();
 // Set initial brightness
 if (brightness==0) autoBrightness();
 else
  setBrightness(brightness);
 wave.volume=sndVol; // Change System Sound Volume
 WaveShieldInit();
 radioInit();
 //if (isRadioPresent) radio.Initialize(NODEID, RF12_915MHZ, NETWORKID);  
 if (isIRPresent)  irrecv.enableIRIn(); // Start the IR receiver. Comment out if IR not present
  startup(); // Show welcoming screen
 Serial.println (FreeRam());
 //radio.Sleep();
}


// =====================================================================================
// >>>>>>>>>>>>>>>>>>>>>>>>> MAIN LOOP <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// =====================================================================================
void loop ()
{
 wdt_reset(); // Resets Watchdog so clock doesn't reboot (keepalive)
 showBigTime(clockColor);
 procAlarm(0);
 procAlarm(1);
 buttonProc();
 rearmAlrm(0);
 rearmAlrm(1);
 infoDisplay();
 autoBrightness ();
 recieveTemp();
 IR_process();
 reEnableRadio();
 indicatorStatus();
 
}
// =====================================================================================
// >>>>>>>>>>>>>>>>>>>>>>>>> END MAIN LOOP <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// =====================================================================================

/*
void myDebug(){
  //Serial.print ("soundAlarm=");Serial.println(soundAlarm);
   //Serial.print ("interruptAlarm=");Serial.println (interruptAlrm); 
  // Serial.print ("SnozeTime="); Serial.println (snoozeTime); 
}


*/
// ===================================================================
// * READ EEPROM Settings *
// ===================================================================
void getEEPROMSettings () {
  byte alrmTst;
  for (byte i=0;i<2;i++) {
    alarmon[i]=EEPROM.read(alarmOnOffLoc[i]); // read Alarm off/Daily/Weekday from EEPROM
    alrmHH[i]=EEPROM.read(alarmHHLoc[i]);  // Read Alarm Hours from EEPROM
    alrmMM[i]=EEPROM.read(alarmMMLoc[i]); // Read Alarm Minutes from EEPROM
    alrmToneNum[i]=EEPROM.read(alarmToneLoc[i]); // Read Alarm Tone number from EEPROM
    // Check if custom alarm schdule is set
    alrmTst=alarmon[i]<<1; // Shift left one to get rid of 1st bit
    alrmTst=alrmTst>>1; // Shift right one to get rid of 1st bit
    if ( (alrmTst == 124) || (alrmTst == 127) ) isSettingAlrmCust[i]=false; // i.e. Daily or Weekday alarm is set
    else isSettingAlrmCust[i]=true; // It's set to custom day
  }
  time12hr=EEPROM.read(mode24HRLoc); // Read time mode (12/24 hours) from EEPROM
  brightness=EEPROM.read(brightLoc); // Read Brightness setting from EEPROM
  clockColor=EEPROM.read(clockColorLoc); // Read Clock Color from EEPROM
  if (clockColor!=RED && clockColor!=GREEN && clockColor!=ORANGE) clockColor=ORANGE;// Failsafe for when EEPROM location is blank or corrupted, so clock will be visible
  clockFont=EEPROM.read(clockFontLoc); // Read Alarm Tone number from EEPROM
  sFX=EEPROM.read(sFXLoc); // Read Sound FX on/of for menu system
  tempUnit=EEPROM.read(tempUnitLoc); // Read temp unit
  sndVol=EEPROM.read(sndVolLoc); // Read Sound Volume
  infoFreq=EEPROM.read(infoFreqLoc); // Read infodisplay options
  infoOptions=EEPROM.read(infoOptionsLoc); // Read infodisplay options
  sayOptions=EEPROM.read(sayOptionsLoc); // Read say prompt options
  doStartup=EEPROM.read(doStartupLoc); // Read say prompt options
  tmpOffset=EEPROM.read (tmpOffsetLoc); // Read Temperature offset
  if ( RFM_Enabled ) isRadioPresent=EEPROM.read (radioOnLoc);
  else isRadioPresent=false;
  if (!isRadioPresent) { // Disable saying ext temperature/humidity
    if (sayOptions & SO_OutTemp) sayOptions = sayOptions ^ SO_OutTemp;
    if (sayOptions & SO_OutHum) sayOptions = sayOptions ^ SO_OutHum;
  }
  if (EEPROMReadInt (clockVerLoc) != firmware_ver) { //Write current firmware version to EEPROM if it's different from stored
    putstring_nl("Writing new Fw version");
    EEPROMWriteInt (clockVerLoc,firmware_ver); 
    delay (15); 
  }
  if ( IR_PRESENT ) isIRPresent=EEPROM.read (IROnLoc); // IR reciever setting
  else isIRPresent=false;
  RF_Frequency=EEPROM.read(RF_FreqLoc); // Frequency (0=915Mhz, 1=433Mhz, 2=868Mhz)
  
  RF_Node=EEPROM.read (RF_NodeLoc); // Read Node ID
  RF_Gateway=EEPROM.read (RF_GatewayLoc); // Read Gateway
  RF_Network=EEPROM.read (RF_NetworkLoc); // Read Network ID
  RF_SensorID=EEPROM.read (RF_SensorLoc); // Read Sensor Node ID
  RF_Encrypt=EEPROM.read(RF_EncryptLoc); // Encryption on/off
}

// ===================================================================
// * Decode IR Codes *
// Comment out if IR reciever not present
// ===================================================================
void IR_process () {
  if (!isIRPresent) return;
  if (irrecv.decode(&results)) {
    switch (results.value) {
      case IR_ON:
      //Serial.println ("Recieved ON/OFF");
      lastButtonTime=millis()+ BOUNCE_TIME_BUTTON;
      processMenuButton();
      break;
      case IR_PLUS:
      //Serial.println ("Recieved PLUS");
      decrement=false;
      lastButtonTime=millis()+ BOUNCE_TIME_BUTTON;
      processIncButton();
      break;
      case IR_MINUS:
      //Serial.println ("Recieved MINUS");
      decrement=true;
      lastButtonTime=millis()+ BOUNCE_TIME_BUTTON;
      processIncButton();
      break;
      case IR_UP:
      //Serial.println ("Recieved UP");
      lastButtonTime=millis()+ BOUNCE_TIME_BUTTON;
      processSetButton();
      break;
      case IR_DOWN:
      //Serial.println ("Recieved DOWN");
      lastButtonTime=millis()+ BOUNCE_TIME_BUTTON;
      processSetButton();
      break;
      case IR_ENTER: // Talk All Items
      //Serial.println ("Recieved ENTER");
      lastButtonTime=millis()+BOUNCE_TIME_QUICK;
      //buttonReleased=true;
      //last_ms=millis()+heldTime;
      butActivated[INC_BUTTON]=true;
      buttonsStatus[INC_BUTTON] = B_HELD;
      quickMenu();
      break;
      case IR_TALK: // Start Talk function
      //Serial.println ("Recieved MUTE");
      lastButtonTime=millis()+BOUNCE_TIME_QUICK;
      //buttonReleased=true;
     // last_ms=millis();
      butActivated[INC_BUTTON]=true;
      buttonsStatus[INC_BUTTON] = B_RELEASED;
      quickMenu();
      break;

    //Serial.println(results.value, HEX);
    
    }
    irrecv.resume(); // Receive the next value
  }
}

void showFWVersion(){
  char welcome[15];
    int ver=EEPROMReadInt (clockVerLoc); // Read 3 digit version number
    byte first=ver/100; // Thousand
    byte scnd =(ver%100)/10;
    byte third=ver%10;
    snprintf(welcome, sizeof(welcome),"Firmware:V%d.%d%d",first,scnd,third); 
    Serial.println (welcome);
}
>>>>>>> origin/master

