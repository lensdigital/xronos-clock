// =======================================================================================
// ---- Display Current Internal/external Temperature (Screen 1) ----
// By: LensDigital
// =======================================================================================
void showTemp(){
  if (isInMenu) return;
  if (screenInUse==0) screenInUse=1; // Claim screen queue
  if (screenInUse!=1) return; // External Temp is showing
  if (pauseScroll) return;
  char myString[35];
  getTemp(1);
  float tempF = (tempC * 1.8) + 32.0; // Convert internal to Farenheit
  float xtempF = (extTemp * 1.8) + 32.0; // Convert external to Farenheit
  int tempCint = int(tempC +0.5) - tmpOffset;// Convert round off decimal to whole number.
  int tempFint = int(tempF +0.5) - tmpOffset; // Convert round off decimal to whole number.
  int xtempFint = int(xtempF +0.5); // Convert round off decimal to whole number.
  if (extTemp > 100 || extTemp < -50) snprintf(myString,sizeof(myString), "Temp: %dF(%dC)",tempFint,tempCint); // only show internal temp
  else snprintf(myString,sizeof(myString), "Temp(in/out):%dF(%dC)/%dF(%dC)",tempFint,tempCint,xtempFint,extTemp); // Show both internal/external temp
  scrolltextlimit (1,DISPLAY_SPLIT,8,myString,1);
  if (scrollDone[1]) { // Reset permits
    screenInUse=0; // Release Screen queue
    scrollDone[1]=false;
  }
 
}

// =======================================================================================
// ---- Display Current Internal/external Temperature (Screen 1) ----
// By: LensDigital
// =======================================================================================
void showTemp_new(){
  if (isInMenu) return;
  if (screenInUse==0) screenInUse=1; // Claim screen queue
  if (screenInUse!=1) return; // External Temp is showing
  if (pauseScroll) return;
  char myString[35];
  char inTempStringF[3]={'--'};
  char inTempStringC[3]={'--'};
  char outTempStringF[3]={'--'};
  char outTempStringC[3]={'--'};
  getTemp(1);
  float tempF = (tempC * 1.8) + 32.0; // Convert internal to Farenheit
  float xtempF = (extTemp * 1.8) + 32.0; // Convert external to Farenheit
  int tempCint = int(tempC +0.5) - tmpOffset;// Convert round off decimal to whole number.
  int tempFint = int(tempF +0.5) - tmpOffset; // Convert round off decimal to whole number.
  int xtempFint = int(xtempF +0.5); // Convert round off decimal to whole number.
  // Decision block (based on User Options)
  // Info: We check which option is enabled (i.e. show Internal or external temperature or both)
  //  Then we check if tempreature is valid (less then 100C but not less thatn -50C. If tempreature is out of range, we only display dashes instead of actual numbers.
  if ( (infoOptions & IO_InTemp) & (infoOptions & IO_OutTemp) ) { // Display of inside and/or outside temp enabled
    if (extTemp < 100 || extTemp > -50) {// External Temperature within norm
      itoa(xtempFint,outTempStringF,10); // Integer to text and fill temp buffer
      itoa(extTemp,outTempStringC,10); // Integer to text and fill temp buffer
    }
    if (tempC < 100 || tempC > -50) {// Internal Temperature within norm?
      itoa(tempFint,inTempStringF,10); // Integer to text and fill temp buffer
      itoa(tempCint,inTempStringC,10); // Integer to text and fill temp buffer
    }
    snprintf(myString,sizeof(myString), "Temp(in/out):%sF(%sC)/%sF(%sC)",inTempStringF,inTempStringC,outTempStringF,outTempStringC); // Show both internal/external temp
  }
  else if (infoOptions & IO_InTemp) { // Display of insidetemp enabled
    if (tempC < 100 || tempC > -50) {// Internal Temperature within norm?
      itoa(tempFint,inTempStringF,10); // Integer to text and fill temp buffer
      itoa(tempCint,inTempStringC,10); // Integer to text and fill temp buffer
    }
    snprintf(myString,sizeof(myString),"In Temp: %sF(%sC)",inTempStringF,inTempStringC); // Show both internal/external temp
  }
  else if (infoOptions & IO_OutTemp) { // Display of outside temp enabled
    if (extTemp < 100 || extTemp > -50) {// External Temperature within norm
      itoa(xtempFint,outTempStringF,10); // Integer to text and fill temp buffer
      itoa(extTemp,outTempStringC,10); // Integer to text and fill temp buffer
    }
    snprintf(myString,sizeof(myString),"Out Temp: %sF(%sC)",outTempStringF,outTempStringC); // Show both internal/external temp
  }
  scrolltextlimit (1,DISPLAY_SPLIT,8,myString,1);
  if (scrollDone[1]) { // Reset permits
    screenInUse=0; // Release Screen queue
    scrollDone[1]=false;
  }
 
}

