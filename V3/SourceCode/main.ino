// =======================================================================================
// ---- First time clock is powered, play startup music, show vers and say time ----
// ---------------------------------------------------------------------------------------
void startup ()
{
  if (!doStartup) return; // Startup Disabled?
  //void initEPROM();
  char welcome[15];
  //byte ver=EEPROM.read (clockVerLoc); // Read 3 digit version number
  int ver=EEPROMReadInt(clockVerLoc);
  int temp = (ver%100); //temp holder
  int ver3 = temp % 10; // Last digit
  int ver2 = (temp - ver3) / 10; // Second Digit
  ver = (ver - ver2) / 100; // First digitf
  playfile("startup1.wav");
  showText(0,0,"Welcome",3,GREEN);
  snprintf(welcome, sizeof(welcome),"Firmware:V%d.%d%d",ver,ver2,ver3); 
  scrolltextsizexcolor(8,welcome,RED,15);
  cls();
  showBigTime(clockColor);
}

// =======================================================================================
// ---- Initializes EEPROM for new Chip. run only first time ----
// ---------------------------------------------------------------------------------------
void initEPROM()
{
  putstring_nl ("Writing Initial settings to EEPROM!!!");
 for (byte i=0;i<2;i++) {
    EEPROM.write (alarmHHLoc[i],0);  
    delay (50);
    EEPROM.write (alarmMMLoc[i],0);
    delay (50);
    EEPROM.write (alarmOnOffLoc[i],0);
    delay (50);
    EEPROM.write(alarmToneLoc[i],1); // Write Alarm Tone number
    delay (50);
 }
  EEPROM.write (mode24HRLoc,false); // Write time mode (12/24 hours)
  delay (50);
  EEPROM.write (brightLoc,3); // Write Brightness setting
  delay (50);
  EEPROM.write (clockColorLoc,GREEN); // Write Clock Color
  delay (50);
  //EEPROM.write (clockVerLoc,firmware_ver); 
  EEPROMWriteInt(clockVerLoc,firmware_ver);
  delay (50);
  EEPROM.write(clockFontLoc,1); // Write Alarm Tone number
  delay (50);
  EEPROM.write (sFXLoc,1); // Write Menu SFX on/off
  delay (50);
  EEPROM.write (tempUnitLoc,true);// Write Temperature units
  delay (50);
  EEPROM.write (sndVolLoc,0);// Write Sound Volume
  delay (50);
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
  EEPROM.write(IROnLoc,false); // Disable IR by default during INIT
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
  EEPROM.write(RF_EncryptLoc,true); // Write
  delay (25);
  EEPROM.write(currTZLoc,0); // Write Current Timezone index
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

// Display divider colon :
void showDivider(byte color){
  int x=0; //offset
  if(time12hr) x=2; // Offset divider by 2 dots left for 12 hr mode
  if (clockFont<3) plot (15-x,5,color);
  plot (16-x,5,color);
  if (clockFont<3) plot (15-x,6,color);
  plot (16-x,6,color);
  if (clockFont<3) plot (15-x,9,color);
  plot (16-x,9,color);
  if (clockFont<3) plot (15-x,10,color);
  plot (16-x,10,color);
}

// =======================================================================================
// ---- Info display ----
// By: LensDigital
// ---------------------------------------------------------------------------------------
void infoDisplay() {
  if (isInMenu) return; // Do not show when in menu
  if (isInQMenu) return; // Do not show when in quick menu
  if (infoFreq == 0) return; // User choose to never show this screen
  if (! ((infoOptions & IO_Date) || (infoOptions & IO_InTemp) || (infoOptions & IO_OutTemp) || (infoOptions & IO_Alarms) || (infoOptions & IO_RFStat) || (infoOptions & IO_OutHum)) ) return; // All display options disabled
  if (soundAlarm[0] || soundAlarm[1]) return; // Do not show if Alarm is playing
  
  // Check if it's time to Show Info display 
  if ( (minute()%10)%infoFreq == 0 && ( second()==5) )  { // Show date and temp every XX minutes and 5 seconds
    char myString[44];
    //if ( (minute()%10)==0) return; // Prevents showing during 0 digit time
    cls();
    showSmTime(0,clockColor); // Show Time on top
    if (infoOptions & IO_Date) // Option Enabled?
      if (!showDate(clockColor)) return; // Scroll Date. Exit if scroll was interrupted by button press
    if (infoOptions & IO_InTemp) // Option Enabled?
      if (!showTemp(clockColor,false,true)) return; // Scroll Inside Temp. Exit if scroll was interrupted by button press
    #if not defined XRONOS2
      if (infoOptions & IO_InHum) // Option Enabled?
        if (!showHumidity(clockColor,0,false)) return; // Scroll Inside Temp. Exit if scroll was interrupted by button press
    #endif
    cls(); 
    if (isRadioPresent) {
        if (infoOptions & IO_OutTemp) {// Option Enabled?
          if (!showTemp(clockColor,false,false)) return; // Scroll Outside Temp. Exit if scroll was interrupted by button press
         cls();
        }
        if (infoOptions & IO_OutHum) {// Humidity Option Enabled?
        if (!showHumidity(clockColor,1,false)) return; //Scroll Outside Humidity. Exit if scroll was interrupted by button press
        cls ();
        }
    }
    if (infoOptions & IO_Alarms) {// Option Enabled?
      if (!alarmInfo(0)) return; //Show info for alarm 1. Exit if scroll was interrupted by button press
      cls ();
      if (!alarmInfo(1)) return; //Show info for alarm 2. Exit if scroll was interrupted by button press
      cls();
    }
    if (isRadioPresent) 
      if (infoOptions & IO_RFStat) {// Option Enabled?
        showSensorData();
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
  if ( ! (alarmon[alrmNum] & ALRM_TGL) ) return true; //  Alarm is off
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


// =======================================================================================
// ---- Says current time ----
// By: LensDigital
// ---------------------------------------------------------------------------------------
void sayTime(){
     playcomplete("TIME_IS.WAV");
     char myString[9];
     myhours=hour(LOCAL_TZ); // Converter from UTC to local
     if(time12hr) { // == 12 Hour Mode ====
       snprintf(myString,sizeof(myString), "%d.WAV",hourFormat12(LOCAL_TZ ) ); // Make Hours string
       playcomplete(myString); // Play Hours
     }
     else // == 24 Hour Mode ====
       if (myhours > 20) { // Make complex 2 digit sound
         playcomplete("20.WAV"); // Play Hours 1st digit
         snprintf(myString,sizeof(myString), "%d.WAV",myhours%10); // Make Hours 2nd digit string
         playcomplete(myString); // Play Hours 2nd digit
       }
       else { // Simple
         snprintf(myString,sizeof(myString), "%d.WAV",myhours); // Make Hours string
         playcomplete(myString); // Play Hours
       }
     if (minute()<20) {
       if (minute()==0) { // We are at hh:00
         if (!time12hr)  playcomplete("100.WAV");  // only say "hundred" if in 24 hour mode (wbp)
       } 
       else {
         if (( (minute()/10)%10) == 0) playcomplete ("OH.WAV"); // If first digit of minute is 0, say "oh"
         snprintf(myString,sizeof(myString), "%d.WAV",minute()); // Make Minutes string
         playcomplete(myString);
         }
       }
     else { // Make complex 2 digit sound
       minutes=(minute()/10)%10;
       switch (minutes){ // Create 1st digit sound
         case 2:
           playcomplete ("20.WAV");
           break;
         case 3:
           playcomplete ("30.WAV");
           break;
         case 4:
           playcomplete ("40.WAV");
           break;
         case 5:
           playcomplete ("50.WAV");
           break;
       }
       if ((minute()%10)!=0) { // Don't say if last digit is 0
         snprintf(myString,sizeof(myString), "%d.WAV",minute()%10); // Make 2nd digit
         playcomplete(myString); // Play 
       }
     }
     // If needed say AM or PM
     if(time12hr) { // == 12 Hour Mode ====
       if (isAM(LOCAL_TZ))  playcomplete("AM.WAV");
       else playcomplete("PM.WAV");
     }
   
}

// =======================================================================================
// ---- Says current time ----
// By: LensDigital
// ---------------------------------------------------------------------------------------
void sayAlarm(byte alrmNum){
  char myString[9];  
   playcomplete("ALARM.WAV");
  snprintf(myString,sizeof(myString), "%d.WAV",alrmNum+1); // Make and say alarm number
  playcomplete(myString);
  playcomplete("IS.WAV");
  playcomplete("SET.WAV");
  playcomplete("2.WAV");
  
  if(time12hr) { // It's 12 hour mode
           myhours = alrmHH[alrmNum];
           if (myhours == 0) 
             myhours=12; // It's Midnight
           else if (myhours >12) myhours=myhours-12;
           snprintf(myString,sizeof(myString), "%d.WAV",myhours); // Make hours filename
           playcomplete(myString); // Play Hours
   }
     else // == 24 Hour Mode ====
       if (alrmHH[alrmNum] > 20) { // Make complex 2 digit sound
         playcomplete("20.WAV"); // Play Hours 1st digit
         snprintf(myString,sizeof(myString), "%d.WAV",alrmHH[alrmNum]%10); // Make Hours 2nd digit string
         playcomplete(myString); // Play Hours 2nd digit
       }
       else { // Simple
         snprintf(myString,sizeof(myString), "%d.WAV",alrmHH[alrmNum]); // Make Hours string
         playcomplete(myString); // Play Hours
       }
     if (alrmMM[alrmNum]<20) {
       if (alrmMM[alrmNum]==0) { playcomplete("100.WAV"); } // We are at hh:00
       else {
         if (( (alrmMM[alrmNum]/10)%10) == 0) playcomplete ("OH.WAV"); // If first digit of minute is 0, say "oh"
         snprintf(myString,sizeof(myString), "%d.WAV",alrmMM[alrmNum]); // Make Minutes string
         playcomplete(myString);
         }
       }
     else { // Make complex 2 digit sound
       minutes=(alrmMM[alrmNum]/10)%10;
       switch (minutes){ // Create 1st digit sound
         case 2:
           playcomplete ("20.WAV");
           break;
         case 3:
           playcomplete ("30.WAV");
           break;
         case 4:
           playcomplete ("40.WAV");
           break;
         case 5:
           playcomplete ("50.WAV");
           break;
       }
       if ((alrmMM[alrmNum]%10)!=0) { // Don't say if last digit is 0
         snprintf(myString,sizeof(myString), "%d.WAV",alrmMM[alrmNum]%10); // Make 2nd digit
         playcomplete(myString); // Play 
       }
     }
     // If needed say AM or PM
     if(time12hr) { // == 12 Hour Mode ====
       if (alrmHH[alrmNum]<12)  playcomplete("AM.WAV");
       else playcomplete("PM.WAV");
     }
     if (alarmon[alrmNum] == DAILY) playcomplete("DAILY.WAV");// Daily Alarm
     else if (alarmon[alrmNum] == WEEKDAY) playcomplete("wkdays.WAV");// Workday Alarm
     else { // Custom ALarm
       playcomplete("EVERY.WAV");
       if (alarmon[alrmNum] & MON) playcomplete("MON.WAV");
       if (alarmon[alrmNum] & TUE) playcomplete("TUE.WAV");
       if (alarmon[alrmNum] & WED) playcomplete("WED.WAV");
       if (alarmon[alrmNum] & THU) playcomplete("THU.WAV");
       if (alarmon[alrmNum] & FRI) playcomplete("FRI.WAV");
       if (alarmon[alrmNum] & SAT) playcomplete("SAT.WAV");
       if (alarmon[alrmNum] & SUN) playcomplete("SUN.WAV");
     }
     
}

// =======================================================================================
// ---- Announce Internal Temperature ----
// By: LensDigital
// ---------------------------------------------------------------------------------------
void sayTemp(int temp, boolean location){
  /*
  if (temp > 100 || temp < -50) {
    playcomplete ("ERR1.WAV");
    return;
  } */
  int unsignedTemp=temp; // Will remove negative sign for vocal processing.
  char myString[8];
  playcomplete("TEMP.WAV");
  if (location) playcomplete("INSIDE.WAV");
  else playcomplete("OUTSIDE.WAV");
  playcomplete("IS.WAV");
 
  if (temp<0) { // Handle Negative temperatures
    playcomplete("MINUS.WAV");
    unsignedTemp=unsignedTemp*-1;// Converts negative to positive
  }
  if (temp<20) {
    snprintf(myString,sizeof(myString), "%d.WAV",unsignedTemp); // Teen and single digits
    playcomplete(myString);
  }
  else { // It's over 20, so make 2 digit phrase
    snprintf(myString,sizeof(myString), "%d0.WAV",(unsignedTemp/10)%10); // first digit
    playcomplete(myString);
    if ((temp%10)!=0) {
      snprintf(myString,sizeof(myString), "%d.WAV",unsignedTemp%10); // first digit
      playcomplete(myString);
    }
  }
  playcomplete("DEGREES.WAV");
  if (tempUnit) playcomplete("fahrenh.WAV");
  else playcomplete("celcius.WAV");
}


// =======================================================================================
// ---- Announce Current Date ----
// By: LensDigital
// ---------------------------------------------------------------------------------------
void sayDate(){
  char myString[10];
  char dayofweek[4]; // Keeps day of week
  playcomplete("TODAY.WAV");
  playcomplete("IS.WAV");
  Serial.println ("Say Date!");
  switch (weekday(LOCAL_TZ)){
    case 1: //Sunday
    strcpy(dayofweek,"SUN");
    break;
    case 2: //Mon
    strcpy(dayofweek,"MON");
    break;
    case 3: 
    strcpy(dayofweek,"TUE");
    break;
    case 4: 
    strcpy(dayofweek,"WED");
    break;
    case 5: 
    strcpy(dayofweek,"THU");
    break;
    case 6: 
    strcpy(dayofweek,"FRI");
    break;
    case 7: 
    strcpy(dayofweek,"SAT");
    break;
  }
  snprintf(myString,sizeof(myString), "%s.WAV",dayofweek); // Day of Week
  playcomplete(myString);
  snprintf(myString,sizeof(myString), "%s.WAV",monthShortStr(month(LOCAL_TZ)) ); // month
  playcomplete(myString);
  if (day(LOCAL_TZ)<20) {
      snprintf(myString,sizeof(myString), "%dst.WAV",day(LOCAL_TZ)); // Make Day String
      playcomplete(myString);
         
  }
  else { // Make complex 2 digit sound
       days=(day(LOCAL_TZ)/10)%10;
       switch (days){ // Create 1st digit sound
         case 2:
           playcomplete ("20.WAV");
           break;
         case 3:
           playcomplete ("30.WAV");
           break;
       }
       if ((day(LOCAL_TZ)%10)!=0) { // Don't say if last digit is 0
         snprintf(myString,sizeof(myString),"%dst.WAV",day(LOCAL_TZ)%10); // Make 2nd digit
         playcomplete(myString); // Play 
       }
     }
  playcomplete("2.WAV");
  playcomplete("1000.WAV");
  snprintf(myString,sizeof(myString), "%d.WAV",year(LOCAL_TZ)%100); // Make 2nd digit
  playcomplete(myString); // Play 
}


// =======================================================================================
// Display large digit (full screen) time
// By: LensDigital
// ---------------------------------------------------------------------------------------
void showBigTime(byte color){
  //if (menuItem > 3 || !okClock) return; // Date setting is in progress. Do not show clock
  if (!okClock) return; // Date setting is in progress. Do not show clock
  int blinkDotDuration =500; // How frequently dots should blink
  int blinkDigDuration = 500; // Frequencey of digit blinking during time setting
  int x=0; //offset
  if (color==4)color=random(3)+1; // Select random color
  
  // Blinker processor (used to blink divider and/or digits during time setting
  if (!isInMenu)  // If we are setting time, don't blick divider
    if ( (millis()-blinkTime > blinkDotDuration)) { // It's been over blinkDuration time
      blinkTime = millis(); // reset offset to current time
      if ( blinkColor == BLACK )  {
       
        blinkColor=color; // Inverse color of divider
      }
      else {
        blinkColor = BLACK;
        
      }
     
      //else if (!blinking) blinkColor=BLACK; // If setting time don't blink divider
    }  
  
  if (blinking){ // Setting time, so blink correct digits
    //Serial.println ("Setting time");
    blinkColor=color; // Show steady divider dots
    if ( (millis()-blinkTime > blinkDigDuration)) { // It's been over blinkDuration time
      blinkTime = millis(); // reset offset to current time
      if (isSettingHours) {
        //putstring_nl ("Setting hours");
       if ( hhColor == BLACK ) hhColor = color; // Inverse color of Hours 
       else hhColor = BLACK;
       mmColor = color; // Minutes not blinking
      }
      if (isSettingMinutes) {
        //putstring_nl ("Setting minutes");
       if ( mmColor == BLACK ) mmColor = color; // Inverse color of Minutes 
       else mmColor = BLACK;
       hhColor = color; // Hours not blinking
      }
    }
  }
  else { hhColor=color; mmColor=color;} // We are not setting time, so show digits as usual
  // --- END OF BLINK PROCESSOR
    
  // Check if we are running in 12 Hour Mode:
  if(time12hr) {
  // == BEGIN 12 Hour Mode ====
    x=2; //offset horus by 2 dots
    myhours=hourFormat12(LOCAL_TZ);
    if (isAM(LOCAL_TZ)) plot (0,1,hhColor); // Show AM Dot
    else plot (0,1,BLACK); // Hide AM Dot
    if ( (myhours/10)%10 == 0 ) showDigit(0-x,2,1,5,clockFont,BLACK); // Hide first digit 
    else showDigit(0-x,2,(myhours/10)%10,5,clockFont,hhColor);
  }
  // === END 12 Hour Mode ===
  else {
  // 24 Hour Mode
   //myhours=hour();
   myhours=hour(LOCAL_TZ); // Converter from UTC to local
    //plot (0,1,BLACK); // Hide AM Dot
    showDigit(0,2,(myhours/10)%10,5,clockFont,hhColor); // Show 1st digit of hour
  }
  showDigit(6-x,2,myhours%10,5,clockFont,hhColor); // Always Show 2nd digit of hour
  showDivider (blinkColor);
  showDigit(15,2,(minute()/10)%10,5,clockFont,mmColor); // Show 1st digit of minute
  showDigit(22,2,minute()%10,5,clockFont,mmColor); // Show 2nd digit of minute
  
}


// =======================================================================================
// ---- Display smaller digit time ----
// By: LensDigital
// Location is either 0: top row, or 1: buttom row
// ---------------------------------------------------------------------------------------
void showSmTime (byte location,byte color){
  char myString[6];
  location=location * 8; // Shift to bottom row if 1
  indicatorStatus(); 
  // Check if we are running in 12 Hour Mode:
  if(time12hr) {
  // == BEGIN 12 Hour Mode ====
    myhours=hourFormat12(LOCAL_TZ);
    if (isAM(LOCAL_TZ)) plot (0,1,color); // Show AM Dot
    else plot (0,1,BLACK); // Hide AM Dot
    snprintf(myString,sizeof(myString), "%d",myhours); // Make hour string
    if ( (myhours/10)%10 == 0 )  // It's one digit hour so need to shift it to the right
      showText(7,location,myString,1,color); // Shift hour to the right since it's sigle digit
    else 
      showText(1,location,myString,1,color); // Show hour at normal position
    
  }
  // === END 12 Hour Mode ===
  else {
  // 24 Hour Mode
   snprintf(myString,sizeof(myString), "%02d",myhours); // make 2 digit hours
   showText(1,location,myString,1,color);
    //plot (0,1,BLACK); // Hide AM Dot
  }
  snprintf(myString,sizeof(myString), "%02d",minute());
  showText(12,location,":",1,color); // Show colum :
  showText(18,location,myString,1,color); // Show minutes
  
  
}

// =======================================================================================
// ---- Display Date and year (mostly for setting) ----
// By: LensDigital
// ---------------------------------------------------------------------------------------
void mainDate(byte color){
  if (!isSettingDate) return; // Not setting Date
  int blinkDigDuration = 500; // Frequencey of digit blinking during time setting
  char dateString[5]; // Var will hold generated string
  if (color==4)color=random(3)+1; // Select random color
  // Blinker processor (used to blink text, number during date setting
  if (isSettingTime){
     if ( (millis()-blinkTime > blinkDigDuration)) { // It's been over blinkDuration time
      blinkTime = millis(); // reset offset to current time
      if (blinking){ // Setting date, so blink correct string
         if ( dateColor == BLACK ) dateColor=color;
         else  dateColor=BLACK;
      }
      else dateColor=color;
    }  
    if (isSettingMonth) {
       monColor=dateColor; 
       ddColor=color; 
       yyColor=color;
     }
     if (isSettingDay) {
       monColor=color;
       ddColor=dateColor;
       yyColor=color;
     }
     if (isSettingYear) {
       monColor=color;
       ddColor=color;
       yyColor=dateColor;
     }
  }
  else {
    monColor=color;
    ddColor=color;
    yyColor=color;
  }
  // --- END OF BLINK PROCESSOR
   
   snprintf(dateString, sizeof(dateString),"%s",monthShortStr(month(LOCAL_TZ)) ); // Create Month String
   showText(1,0,dateString,2,monColor); // Show month
   snprintf(dateString, sizeof(dateString),"%2d",day(LOCAL_TZ) ); // create Day strng
   showText(20,0,dateString,2,ddColor); // Show day
   snprintf(dateString, sizeof(dateString),"%2d",(year(LOCAL_TZ)) ); // create Year String
   showText(5,8,dateString,1,yyColor); // Show year
}
// ---------------------------------------------------------------------------------------


// =======================================================================================
// ---- Display Current Temperature  ----
// ---- speak = true if voice annoucment desired
// ---- location (0=inside, 1=outside, 2=both)
// By: LensDigital
// =======================================================================================
boolean showTemp(byte color,boolean speak, boolean location){
  char myString[32];
  char tempInOut[7];
  byte tmpOffset2=0; // will differ if getting temp from outside
  if (location)  tmpOffset2=tmpOffset; //Only for Inside temp Actual offset is used
  boolean returnVal=true;// Return value
  float tempC;
  if (location) { // Get temperature from attached sensor
    getIntSensorData();
    tempC = tempCint;
    snprintf(tempInOut,sizeof(tempInOut), "In");
  }
  else { // Get Temperature from external sensor
    if (!isRadioPresent) return true;
    tempC=extTemp; // External Temperature was requested
    snprintf(tempInOut,sizeof(tempInOut), "Out");
  }
  if (tempC > 100 || tempC < -50) { // Temperature sensor is not working
    if (speak) playfile("ERR1.WAV");
    snprintf(myString,sizeof(myString), "%s Temp Sensor ERROR",tempInOut); // Show Error and exit
    showSmTime(0,color); // Show small digit time on top
    return scrolltextsizexcolor(8,myString,RED,5);
 }
  float tempF = (tempC * 1.8) + 32.0; // Convert to Farenheit
  int tempCint = int(tempC +0.5) - tmpOffset2;// Convert round off decimal to whole number.
  int tempFint = int(tempF +0.5) - tmpOffset2; // Convert round off decimal to whole number.
  showSmTime(0,color); // Show small digit time on top
  if(!speak) { //Scroll Temp
  if (tempUnit) snprintf(myString,sizeof(myString), "%s Temp:%dF ",tempInOut,tempFint); // Format String for Farenheight
  else snprintf(myString,sizeof(myString), "%s Temp:%dC ",tempInOut,tempCint); // Format String for Celcius
    returnVal=scrolltextsizexcolor(8,myString,color,20); 
  }
  if (speak) {
    if (tempUnit) snprintf(myString,sizeof(myString), "%dF ",tempFint); // Short Format String for Farenheight
    else  snprintf(myString,sizeof(myString), "%dC ",tempCint); // Short Format String for Farenheight
    //showText(5,8,myString,1,color); // Show Static Temp string
    jumpTextVertical(5,BOTTOM_SCREEN,RAISE_UP,FLY_IN,myString, 1,color, 25);
    if (tempUnit) sayTemp(tempFint,location);
    else sayTemp(tempCint,location);
  }
  return returnVal;
}

// =======================================================================================
// ---- Show Current Date (for quick menu display) ----
// By: LensDigital
// =======================================================================================
boolean showDate(byte color){
  if (isInMenu) return true; // Do not sound alarm if changing settings
  char dateString[14]; // stores formatted date
  //showSmTime(0,color); // Show small digit time on top
  // Format Date and store in dateString array
     snprintf(dateString,sizeof(dateString), "%s %02d, %d ",monthShortStr(month(LOCAL_TZ)),day(LOCAL_TZ),year(LOCAL_TZ));
     return scrolltextsizexcolor(8,dateString,color,25);
}

// ---------------------------------------------------------------------------------------


// ====================================================================================================
// ---- Talking menu (show and anounse Date/Temperature/Clock, etc.) used by quickDisplay function ----
// ---- Pass TRUE to go thru all items (i.e. button was held), or FALSE to go thru each item with press of a button
// ---- by LensDigital
// ====================================================================================================
void talkingMenu (boolean mmode) {
  isSettingAlarm=false;
  if (mbutState > 7) mbutState=1; // Go back to beginning of the menu
  if (!mmode) { //Single item talk mode
    sayItem();
    mbutState++;
  }
  else { // Multiple items talk mode (say all of them)
    mbutState=1;
    while (mbutState < 8) { // Go thru all 7 items
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
   //Serial.print ("DEBUG: mbutState: "); Serial.println (mbutState);
  
switch (mbutState) {
 
   case 1: // Show/Say Time
    if (sayOptions & SO_Time) { // This option is enabled
      cls();
      okClock=true; 
      isSettingDate=false;
      showBigTime(clockColor);
      sayTime();
      break; 
    }
    else  mbutState++;
   case 2: // Show/Say Date
     if (sayOptions & SO_Date) { // This option is enabled
      isSettingDate = true;
      okClock=false;
      cls();
      mainDate(clockColor); // Show full screen date
      sayDate();
      break; 
     }
      else  mbutState++;
   case 3: // Say and show INdoor temperature
     if (sayOptions & SO_InTemp) {  // This option is enabled
      isSettingDate = false;
      okClock=false;
      cls();
      showSmTime(0,ORANGE);
      showTemp(ORANGE,true, true); // Scroll temperature on the bottom
      break; 
     }
     else  mbutState++;
  case 4: // Say and show Outdoor temperature
    if (sayOptions & SO_OutTemp) {  // This option is enabled
      isSettingDate = false;
      okClock=false;
      cls();
      showSmTime(0,ORANGE);
      showTemp(ORANGE,true, false); // Scroll temperature on the bottom
     break; 
    }
     else  mbutState++;
  case 5: // Say and show Indoor humidity
    #ifdef XRONOS2 // Xronos 2 doesn't have internal humiidty sensor
      mbutState++;
    #else
        if (sayOptions & SO_InHum) {  // This option is enabled
          cls();
          showHumidity(ORANGE,0,true);
        break;
        }
    else  mbutState++;
    #endif
  case 6: // Say and show Outdoor humidity
    if (sayOptions & SO_OutHum) {  // This option is enabled
      isSettingDate = false;
      okClock=false;
      cls();
      showHumidity(ORANGE,1,true);
     break;
    }
    else  mbutState++;
  case 7: // Say Alarm
    if (sayOptions & SO_Alarms) {  // This option is enabled
      if (! (alarmon[0] & ALRM_TGL) && ! (alarmon[1] & 128) ) { lastButtonTime = 0; break; }//Both Alarms are off
      if ( alarmon[0] & ALRM_TGL) { // Alarm 1 is On
        isSettingDate = false;
        okClock=false;
        isSettingAlarm=true;
        cls();
        menuItem=1;
        subMenu[0]=4; // Enable display of Alarm 1
        showAlarm(clockColor);
        sayAlarm(0);
        
      }
      if ( alarmon[1] & ALRM_TGL) { // Alarm 2 is on
        menuItem=2;
        subMenu[1]=4; // Enable display of Alarm 2
        showAlarm(clockColor);
        sayAlarm(1);
     
      }
      break; 
    }
   default: // failsafe
   lastButtonTime = 0;// Exit QMenu
   // mbutState=1;
   break;
  } 
}



void startBlinking(){
  blinking = !isIncrementing;
}

void stopBlinking(){
  blinking = false;
}


// =======================================================================================
// ---- Converts 24 hours to 12 mode, returns true if AM ----
// ---- by LensDigital
// =======================================================================================
boolean hoursMode(byte hrs) {
  if (hrs == 0) myhours=12; // It's Midnight
  else if (hrs >12) myhours=hrs-12;
  if (hrs<12)
    return true; // It's AM so return true
 else 
    return false; // It's PM
}

/*
// =======================================================================================
// ---- Checks Ambient Light leve and adjust brightness ----
// ---- by LensDigital & William Phelps
// =======================================================================================
//static unsigned long lastRun = 0;
// =======================================================================================
// ---- calc running average of last n readings
// =======================================================================================
#define LRSIZE 3  // average of 3 readings
static int LR[LRSIZE] = {2,2,2}; // previous values
static byte LRindex = 0;
byte runningAverage(int r)
{
  int sum = 0;
  LR[LRindex] = r; // replace oldest value with new
  if (++LRindex == LRSIZE) // increment index 
    LRindex = 0; // and wrap
  for (byte i = 0; i<LRSIZE; i++) {
    sum += LR[i];
  }
  return sum / LRSIZE; // return average
}
*/

void autoBrightness () {
  if (isInMenu) return;
  if (brightness) return; // Brightness is not set to 0 (auto)
//  if (second()%10) return; // Take readings every 10th second only
  if ((millis()-lastLReading) < L_READING_FREQ) return; 
  lastLReading = millis();
  //Serial.println ("Changing Brightness");
  if (prevBrightness==0) {  // Initialized previous Brightness setting only if Brightness was reset
    //prevBrightness=map(analogRead(photoCellPin), PHOTOCELL_MIN, PHOTOCELL_MAX, 1, 5); // Get Ambient Light Reading
    prevBrightness=map( constrain (analogRead(photoCellPin), PHOTOCELL_MIN, PHOTOCELL_MAX), PHOTOCELL_MIN, PHOTOCELL_MAX, 1, 5); // Get Ambient Light Reading
    setBrightness(prevBrightness); // Set LED brightness
  }
//  lightLevel = map(analogRead(photoCellPin), PHOTOCELL_MIN, PHOTOCELL_MAX, 1, 5); // Get Ambient Light Reading
  lightLevel = map( constrain (analogRead(photoCellPin), PHOTOCELL_MIN, PHOTOCELL_MAX), PHOTOCELL_MIN, PHOTOCELL_MAX, 1, 5); // Get Ambient Light Reading
  //lightLevel = runningAverage(lightLevel); // calc running average 
  if (lightLevel != prevBrightness) { // Set LED brightness only if light changed
    setBrightness(lightLevel);
    prevBrightness=lightLevel;
    //Serial.println (lightLevel);
    //Serial.println (FreeRam());
  }
}
// =======================================================================================
// ---- Display/Say Humidity  ----
//      location (0=internal, 1=external)
// ---- speak = true if voice annoucment desired
// By: LensDigital
// =======================================================================================
boolean showHumidity(byte color, bool location, bool speak) {
  #ifdef XRONOS2
    if (location==0) return; // Xronos 2 doesn't have internal humidity sensor
  #endif
  int humidity;
  if (location==0) humidity=humidInt; // Indoor Humidity requested
  else humidity=extHum; // Outdoor humidity requested
  //if (humidity==0) return false; // Humidity did not update
  char myString[25];
  showSmTime(0,color); // Show small digit time on top
  //if (humidity > 100 || humidity < 1) { // Humidity sensor is not working
  if (humidity==0) { // Humidity sensor is not working
    //showSmTime(0,color); // Show small digit time on top
    if (speak) playfile("ERR1.WAV");
    return scrolltextsizexcolor(8,"Humidity Sensor ERROR",RED,5);
    //jumpTextVertical(5,BOTTOM_SCREEN,RAISE_UP,FLY_IN,"ERR", 1,color, 25);
  }
  if(!speak) { //Scroll
    if (location==0) snprintf(myString,sizeof(myString), "Inside Humidity %2d%%",humidity); // Scroll  Humidity
    else snprintf(myString,sizeof(myString), "Outside Humidity %2d%%",humidity); // Scroll  Humidity
    return scrolltextsizexcolor(8,myString,clockColor,20);
  }
  else {
    snprintf(myString,sizeof(myString), "%d%%",humidity); // Make string for Outside Humidity  
    jumpTextVertical(5,BOTTOM_SCREEN,RAISE_UP,FLY_IN,myString, 1,color, 25);
    sayHumidity(location);
  }
  
}

// =======================================================================================
// ---- Announce External Humidity  ----
// ---- Called from showHumidity 
// By: LensDigital
// =======================================================================================
void sayHumidity(boolean location) {
    #ifdef XRONOS2
      if (location==0) return; // Xronos 2 doesn't have internal humidity sensor
    #endif
    char myString[8];
    int humidity;
    if (location==0) humidity=humidInt;
    else humidity=extHum;
    //Serial.println ("Say Humidity");
    playcomplete("HUMIDITY.WAV");
    if (location==0) playcomplete("INSIDE.WAV");
    else playcomplete("OUTSIDE.WAV");
    playcomplete("IS.WAV");
   if (humidity < 20) { // Say it as is
      snprintf(myString,sizeof(myString), "%d.WAV",humidity); // Scroll Outside Humidity  
      playcomplete (myString);
    }
    else { // Say 20,30, etc
     snprintf(myString,sizeof(myString), "%d0.WAV",(humidity/10)%10); // Scroll Outside Humidity  
     playcomplete (myString);
     if ((humidity%10)!=0) { // Don't say if last digit is 0
           snprintf(myString,sizeof(myString), "%d.WAV",humidity%10); // Make 2nd digit
           playcomplete(myString); // Play   
     } // end If
    } // end Else
    playcomplete ("PERCENT.WAV");
}
// =======================================================================================
// ---- Display remote sensor last time received and battery voltage  ----
// By: LensDigital
// =======================================================================================
void showSensorData() {
  char myString[44];
  byte tColor=clockColor; // Changes text color based on how stale data is (i.e. red if no data recieved in a while)
  if (RFRecieved) { // Show sensor timestamp
          if ((unsigned long)millis()-lastRFEvent < RFTimeout*60000 ) {
              snprintf(myString,sizeof(myString), "Sns data rcv at %02d:%02d",hour(myTZ.toLocal(last_RF, &tcr)),minute(last_RF) );
              tColor=clockColor;
          }
          else {
            snprintf(myString,sizeof(myString), "Sns data rcv %02d/%02d at %02d:%02d",month(myTZ.toLocal(last_RF, &tcr)),day(myTZ.toLocal(last_RF, &tcr)), hour(myTZ.toLocal(last_RF, &tcr)),minute(last_RF) );
            tColor = RED;
            
          }
          // Split batter 3 digit voltage into separate digits
          byte first=sBatt/100; // Thousand
          byte scnd =(sBatt%100)/10;
          byte third=sBatt%10;
          showSmTime(0,clockColor); // Show Time on top
          //snprintf(myString,sizeof(myString), "Sns data rcv at %02d:%02d",hour(myTZ.toLocal(last_RF, &tcr)),minute(last_RF) );
          if (! scrolltextsizexcolor(8,myString,clockColor,5) ) return;
           // Show battery info
          cls();
          showSmTime(0,clockColor); // Show Time on top
          snprintf(myString,sizeof(myString), "Sns batt: %d.%d%dV",first,scnd,third);
          if (! scrolltextsizexcolor(8,myString,clockColor,5) ) return;
          // Show sensor uptime
          /*
          cls();
          showSmTime(0,clockColor); // Show Time on top
          snprintf(myString,sizeof(myString), "Sns uptime: %s",sUptime);
          //Serial.println (myString);
          if (! scrolltextsizexcolor(8,myString,clockColor,5) ) return;
          */
     }
   else { // Sensor data NEVER been recieved yet
         showSmTime(0,clockColor); // Show Time on top
         if (! scrolltextsizexcolor(8,"No sensor data",RED,5) ) return;
        }
}

// ===============================================================================================
// --- Gets internal sensors data (Temperature/Humidity)
// ===============================================================================================
void getIntSensorData() {
  float rhlinear, rhvalue,temperature;
  //Serial.println("Requesting temperature/humidity...");
  #ifdef XRONOS2
    sensors.requestTemperatures(); // Send the command to get temperatures
    temperature=sensors.getTempC(insideThermometer);
  #else
      temperature=TH02.ReadTemperature();
     // Serial.println (temperature);
      rhvalue=TH02.ReadHumidity();
      // Calculate linear compensation
      rhlinear = rhvalue - ((rhvalue*rhvalue) * TH02_A2 + rhvalue * TH02_A1 + TH02_A0);
      rhvalue = rhlinear;
     // Calculate humidty temperature compesation
     rhvalue += (temperature - 30.0) * (rhlinear * TH02_Q1 + TH02_Q0);
      // check if returns are valid, if they are NaN (not a number) then something went wrong!
     if (isnan(temperature) || isnan(rhvalue))  Serial.println("Failed to read from TH02");
     humidInt=int(rhvalue +0.5);
  #endif
 /*
  Serial.print("Temperature is: ");
  Serial.print (temperature); 
  Serial.println("C"); 
  Serial.print("Humidity is: ");
  Serial.print (rhvalue); 
  Serial.println(" %\t"); 
  /*
  Serial.print("Battery is: ");
  Serial.print (getBatVoltage()); 
  Serial.println("V"); 
  */
  tempCint=int(temperature +0.5);
  
}

// ===================================================================
// * TH2/DS18B20 Temperature sensor iniitialization *
// ===================================================================
void TempInit(){
 // ===================================================================
  #if not defined XRONOS2
    TH02.begin(); // Initi TH02 for Xronos 2
  #else // Init DS18B20 for Xronos 2
    //Serial.print("Locating DS18B20 temperature devices...");
    sensors.begin();
    putstring("Found ");
    Serial.print(sensors.getDeviceCount(), DEC);
    putstring_nl(" devices.");
    // report parasite power requirements
    putstring("DS18B20 parasite power is: "); 
    if (sensors.isParasitePowerMode()) putstring_nl("ON");
    else putstring_nl("OFF");
     if (!sensors.getAddress(insideThermometer, 0)) putstring_nl("Unable to find address for Device 0"); 
   // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
    sensors.setResolution(insideThermometer, 9); 
  #endif
}

// ===============================================================================================
// --- Update LED Indicators (RF data recieved, etc.)
// ===============================================================================================
void indicatorStatus () {
  if (!isRadioPresent) return; // Quit if RFM disabled
  char indColor;
  if (lastRFEvent==0) indColor=RED;
  else if ((unsigned long)millis()-lastRFEvent < RFTimeout*60000 ) indColor=GREEN;
  else indColor=RED;
  plot (31,0,indColor);
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