// =======================================================================================
// ---- Display Current External Humidity  ----
// By: LensDigital
// =======================================================================================
void showHumidityExt(){
  if (isInMenu) return;
  if (!isRadioPresent) return;
  if (screenInUse==0) screenInUse=3; // Claim screen queue
  if (screenInUse!=3) return; // Screen queue is in use 
  if (pauseScroll) return;
  char myString[24];
  if (infoOptions & IO_OutHum) {
    if (extHum > 100 || extHum < 1) scrolltextlimit (1,DISPLAY_SPLIT,8,"External Humidity Sensor Error",1); // Humidity sensor is not working
    else {
      snprintf(myString,sizeof(myString), "Humidity outside is %2d%%",extHum); // Scroll Outside Humidity
      scrolltextlimit (1,DISPLAY_SPLIT,8,myString,1);
    }
    if (scrollDone[1]) { // Reset permits
     screenInUse=0; // Release Screen queue
      scrollDone[1]=false;
    }
  }
}

// =======================================================================================
// ---- Get temperature reading  at specified intervals (freq)----
// By: LensDigital
// =======================================================================================
void getTemp(byte freq)
{
  if ( (minute()%10)%freq == 0 && ( second()==5) )  { // Get reading every "freq" minute 5 sec
    pauseScroll=true;
    indicatorLED (GREEN_LED);
    sensors.begin();
    sensors.requestTemperatures(); // Send the command to get temperatures
    tempC = sensors.getTempC(insideThermometer);
    delay (500);
    pauseScroll=false;
    indicatorLED (BLACK_LED);
  }
}

// Display scrolling text message
void showMsg(byte disp) {
  if (!okMsg) return;
  okDate=false;
  scrolltextlimit (disp,DISPLAY_SPLIT,0,msgbuffer,1);
  if (scrollDone[disp]) { // Reset permits
    okDate=true;
    okMsg=false;
    scrollDone[disp]=false;
  }
}


// ===================================================================
// * DS18B20 Temperature sensor iniitialization *
// ===================================================================
void TempInit(){
 // ===================================================================
  // Temperature sensor init
 // Serial.print  (F("Found "));
 // Serial.print(sensors.getDeviceCount(), DEC);
 // Serial.println  (F(" DS18B20 sensors"));
  // report parasite power requirements
  Serial.print  (F("DS18B20 parasite power is: ")); 
  if (sensors.isParasitePowerMode()) Serial.println  (F("ON"));
  else Serial.println  (F("OFF"));
   if (!sensors.getAddress(insideThermometer, 0)) Serial.println  (F("Unable to find address for DS18B20")); 
 // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 9); 
  sensors.begin();
  sensors.requestTemperatures(); // Send the command to get temperatures
  tempC = sensors.getTempC(insideThermometer);
}


void startBlinking(){
  blinking = !isIncrementing;
}

void stopBlinking(){
  blinking = false;
}

// ===================================================================
// * READ EEPROM Settings *
// ===================================================================
void getEEPROMSettings () {
  if (EEPROM.read (clockVerLoc) != firmware_ver) { //Write current firmware version to EEPROM if it's different from stored
    Serial.println  (F("Writing new Fw version"));
    EEPROM.write (clockVerLoc,firmware_ver); 
    delay (15); 
  }
  time12hr=EEPROM.read(mode24HRLoc); // Read time mode (12/24 hours) from EEPROM
  brightness=EEPROM.read(brightLoc); // Read Brightness setting from EEPROM
  clockColor=EEPROM.read(clockColorLoc); // Read Clock Color from EEPROM
  if (clockColor!=RED && clockColor!=GREEN && clockColor!=ORANGE) clockColor=GREEN;// Failsafe for when EEPROM location is blank or corrupted, so clock will be visible
  clockFont=EEPROM.read(clockFontLoc); // Read Alarm Tone number from EEPROM
 // sFX=EEPROM.read(sFXLoc); // Read Sound FX on/of for menu system
  tempUnit=EEPROM.read(tempUnitLoc); // Read temp unit
 // sndVol=EEPROM.read(sndVolLoc); // Read Sound Volume
  infoFreq=EEPROM.read(infoFreqLoc); // Read infodisplay options
  infoOptions=EEPROM.read(infoOptionsLoc); // Read infodisplay options
 // sayOptions=EEPROM.read(sayOptionsLoc); // Read say prompt options
  doStartup=EEPROM.read(doStartupLoc); // Read say prompt options
  tmpOffset=EEPROM.read (tmpOffsetLoc); // Read Temperature offset
  if ( RFM12B_Enabled ) isRadioPresent=EEPROM.read (radioOnLoc);
  else isRadioPresent=false;
  
  if (!isRadioPresent) { // Disable saying ext temperature
    if (sayOptions & 4) sayOptions = sayOptions ^ 4;
  }
  LEDEnabled=EEPROM.read(LEDEnabledLoc);//Read LED indicator option
  GPSEnabled=EEPROM.read(GPSEnabledLoc);//Read GPS option
  RF_Frequency=EEPROM.read(RF_FreqLoc); // Frequency (0=915Mhz, 1=433Mhz, 2=868Mhz)
  RF_Node=EEPROM.read (RF_NodeLoc); // Read Node ID
  RF_Gateway=EEPROM.read (RF_GatewayLoc); // Read Gateway
  RF_Network=EEPROM.read (RF_NetworkLoc); // Read Network ID
  RF_SensorID=EEPROM.read (RF_SensorLoc); // Read Sensor Node ID
  RF_Encrypt=EEPROM.read(RF_EncryptLoc); // Encryption on/off
  isIRPresent=EEPROM.read (IROnLoc); // IR reciever setting
  
  //currTZ=EEPROM.read(currTZLoc); // Read Current Timezone Index
 // if (currTZ > 7) currTZ=0; // Failsafe in case EEPROM was not initialized
}

// =======================================================================================
// ---- Checks Ambient Light leve and adjust brightness ----
// ---- by LensDigital
// =======================================================================================
void autoBrightness () {
  if (isInMenu) return;
  if (brightness) return; // Brightness is not set to 0 (auto)
  if (second()%10) return; // Take readings every 10th second only
  //Serial.println (analogRead(photoCellPin));
  //Serial.println (lightLevel);
  if (prevBrightness==0) {  // Initialized previous Brightness setting only if Brightness was reset
    
    //prevBrightness=map(analogRead(photoCellPin), PHOTOCELL_MIN, PHOTOCELL_MAX, 1, 5); // Get Ambient Light Reading
    prevBrightness=map( constrain (analogRead(photoCellPin), PHOTOCELL_MIN, PHOTOCELL_MAX), PHOTOCELL_MIN, PHOTOCELL_MAX, 1, 5); // Get Ambient Light Reading
    setBrightness(prevBrightness); // Set LED brightness
  }
//  lightLevel = map(analogRead(photoCellPin), PHOTOCELL_MIN, PHOTOCELL_MAX, 1, 5); // Get Ambient Light Reading
  lightLevel = map( constrain (analogRead(photoCellPin), PHOTOCELL_MIN, PHOTOCELL_MAX), PHOTOCELL_MIN, PHOTOCELL_MAX, 1, 5); // Get Ambient Light Reading
  if (lightLevel != prevBrightness) { // Set LED brightness only if light changed
    setBrightness(lightLevel);
    prevBrightness=lightLevel;
    //Serial.println (lightLevel);
    //Serial.println (FreeRam());
  }
}

// ===================================================================
// * Decode IR Codes *
// Comment out if IR reciever not present
// ===================================================================
void IR_process () {
  //if (!isIRPresent) return;
  if (irrecv.decode(&results)) {
     /*
    if (irBlink) {
      irBlink=false;
      indicatorLED(YELLOW_LED);
    }
    else {
      irBlink=true;
      indicatorLED(BLACK_LED);
    }
    */
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
      buttonReleased=true;
      last_ms=millis()+heldTime;
      //quickDisplay();
      break;
      case IR_TALK: // Start Talk function
      //Serial.println ("Recieved MUTE");
      lastButtonTime=millis()+BOUNCE_TIME_QUICK;
      buttonReleased=true;
      last_ms=millis();
      //quickDisplay();
      
      break;

    //Serial.println(results.value, HEX);
    
    }
    irrecv.resume(); // Receive the next value
  }
}

// =======================================================================================
// ---- First time clock is powered, play startup music, show vers and say time ----
// ---------------------------------------------------------------------------------------
void startup ()
{
  if (!doStartup) return; // Startup Disabled?
  char welcome[15];
  byte ver=EEPROM.read (clockVerLoc); // Read 3 digit version number
  byte temp = (ver%100) %10; //temp holder
  byte ver3 = temp % 10; // Last digit
  byte ver2 = (temp - ver3) / 10; // Second Digit
  ver = (ver - ver2) / 100; // First digitf
//  playfile("startup1.wav");
  showText(0,0,"Welcome",3,GREEN);
  snprintf(welcome, sizeof(welcome),"Firmware:V%d.%d%d",ver,ver2,ver3); 
  //scrolltextsizexcolor(8,welcome,RED,15);
  cls();
  showBigTime(clockColor);
}

// =======================================================================================
// ---- Initializes EEPROM for new Chip. run only first time ----
// ---------------------------------------------------------------------------------------
void initEPROM()
{
  Serial.println  (F("Writing Initial settings to EEPROM!!!"));
  EEPROM.write (mode24HRLoc,false); // Write time mode (12/24 hours)
  delay (50);
  EEPROM.write (brightLoc,3); // Write Brightness setting
  delay (50);
  EEPROM.write (clockColorLoc,GREEN); // Write Clock Color
  delay (50);
  EEPROM.write (clockVerLoc,firmware_ver); 
  delay (50);
  EEPROM.write(clockFontLoc,1); // Write Alarm Tone number
  delay (50);
//  EEPROM.write (sFXLoc,1); // Write Menu SFX on/off
//  delay (50);
  EEPROM.write (tempUnitLoc,true);// Write Temperature units
  delay (50);
 // EEPROM.write (sndVolLoc,0);// Write Sound Volume
 // delay (50);
  EEPROM.write(infoFreqLoc,2); // Write infodisplay freq options
  delay (50);
  EEPROM.write(sayOptionsLoc,255); // Write Say options
  delay (50);
  EEPROM.write(doStartupLoc,true); // Write Startup option
  delay (50);
  EEPROM.write(tmpOffsetLoc,2); // Write Temperature Offset
  delay (50);
  EEPROM.write(infoOptionsLoc,2); // Write InfoDisplay options
  delay (50);
   EEPROM.write(radioOnLoc,false); // Disable RFM by default during INIT
  delay (50);
  EEPROM.write(LEDEnabledLoc,false); // Write LED options
  delay (50);
  EEPROM.write(GPSEnabledLoc,GPSEnabled); // Write LED options
  delay (50);
  EEPROM.write(RF_FreqLoc,0); // Write Frequency
  delay (50);
  EEPROM.write(RF_NodeLoc,NODEID); // Write 
  delay (50);
  EEPROM.write(RF_GatewayLoc,GATEWAYID); // Write 
  delay (50);
  EEPROM.write(RF_NetworkLoc,NETWORKID); // Write 
  delay (50);
  EEPROM.write(RF_SensorLoc,SENSORID); // Write 
  delay (50);
  EEPROM.write(RF_EncryptLoc,false); // Write
  delay (25);
  EEPROM.write(IROnLoc,false); // Disable IR by default during INIT
  delay (25);
  // Write TimeZones
  usEastern.writeRules(ESTLoc);
  usCentral.writeRules(CSTLoc);
  usMountain.writeRules(MSTLoc);
  usArizona.writeRules(AZSTLoc);
  usPacific.writeRules(PSTLoc);
  usAlaska.writeRules(AKSTLoc);
  usHawaii.writeRules(HSTLoc);
  Universal.writeRules(UTCLoc);
}

/*
// =======================================================================================
// ---- Info display ----
// By: LensDigital
// ---------------------------------------------------------------------------------------
void infoDisplay() {
  if (infoFreq == 0) return; // User choose to never show this screen
  if (! ((infoOptions & 128) || (infoOptions & 64) || (infoOptions & 32) || (infoOptions & 16) || (infoOptions & 8) ) ) return; // All display options disabled
  if (soundAlarm[0] || soundAlarm[1]) return; // Do not show if Alarm is playing
  if (isInMenu) return; // Do not show when in menu
  if (isInQMenu) return; // Do not show when in quick menu
  // Check if it's time to Show Info display 
  if ( (minute()%10)%infoFreq == 0 && ( second()==5) )  { // Show date and temp every XX minutes and 5 seconds
    char myString[44];
    //if ( (minute()%10)==0) return; // Prevents showing during 0 digit time
    cls();
    //showSmTime(0,clockColor); // Show Time on top
    if (infoOptions & 128) // Option Enabled?
      //if (!showDate(clockColor)) return; // Scroll Date. Exit if scroll was interrupted by button press
    if (infoOptions & 64) // Option Enabled?
     // if (!showTemp(clockColor,false,true)) return; // Scroll Inside Temp. Exit if scroll was interrupted by button press
    cls(); 
    if (isRadioPresent) 
      if (infoOptions & 32) {// Option Enabled?
        //if (!showTemp(clockColor,false,false)) return; // Scroll Outside Temp. Exit if scroll was interrupted by button press
        cls();
      }
    if (infoOptions & 16) {// Option Enabled?
      if (!alarmInfo(0)) return; //Show info for alarm 1. Exit if scroll was interrupted by button press
      cls ();
      if (!alarmInfo(1)) return; //Show info for alarm 2. Exit if scroll was interrupted by button press
      cls();
    }
    if (isRadioPresent) 
      if (infoOptions & 8) {// Option Enabled?
        if ( last_RF > 0 && ( (millis()-last_RF) < 1800000) )  { // Show sensor timestamp
          //Serial.println ("Sensor data recieved recently");
          //showSmTime(0,clockColor); // Show Time on top
          snprintf(myString,sizeof(myString), "Sensor data recieved %2d seconds ago",second(last_RF));
         // if (! scrolltextsizexcolor(8,myString,clockColor,5) ) return;
          cls ();
        }
        else { // Sensor data hasn't been reiceved in a while
          //showSmTime(0,clockColor); // Show Time on top
          snprintf(myString,sizeof(myString), "Sensor data not recieved in over 30 minutes",second(last_RF));
          //if (! scrolltextsizexcolor(8,myString,RED,5) ) return;
        }
        cls ();
      }
  }
}


// =======================================================================================
// ---- shows alarm information For Info display ----
// Returns False if scrolling function was interrupted by buttons
// By: LensDigital
// ---------------------------------------------------------------------------------------
boolean alarmInfo(byte alrmNum){
  if ( ! (alarmon[alrmNum] & 128) ) return true; //  Alarm is off
     char myString[54]; // String to keep Alarm msg
     char wkdays[28]; //Days of week
     showSmTime(0,clockColor); // Show time on top
     if ( snoozeTime[alrmNum]!=10 ) { // Are we snoozing?
       snprintf(myString,sizeof(myString), "Alarm%d on! Snoozing...Z Z z z z z...",alrmNum+1);
       return scrolltextsizexcolor(8,myString,RED,5);
     }
     else { // Alarm is on but not snoozing
       if (alarmon[alrmNum] == DAILY) {  // Daily Alarm
         if(time12hr) { // It's 12 hour mode
           if (hoursMode(alrmHH[alrmNum]) )  // Check if it's AM
             snprintf(myString,sizeof(myString), "Alarm %d is set Daily at %d:%02d AM",alrmNum+1,myhours,alrmMM[alrmNum]); 
           else // It's PM
             snprintf(myString,sizeof(myString), "Alarm %d is set Daily at %d:%02d PM",alrmNum+1,myhours,alrmMM[alrmNum]); 
         }
         else // It's 24 hour mode
           snprintf(myString,sizeof(myString), "Alarm %d is set Daily at %02d:%02d",alrmNum+1,alrmHH[alrmNum],alrmMM[alrmNum]); 
       }
       else if (alarmon[alrmNum] == WEEKDAY) {  // It's WeekDay alarm
         if(time12hr) { // It's 12 hour mode
           myhours = alrmHH[alrmNum];
           if (myhours == 0) 
             myhours=12; // It's Midnight
           else if (myhours >12) myhours=myhours-12;
           if (alrmHH[alrmNum]<12) // Show AM
             snprintf(myString,sizeof(myString), "Alarm %d is set M-F at %d:%02d AM",alrmNum+1,myhours,alrmMM[alrmNum]); 
           else 
             snprintf(myString,sizeof(myString), "Alarm %d is set M-F at %d:%02d PM",alrmNum+1,myhours,alrmMM[alrmNum]); 
         }
         else // It's 24 hour mode
         snprintf(myString,sizeof(myString), "Alarm %d is set M-F at %02d:%02d",alrmNum+1,alrmHH[alrmNum],alrmMM[alrmNum]); 
       }
       else {  // It's Custom alarm
          // Make custom string containing each weekday that alarm is set too
         strcpy (wkdays,"");
         if (alarmon[alrmNum] & MON)  strcat (wkdays, "Mon ");
         if (alarmon[alrmNum] & TUE)  strcat (wkdays, "Tue ");
         if (alarmon[alrmNum] & WED)  strcat (wkdays, "Wed ");
         if (alarmon[alrmNum] & THU)  strcat (wkdays, "Thu ");
         if (alarmon[alrmNum] & FRI)  strcat (wkdays, "Fri ");
         if (alarmon[alrmNum] & SAT)  strcat (wkdays, "Sat ");
         if (alarmon[alrmNum] & SUN)  strcat (wkdays, "Sun ");
           
          if(time12hr) { // It's 12 hour mode
           myhours = alrmHH[alrmNum];
           if (myhours == 0) 
             myhours=12; // It's Midnight
           else if (myhours >12) myhours=myhours-12;
           if (alrmHH[alrmNum]<12) // Show AM
             snprintf(myString,sizeof(myString), "Alarm%d is set to %d:%02d AM %s",alrmNum+1,myhours,alrmMM[alrmNum],wkdays); 
           else 
             snprintf(myString,sizeof(myString), "Alarm%d is set to %d:%02d PM %s",alrmNum+1,myhours,alrmMM[alrmNum],wkdays); 
         }
         else // It's 24 hour mode
         snprintf(myString,sizeof(myString), "Alarm%d is set to %02d:%02d %s",alrmNum+1,alrmHH[alrmNum],alrmMM[alrmNum],wkdays); 
       }
     return scrolltextsizexcolor(8,myString,clockColor,5);
     }
     
}
*/

// =======================================================================================
// ---- Process Quick display function (to show Date/Temperature/Clock, etc.) ----
// ---- by LensDigital
// =======================================================================================
void quickDisplay()
{

  
  // Read button, but only act after button is RELEASED! This will detect "HELD" or "PRESSED" state
  if (currStatusInc == HIGH) { //Button Was pressed
  // debouncing;
    if ((millis() - lastButtonTime) < BOUNCE_TIME_QUICK) return; //Debounce
    buttonReleased=false;
    buttonPressedInc=true;
    last_ms=millis(); // Set Held Timer
  }
  else { // Button was released (LOW)
    if ((millis() - lastButtonTime) < BOUNCE_TIME_QUICK) return; //Debounce
     if (buttonPressedInc) { // Checks if buttone was previously HIGH. This Eliminates bug with HOLD state
       buttonReleased=true;  
       buttonPressedInc=false; // Resets button pressed state
     }
  }
  
  if (soundAlarm[0]) interruptAlrm[0]=true; // If pressed Stops Alarm
  if (soundAlarm[1]) interruptAlrm[1]=true; // If pressed Stops Alarm
  isInQMenu=true;
  lastButtonTime = millis();
  if (buttonReleased) 
      if ( (millis() - last_ms ) > heldTime) {
        buttonReleased=false;
        talkingMenu(true); 
      }
      else {
        buttonReleased=false;
        talkingMenu(false);
      }
  
  
}

// ====================================================================================================
// ---- Talking menu (show and anounse Date/Temperature/Clock, etc.) used by quickDisplay function ----
// ---- Pass TRUE to go thru all items (i.e. button was held), or FALSE to go thru each item with press of a button
// ---- by LensDigital
// ====================================================================================================
void talkingMenu (boolean mmode) {
  isSettingAlarm=false;
  if (mbutState > 5) mbutState=1; // Go back to beginning of the menu
  if (!mmode) { //Single item talk mode
    sayItem();
    mbutState++;
  }
  else { // Multiple items talk mode (say all of them)
    mbutState=1;
    while (mbutState < 6) { // Go thru all 6 items
      sayItem();
     mbutState++;
    } // End While
    lastButtonTime = 0;// Exit QMenu
  } // End Else 
  
  //if (mmode) lastButtonTime = 0;// Exit QMenu
}

// ====================================================================================================
// ---- Talk function ----
// ---- Will announce time/date/temperature, etc. if these options enabled
// ---- by LensDigital
// ====================================================================================================
void sayItem () {
  talkingLogic ();
  switch (mbutState) {
   case 1: // Show/Say Time
      cls();
      okClock=true; 
      isSettingDate=false;
      showBigTime(clockColor);
//      sayTime();
      break; 
  case 2: // Show/Say Date
      isSettingDate = true;
      okClock=false;
      cls();
      mainDate(clockColor); // Show full screen date
//      sayDate();
  break; 
  case 3: // Say and show INdoor temperature
    
      isSettingDate = false;
      okClock=false;
      /*cls();
      //showSmTime(0,ORANGE);
      showTemp(ORANGE,true, true); // Scroll temperature on the bottom
      */
  break; 
  case 4: // Say and show Outdoor temperature
      isSettingDate = false;
      okClock=false;
      /*
      cls();
      showSmTime(0,ORANGE);
      showTemp(ORANGE,true, false); // Scroll temperature on the bottom
      */
  break; 
  case 5: // Say Alarm
      if (! (alarmon[0] & 128) && ! (alarmon[1] & 128) ) { lastButtonTime = 0; break; }//Both Alarms are off
      if ( alarmon[0] & 128) { // Alarm 1 is On
        isSettingDate = false;
        okClock=false;
        isSettingAlarm=true;
        menuItem=1;
        subMenu[0]=4; // Enable display of Alarm 1
        /*
        cls();
        showAlarm(clockColor);
        sayAlarm(0);
        */
        
      }
      if ( alarmon[1] & 128) { // Alarm 2 is on
        menuItem=2;
        subMenu[1]=4; // Enable display of Alarm 2
        /*
        showAlarm(clockColor);
        sayAlarm(1);
       */
      }
   break; 
   default: // failsafe
   lastButtonTime = 0;// Exit QMenu
   // mbutState=1;
   break;
  } 
}

// Decide which itema needs to be skipped in talking menu
void talkingLogic () {
  
    switch (mbutState){ // Skip item if it's disabled in EEProm
     case 1: 
     if (!(sayOptions & 64)) {
       mbutState++;
       if (!(sayOptions & 32)) {
         mbutState++; 
         if (!(sayOptions & 16)) { 
           mbutState++;
           if (!(sayOptions & 4)) {
             mbutState++;
             if (!(sayOptions & 8)) {
               mbutState++;
               return;
             }
           }
         }
       }
     }
         
     break;
     case 2:
     if (!(sayOptions & 32)) {
       mbutState++; 
       if (!(sayOptions & 16)) { 
           mbutState++;
           if (!(sayOptions & 4)) {
             mbutState++;
             if (!(sayOptions & 8)) {
               mbutState++;
             }
           }
         }
     }
     break;
     case 3:
     if (!(sayOptions & 16)) {
       mbutState++; 
       if (!(sayOptions & 4)) {
             mbutState++;
             if (!(sayOptions & 8)) {
               mbutState++;
             }
           }
     }
     break;
     case 4:
     if (!(sayOptions & 4)) {
       mbutState++;
       if (!(sayOptions & 8)) {
               mbutState++;
             }
     }
     break;
     case 5:
     if (!(sayOptions & 8)) mbutState++;
     break;
    }
    
}




// =======================================================================================
// ---- Lights up LED indicator with specified color  ----
// By: LensDigital
// =======================================================================================
void indicatorLED (byte color) {
  if (!LEDEnabled) return;
  switch (color) {
   case BLACK_LED:
     digitalWrite (redLED, LOW);
     digitalWrite (greenLED, LOW);
     digitalWrite (blueLED, LOW);
   break;
   case RED_LED:
     digitalWrite (redLED, HIGH);
     digitalWrite (greenLED, LOW);
     digitalWrite (blueLED, LOW);
   break;
   case GREEN_LED:
     digitalWrite (redLED, LOW);
     digitalWrite (greenLED, HIGH);
     digitalWrite (blueLED, LOW);
   break;
   case BLUE_LED:
     digitalWrite (redLED, LOW);
     digitalWrite (greenLED, LOW);
     digitalWrite (blueLED, HIGH);
   break;
   case MAG_LED:
     digitalWrite (redLED, HIGH);
     digitalWrite (greenLED, LOW);
     digitalWrite (blueLED, HIGH);
   break;
   case CYA_LED:
     digitalWrite (redLED, LOW);
     digitalWrite (greenLED, HIGH);
     digitalWrite (blueLED, HIGH);
   break;  
   case YELLOW_LED:
     digitalWrite (redLED, HIGH);
     digitalWrite (greenLED, HIGH);
     digitalWrite (blueLED, LOW);
   case WHITE_LED:
     digitalWrite (redLED, HIGH);
     digitalWrite (greenLED, HIGH);
     digitalWrite (blueLED, HIGH);
   break;
  }
  
}
/*
void testGPS() {
  char myString[20];
  int Year;
  byte Month, Day, Hour, Minute, Second;
  while (SerialGPS.available()) {
    if (gps.encode(SerialGPS.read())) { // process gps messages
      // when TinyGPS reports new data...
      unsigned long age;
    //  int Year;
     // byte Month, Day, Hour, Minute, Second;
      gps.crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, NULL, &age);
    }
  }
  snprintf(myString, sizeof(myString),"Year:%d",Year); 
  showText(0,0,myString,1,GREEN);
}
*/
// =======================================================================================
// ---- Sets time by GPS  ----
// =======================================================================================
void getGPSTIme() {
  if (!GPSEnabled) return;
  if (!setGPSTime)
  if ( (hour(LOCAL_TZ) == 23) && ( second()==25) ) setGPSTime=true;// Get time at 11 PM
  if (!setGPSTime) return;    
  for (int i=0;i<1;i++){
   // Serial.println ("Getting GPS Time");
    while (Serial.available()) {
      if (gps.encode(Serial.read())) { // process gps messages
        // when TinyGPS reports new data...
        unsigned long age;
        int Year;
        byte Month, Day, Hour, Minute, Second;
        gps.crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, NULL, &age);
        //gps.get_position(&lat, &lon, &age);
       // Serial.print ("RAW lattitude: "); Serial.println (lat);
        //printCoordinates();
       // Serial.print ("lattitude: "); printCoordinates(lat,true);
       // Serial.print ("RAW Longitude: "); Serial.println (lon);
       // Serial.print ("longitude: "); printCoordinates(lon,false);

        if (age < 500) {
          // set the Time to the latest GPS reading
          if (Year<2016) return; // Failsafe for bad data
          setTime(Hour, Minute, Second, Day, Month, Year);
          //adjustTime(offset * SECS_PER_HOUR);
           RTC.set(now()); // Writes time change to RTC chip    
          Serial.println ("Adjusting Time");
          //timestamp[0]=hour();
          timestamp[0]=hour(LOCAL_TZ);
          timestamp[1]=minute();
          timestamp[2]=second();
          if (infoOptions & IO_GPSStat) snprintf(msgbuffer,sizeof(msgbuffer),"GPS time updated:%02d:%02d:%02d",timestamp[0],minute(),second());
          Serial.println (msgbuffer);
          if (i=1) setGPSTime=false; // Reset GPS var
          lastGPSEvent=millis();
        }
      }
    }
    if (timeStatus()!= timeNotSet) {
      if (now() != prevDisplay) { //update the display only if the time has changed
        prevDisplay = now();
        //digitalClockDisplay();  
      }
    }
  } // End of for
  
}

void printCoordinates (long lat) {
  long coord=lat;
  char dr; // East or West, North or Sounth
  int lat_deg=lat/100000;
  int var1=coord % 100000; // last 5 digit
  int lat_hr=var1/1000;
  int var2=var1 % 1000 ; // last 3 digits
  int lat_min=var2/10;
  int lat_min2=var2%10;
  
  if (lat<0) dr='S'; else dr='N';
 // if (lon<0) dr[0]='W'; else dr[0]='E';
  if (coord<0) coord=abs(coord);
 // Serial.print (coord / 100000); Serial.print(dr);Serial.print (" "); // Degree
 // Serial.print ( (var1 / 1000) ); Serial.print ("' "); // Hours
 // Serial.print ( var2 / 10 ); //Minutes
  //    Serial.print(".");
   //     Serial.print ( var2 % 10 ); // Decimal
   //       Serial.println ("\"");
//   snprintf(msgbuffer,sizeof(msgbuffer),"Lattitude:%dN %d %d.%d",coord / 100000,var1 / 1000,var2 / 10,var2 % 10);
  snprintf(msgbuffer,sizeof(msgbuffer),"Lat:%2d%c %2d' %2d.%d",lat_deg,78,lat_hr,lat_min,lat_min2);
   //for (int i=0;i<sizeof(msgbuffer);i++) Serial.print (msgbuffer[i]);
   Serial.println (msgbuffer);
}

// ===============================================================================================
// --- Update LED Indicators (RF data recieved, GPS, etc.)
// ===============================================================================================
void indicatorStatus () {
  if (!isRadioPresent) return; // Quit if RFM disabled
  char indColorRF, indColorGPS;
  if (lastRFEvent==0) indColorRF=BLACK;
  else if ((unsigned long)millis()-lastRFEvent < RFTimeout*60000 ) indColorRF=GREEN;
  else indColorRF=BLACK;
   plot (47,0,indColorRF);
  if (!GPSEnabled) return;
  if (lastGPSEvent==0) indColorGPS=BLACK;
  else if ((unsigned long)millis()-lastGPSEvent < GPSTimeout*60000 ) indColorGPS=GREEN;
  else indColorGPS=BLACK;
  plot (0,15,indColorGPS);
}

// ===============================================================================================
// --- Write a 2 byte integer to the eeprom at the specified address and address + 1
// ===============================================================================================
void EEPROMWriteInt(int p_address, int p_value)
     {
     byte lowByte = ((p_value >> 0) & 0xFF);
     byte highByte = ((p_value >> 8) & 0xFF);

     EEPROM.write(p_address, lowByte);
     EEPROM.write(p_address + 1, highByte);
     }

// ==================================================================================================
// --- Read a 2 byte integer from the eeprom at the specified address and address + 1
// ==================================================================================================
unsigned int EEPROMReadInt(int p_address)
     {
     byte lowByte = EEPROM.read(p_address);
     byte highByte = EEPROM.read(p_address + 1);

     return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
     }


// ==================================================================================================
// --- Reboots processor via Watchdog
// ==================================================================================================
void softReboot() {
  wdt_enable(WDTO_15MS);
  while(1)
  {
  }
}

