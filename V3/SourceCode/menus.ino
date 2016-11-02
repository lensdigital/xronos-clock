// =======================================================================================
// Menu Text definitions. In 2 dimental arrays first element 1st row, 2nd is second row
// =======================================================================================
char *MainMenuStr[MAX_MENUS][2]={ {"Set","Alrm1"}, {"Set","Alrm2"}, {"Time/","Date"}, {"Sys","Setup"},{"User","Menu"} };
char *alarmMenuStr[13]={"ERR","Daily","WKDAY","Custm","MON:","TUE:","WED:","THU:","FRI:","SAT:","SUN:","EXIT","TONE"};
char *sysMenuStr[11][2]={ {"Mode:",""},{"Light",""},{"Color",""}, {"Font",""}, {"Menu","sFX"},{"Temp",""},{"Sound","Vol"},{"Startup",""},{"RF","Setup"},{"IR",""}, {"RESET","FLASH"} };
char *sysBrightStr[6]={"Auto","Night","Low","Med","High","Max"};
char *sysColorStr[3]={"Green","Red","Orang"};
char *sysRFMStr[7]={"Turn","FREQ","NODE","NET","SNSR","ENCR","Exit"};
char *optMenuStr[3][2]={ {"Info","Displ"}, {"Talk","Items"}, {"Temp","Offs:-"} };
char *optInfoDispStr[10]={"Scrol", "Every","Date", "InTemp", "ExTemp", "Alarm", "RFStat", "InHum","ExHum","Exit" };
char *optSayStr[9]={"Say", "Time", "Date", "InTemp","ExTemp", "InHum", "ExHum", "Alrm", "Exit" };
char *TZMenuStr[8]={"EST","CST","MNT","AZ","PST","AL","HI","UTC"};

// =======================================================================================
// ---- Show Menu items and set vars ----
// ---- by LensDigital
// =======================================================================================
void showMenu() 
{
 playSFX(2);
  for (int i=0;i<MAX_SUBMENUS;i++) { // Initialize SubMenus
      subMenu[i]=0;
   }
  switch (menuItem) {
    case 1: // Set Alarm 1 Menu Item (Display Only)
          putstring_nl ("Display Alrm 1 Menu");
         okClock=false;
         isSettingSys=false;
         isSettingOptions=false;
         cls();
        //showText(7,0,"Set",1,ORANGE);
        //showText(0,8,"Alrm1",1,ORANGE);
        showText(7,0,MainMenuStr[menuItem-1][0],1,ORANGE);
        showText(0,8,MainMenuStr[menuItem-1][1],1,ORANGE);
        break;
    case 2: // Set Alarm 2 Menu Item (Display Only)
         putstring_nl ("Display Alrm 2 Menu");
         okClock=false;
         isSettingSys=false;
         cls();
        showText(7,0,MainMenuStr[menuItem-1][0],1,ORANGE);
        showText(0,8,MainMenuStr[menuItem-1][1],1,ORANGE);
        break;
    case 3: // Time and Date setting menu
        putstring_nl ("Display Date/Time Menu");
        okClock=false;
        isSettingSys=false;
        cls();
        showText(3,0,MainMenuStr[menuItem-1][0],1,ORANGE);
        showText(3,8,MainMenuStr[menuItem-1][1],1,ORANGE);
        break;
   case 4: // System Settings (preferences/setup)
        putstring_nl ("Display System Menu");
        okClock=false;
        cls();
        showText(7,0,MainMenuStr[menuItem-1][0],1,ORANGE);
        showText(1,8,MainMenuStr[menuItem-1][1],1,ORANGE);
        break;
   case 5: // User Options menu
       putstring_nl ("Display Options Menu");
       okClock=false;
       isSettingSys=false;
       cls();
       showText(3,0,MainMenuStr[menuItem-1][0],1,ORANGE);
       showText(3,8,MainMenuStr[menuItem-1][1],1,ORANGE);
       break;
  } 
}


// =======================================================================================
// ---- Set Alarm ----
// ---- by LensDigital
// =======================================================================================
void setAlarm(byte alrmNum) {
  char myString[11];
  interruptAlrm[alrmNum]=false; //Rearm Button interrupt
  soundAlarm[alrmNum]=false; // Make sure alarm that just sounded doesn't resume
  snoozeTime[alrmNum]=10; // Disable Snooze
  if (alrmToneNum[alrmNum]<6) alrmVol[alrmNum]=7; // Set initial alarm volume (for escalating alarms)
  if (subMenu[alrmNum] >0 && subMenu[alrmNum] <4) playSFX(1); // Don't play sound if not setting anything, i.e. submenu=0 or setting hrs, minutes, tone
  switch (subMenu[alrmNum]) {
    case 1: // Alarm On/off
      alarmon[alrmNum]=alarmon[alrmNum] ^ ALRM_TGL; //Toggle first bit (on/off)
      delay (5);
      EEPROM.write (alarmOnOffLoc[alrmNum],alarmon[alrmNum]);
      cls();
      break;
    case 2: // Set Alarm frequency to off/daily/weekday/custom
          switch (alarmon[alrmNum]){
          case DAILY:
          alarmon[alrmNum]=WEEKDAY; // Was Daily, now Workday
          isSettingAlrmCust[alrmNum] =false;
          break; 
          case WEEKDAY:
          alarmon[alrmNum]=CUSTOM; // Was Workday, now custom
          isSettingAlrmCust[alrmNum] =true;
          break;
          default:
          alarmon[alrmNum]=DAILY; // Was custom, now Daily
          isSettingAlrmCust[alrmNum] =false; // Remove if want to keep custom Menu setting even if alarm is off (need to tweak to make cusotm right after OFF so it will remember day settings 
        }
        EEPROM.write (alarmOnOffLoc[alrmNum],alarmon[alrmNum]);
        delay (5);
        playSFX(1);
        cls();
        break;
     case 3: // Custom Day selection
       // Check if Custom Seeting was picked, if not skip this option
       switch (subMenu[4+alrmNum]) {
        case 1: // Mon
        alarmon[alrmNum]=alarmon[alrmNum] ^ MON; //Toggle
        break;
        case 2: // Tue
        alarmon[alrmNum]=alarmon[alrmNum] ^ TUE; //Toggle 
        break;
        case 3: // Wed
        alarmon[alrmNum]=alarmon[alrmNum] ^ WED; //Toggle 
        break;
        case 4: // Thu
        alarmon[alrmNum]=alarmon[alrmNum] ^ THU; //Toggle 
        break;
        case 5: // Fri
        alarmon[alrmNum]=alarmon[alrmNum] ^ FRI; //Toggle 
        break;
        case 6: // Sat
        alarmon[alrmNum]=alarmon[alrmNum] ^ SAT; //Toggle 
        break;
        case 7: // Sun
        alarmon[alrmNum]=alarmon[alrmNum] ^ SUN; //Toggle 
        break;
        case 8: // Exit
        subMenu[4+alrmNum]=0;
        subMenu[alrmNum]=4;//Move to next menu item
        cls();
        break;
       }
        EEPROM.write (alarmOnOffLoc[alrmNum],alarmon[alrmNum]);
        playSFX(1);
        cls();
     break;
     case 4: // Set Alarm hours
        isAlarmModified[alrmNum]=true; // Set to True so when exiting changes will be written to EEProm
        if (decrement) alrmHH[alrmNum]--; 
        else alrmHH[alrmNum]++;
        if (alrmHH[alrmNum]==255) alrmHH[alrmNum] = 23; // Negative number (byte) will be 255
        else if (alrmHH[alrmNum] > 23) alrmHH[alrmNum] = 0;      
        break;
     case 5: // Set Alarm minutes  
        isAlarmModified[alrmNum]=true; // Set to True so when exiting changes will be written to EEProm
        if (decrement) alrmMM[alrmNum]--; 
        else alrmMM[alrmNum]++;
        if (alrmMM[alrmNum] ==255) alrmMM[alrmNum] = 59; // Negative number (byte) will be 255
        else if (alrmMM[alrmNum] > 59) alrmMM[alrmNum] = 0;
        
        
        break;
    case 6: // Set Alarm melody  
        if (decrement) alrmToneNum[alrmNum]--;
        else alrmToneNum[alrmNum]++;
//        wave.stop();
        if (alrmToneNum[alrmNum] == 255) alrmToneNum[alrmNum] = 10; // Negative number (byte) will be 255
        else if (alrmToneNum[alrmNum] > 10) alrmToneNum[alrmNum] = 1;
        snprintf(myString,sizeof(myString), "ALRM%d.WAV",alrmToneNum[alrmNum]); // Make Alarm Filename
        playfile(myString);
        EEPROM.write(alarmToneLoc[alrmNum],alrmToneNum[alrmNum]);
        delay (5);
        cls();
        break;
  }

}

// =======================================================================================
// ---- Show Alarm setting screen ----
// By: LensDigital
// =======================================================================================
void showAlarm(byte color){
  if (!isSettingAlarm) return; // Exit if not setting alarm
  byte alrmNum;
  char myString[11];
  alrmBlink(color);
  if (menuItem==1) alrmNum=0; // Decide which Alarm to show
  else alrmNum=1;
  switch (subMenu[alrmNum]) { 
    
   case 1: // Alarm On/off
     snprintf(myString,sizeof(myString),"Alrm%d",alrmNum+1);
     showText(1,8,myString,1,color);
     if (alarmon[alrmNum] & ALRM_TGL) showText(10,0,"ON",1,hhColor); 
     else showText(10,0,"OFF",1,hhColor); 
     break;
    case 2:  // Alarm Frequency Setup (Off/Daily/Weekday/Custom)
     snprintf(myString,sizeof(myString),"Alrm%d",alrmNum+1);
     showText(1,8,myString,1,color);
     switch (alarmon[alrmNum]) {
      case 0:
      showText(10,0,alarmMenuStr[0],1,hhColor); // Should never come here!
      break;
      case 255:
      showText(0,0,alarmMenuStr[1],1,hhColor);
      break;
      case 252:
      showText(1,0,alarmMenuStr[2],1,hhColor);
      break;
      default:
      showText(0,0,alarmMenuStr[3],1,hhColor);
     }
     
   case 3: // Custom Alarm day setting
     snprintf(myString,sizeof(myString),"Alrm%d",alrmNum+1);
     showText(1,8,myString,1,color);
        switch (subMenu[4+alrmNum]){
         case 1: // Monday
         showText(0,0,alarmMenuStr[subMenu[4+alrmNum]+3],1,color);
         if (alarmon[alrmNum] & MON) showText(25,0,"Y",1,hhColor) ;
         else showText(25,0,"N",1,hhColor); 
         break;
         case 2: // Tue
         showText(0,0,alarmMenuStr[subMenu[4+alrmNum]+3],1,color);
         if (alarmon[alrmNum] & TUE) showText(25,0,"Y",1,hhColor);
         else showText(25,0,"N",1,hhColor);
         break;
         case 3: // Wed
         showText(0,0,alarmMenuStr[subMenu[4+alrmNum]+3],1,color);
         if (alarmon[alrmNum] & WED) showText(25,0,"Y",1,hhColor);
         else showText(25,0,"N",1,hhColor);
         break;
         case 4: // Thu
         showText(0,0,alarmMenuStr[subMenu[4+alrmNum]+3],1,color);
         if (alarmon[alrmNum] & THU) showText(25,0,"Y",1,hhColor);
         else showText(25,0,"N",1,hhColor);
         break;
         case 5: // Fri
         showText(0,0,alarmMenuStr[subMenu[4+alrmNum]+3],1,color);
         if (alarmon[alrmNum] & FRI) showText(25,0,"Y",1,hhColor);
         else showText(25,0,"N",1,hhColor);
         break;
         case 6: // Sat
         showText(0,0,alarmMenuStr[subMenu[4+alrmNum]+3],1,color);
         if (alarmon[alrmNum] & SAT) showText(25,0,"Y",1,hhColor);
         else showText(25,0,"N",1,hhColor);
         break;
         case 7: // Sun
         showText(0,0,alarmMenuStr[subMenu[4+alrmNum]+3],1,color);
         if (alarmon[alrmNum] & SUN) showText(25,0,"Y",1,hhColor);
         else showText(25,0,"N",1,hhColor);
         break;
         case 8: // Exit
         showText(0,0,alarmMenuStr[subMenu[4+alrmNum]+3],1,color);
         break;
      }
   break;
   case 4: // Setting Alarm hours and minutes
   case 5:
     snprintf(myString,sizeof(myString),"Alrm%d",alrmNum+1);
     showText(1,8,myString,1,color);
     if(time12hr) {
       // == BEGIN 12 Hour Mode ====
       myhours=alrmHH[alrmNum]; // Get Alarm Hours from EEPROM
       if (myhours==0) myhours=12; // Midnight
       else if (myhours >12) myhours=myhours-12;
       if (alrmHH[alrmNum]<12)  plot (0,1,hhColor); // Show AM Dot
       else plot (0,1,BLACK); // Hide AM Dot
       snprintf(myString,sizeof(myString), "%d",myhours);
       if ( (myhours/10)%10 ==0) {// It's one digit hour so need to hide first digit
         showText(1,0," ",1,hhColor);
         showText(7,0,myString,1,hhColor);
       }
       else
         showText(1,0,myString,1,hhColor);
      }
      // === END 12 Hour Mode ===
      else {
         //Serial.println ("We are in 24 Hour Mode");
        // 24 Hour Mode
        //plot (1,1,BLACK); // Hide AM Dot
        snprintf(myString,sizeof(myString), "%02d",alrmHH[alrmNum]); // make 2 digit hours
        showText(1,0,myString,1,hhColor);
      }
      snprintf(myString,sizeof(myString), "%02d",alrmMM[alrmNum]);
      showText(12,0,":",1,color); // Show colum :
      showText(18,0,myString,1,mmColor);
      
    break;
    case 6: // Alarm Tone selection
      showText(1,0,alarmMenuStr[13],1,color);
      snprintf(myString,sizeof(myString), "ALRM%d",alrmToneNum[alrmNum]); // Make Alarm Filename
      showText(0,8,myString,1,hhColor); 
    break;
    
  }
  
}

// =======================================================================================
// ---- Alarm Setting Blink Function ----
// ---- by LensDigital
// =======================================================================================
void alrmBlink(byte color) {
  int blinkDigDuration =500;
     if (blinking) {
       if ( (millis()-blinkTime > blinkDigDuration)) { // It's been over blinkDuration time
          blinkTime = millis(); // reset offset to current time
          if (isSettingAlrmHH) {
            if (hhColor==BLACK) {
             hhColor = color; // Inverse color of Hours 
             }
           else {
             hhColor = BLACK;
            
           }
             
           mmColor = color; // Minutes not blinking
          }
          if (isSettingAlrmMM) {
           if ( mmColor == BLACK ) mmColor = color; // Inverse color of Minutes 
           else mmColor = BLACK;
           hhColor = color; // Hours not blinking
          }
        } 
    } 
    else {  // DO not blink!
     if (isSettingAlrmHH) {
       hhColor=color;
       mmColor=color;
     }
     else {
       hhColor=color; 
       mmColor=color;
     }
    }
    
}


// =======================================================================================
// ---- Set TimeDate ----
// ---- by LensDigital
// =======================================================================================
void setTimeDate() {
  hours=hour(); // Store current hour value from clock
  minutes=minute(); // Store current minute value from clock
  seconds=second();
  months=month(); // Store current month value from clock  
  days=day();// Store current day value from clock  
  years=year() %100; // Store current year (last 2 digits) from clock  
  switch (subMenu[TIME_MENU]) {
    case 1: // Set hours
      if (decrement) hours--;
      else hours++;
      if (hours == 255) hours = 23;
      else if (hours > 23) hours = 0;
      break;
    case 2: // Set minutes
      if (decrement) minutes--;
      else minutes++;
      if (minutes == 255) minutes = 59;
      else if (minutes > 59) minutes = 0;
      seconds=0; // Reset seconds to 0 with each minute change
      break;
    case 3: // Set Days
      if (decrement) days--;
      else  days++;
      if (days == 255) days = 31;
      else if (days >31) days = 0;
      break;
    case 4: // Set months
       if (decrement) months--;
       else months++;
       if (months == 0) months=12;
      else if (months > 12) months=1;
      break;
    case 5: // Set Years 
      if (decrement) years--;
      else years++;
      if (years < 16 ) years = 40;
      else if (years > 40) years = 16; // Default to 2013
      break;
    case 6: // Set Timezone
      playSFX(1);
      if (decrement) currTZ--;
      else currTZ++;
      if (currTZ < 0 ) currTZ = 7;
      else if (currTZ > 7) currTZ = 0; // Default to 2016
      EEPROM.write(currTZLoc,currTZ);
      delay (15);
      rebootPending=true;
      break;
   
  }  
   // IMPORTANT! This will keep track of seconds for better time setting! 
   if ( millis()-last_ms > 1000) { // Has it been over 1 second since Set button was pressed?
      seconds++;
      if (seconds >59) {
          seconds=0;
          minutes++;
      }
      if (minutes > 59) {
          minutes=0;
          hours++;
      }
      if (hours >23){
          hours =0;
          days++;
      }
     last_ms=millis();   
    }
    setTime(hours,minutes,seconds,days,months,years); // Sets System time (and converts year string to integer)
    RTC.set(now()); // Writes time change to RTC chip     
}


// =======================================================================================
// ---- System Settings ----
// ---- by LensDigital
// =======================================================================================
void sysSetting(){
 //putstring_nl ("Setting System");
 if (subMenu[SYS_MENU]) playSFX(1); // Don't play sound if not setting anything, i.e. submenu=0
 switch (subMenu[SYS_MENU]) {
    case 1: // 12/24 Hour Mode
        if (!time12hr) time12hr=true; 
        else time12hr=false ; 
        EEPROM.write(mode24HRLoc,time12hr);
        delay (5);
         break;
     case 2: // Set Display Brightness
        cls();
        if (decrement) brightness--;
        else brightness++;
        if (brightness == 255) brightness = 5;
        else if (brightness > 5) brightness = AUTO_BRIGHTNESS_ON;
        EEPROM.write(brightLoc,brightness);
        delay (15);
        prevBrightness=0; 
        if (brightness==0) autoBrightness();
        else
          setBrightness(brightness);
        break;
     case 3: // Set Clock Color
       cls();
       if (decrement) clockColor--;
       else clockColor++;
       if (clockColor == 0) clockColor=3;
       else if (clockColor>3) clockColor=1;
       EEPROM.write(clockColorLoc,clockColor);
       delay (5);
       break;
     case 4: // Set Clock Font (0-5)
       cls();
       if (decrement) clockFont--;
       else clockFont++;
       if (clockFont ==255) clockFont=4;
       else if (clockFont>4) clockFont=0;
       EEPROM.write(clockFontLoc,clockFont);
       delay (5);
       break;
     case 5: // Set sFX on/off
       cls();
       if (sFX) sFX=false;
       else  sFX=true;
       EEPROM.write(sFXLoc,sFX);
       delay (5);
       break;
     case 6: // Temperature Units
       cls();
       if (tempUnit) tempUnit=false;
       else  tempUnit=true;
       EEPROM.write(tempUnitLoc,tempUnit);
       delay (5);
       break;
      case 7: // Sound Volume
        cls();
        if (decrement) sndVol++;
        else sndVol--;
        if (sndVol == 255) sndVol=8;
        else if (sndVol > 8) sndVol=0;
        //playSFX(1);
        EEPROM.write(sndVolLoc,sndVol);
        delay (5);
        break;
      case 8: // Startup on/off
        cls();
        if (doStartup) doStartup=false;
        else doStartup=true;
        //playSFX(1);
        EEPROM.write(doStartupLoc,doStartup);
        delay (5);
        break;
      case 9: // RF Setup
         cls();
        switch (subMenu[RF_MENU]) {
        case 0: // Enter Menu
          subMenu[RF_MENU]=1;
        break;
        case 1: // RFM On/Off
          if ( RFM_Enabled ) { // If chip phisically present
            if (isRadioPresent) isRadioPresent=false;
            else {
              isRadioPresent=true;
              delay (15);
            }
            //playSFX(1);
            EEPROM.write(radioOnLoc,isRadioPresent);
            delay (5);
          }
          else {
            playSFX(4);
            isRadioPresent=false;
          }
        break;
        case 2: // Frequency
           if (decrement) RF_Frequency--;
           else RF_Frequency++;
           if (RF_Frequency>2) RF_Frequency=0;
           EEPROM.write(RF_FreqLoc,RF_Frequency);
           delay (5);
        break;
        case 3: // NodeID
           if (decrement) RF_Node-- ;
           else RF_Node++ ;
           if (RF_Node>99) RF_Node=1;
           EEPROM.write(RF_NodeLoc,RF_Node);
           delay (5);
        break;
        case 4: // Network ID
           if (decrement) RF_Network-- ;
           else RF_Network++ ;
           if (RF_Network>99) RF_Network=1;
           EEPROM.write(RF_NetworkLoc,RF_Network);
           delay (5);
        break;
        case 5: // Sensor ID
           if (decrement) RF_SensorID-- ;
           else RF_SensorID++ ;
           if (RF_SensorID>99) RF_SensorID=1;
           EEPROM.write(RF_SensorLoc,RF_SensorID);
           delay (5);
        break;
        case 6: // Encryption
           RF_Encrypt=!RF_Encrypt;
           EEPROM.write(RF_EncryptLoc,RF_Encrypt);
           delay (5);
        break;
        case 7: // Exit
           subMenu[RF_MENU]=0;
        break;
        
        }
      break;
      case 10: // IR
        cls(); 
        if (IR_PRESENT) { // If IR Hardware defined globally
          if (isIRPresent) isIRPresent=false; // Turn off IR
          else isIRPresent=true; // Turn on IR
          //playSFX(1);
        }
        EEPROM.write(IROnLoc,isIRPresent); 
        delay (5);
      break;
      case 11: // Factor Reset (INIT EEPROM)
        cls(); 
        initEPROM();
        getEEPROMSettings ();
         // Exit and reinitialize
        isInMenu = false;
        isSettingTime = false;
        isSettingDate = false;
        isSettingAlarm = false;
        isSettingSys=false;
        isSettingOptions=false;
        blinking = false;
        isSettingHours   = false;
        isSettingMinutes = false;
        isSettingMonth   = false;
        isSettingDay   = false;
        isSettingYear   = false;
        menuItem=0;
        decrement=false;
       for (int i=0;i<MAX_SUBMENUS;i++) { // Initialize SubMenus
          subMenu[i]=0;
       }
       
        okClock=true; 
        isIncrementing = false; 
        cls();
        rebootPending=true;
      break;
  }

}

// =======================================================================================
// ---- Show System setting screen ----
// By: LensDigital
// =======================================================================================
void showSys(){
  if (!isSettingSys) return; // Exit if not setting system
  byte color=clockColor;
  int blinkDigDuration =500;
  if ( (millis()-blinkTime > blinkDigDuration)) { // It's been over blinkDuration time
      blinkTime = millis(); // reset offset to current time
      if ( hhColor == BLACK ) hhColor = color; // Inverse color  
      else hhColor = BLACK;
  } 
  switch (subMenu[SYS_MENU]){ 
    case 1: // We are setting 12/24 Hour mode
      showText(1,0,sysMenuStr[subMenu[SYS_MENU]-1][0],1,color);
      if (time12hr)showText(1,8,"12 HR ",1,hhColor);
      else showText(1,8,"24 HR",1,hhColor);
      break;
    case 2:  // Adjust Brightness Level
      showText(1,0,sysMenuStr[subMenu[SYS_MENU]-1][0],1,color);
      //Serial.println (brightness);
      switch (brightness) {
      case 0:
        showText(1,8,sysBrightStr[brightness],1,hhColor);
        break;
      case 1:
        showText(1,8,sysBrightStr[brightness],1,hhColor);
        break;
      case 2:
        showText(8,8,sysBrightStr[brightness],1,hhColor);
        break;
      case 3:
        showText(8,8,sysBrightStr[brightness],1,hhColor);
        break;
      case 4: 
        showText(5,8,sysBrightStr[brightness],1,hhColor);
        break;
      case 5: 
        showText(8,8,sysBrightStr[brightness],1,hhColor);
        break;
      }
    break;
    case 3: // Set Clock Color
      showText(1,0,sysMenuStr[subMenu[SYS_MENU]-1][0],1,color);
      switch ( clockColor) {
       case 1:  
         showText(1,8,sysColorStr[clockColor-1],1,GREEN);
         break;
       case 2:  
         showText(8,8,sysColorStr[clockColor-1],1,RED);
         break;  
       case 3:  
         showText(1,8,sysColorStr[clockColor-1],1,ORANGE);
         break;
      }
    break;
    case 4: // Set Clock Font
      char myString[8];
      showText(1,0,sysMenuStr[subMenu[SYS_MENU]-1][0],1,color);
      snprintf(myString,sizeof(myString), "Font%d",clockFont); // Show current font number
      showText(1,8,myString,1,hhColor);
    break;
    case 5: // Enable/Disable menu SFX
      showText(2,0,sysMenuStr[subMenu[SYS_MENU]-1][0],1,color);
      showText(0,8,sysMenuStr[subMenu[SYS_MENU]-1][1],1,color);
      if (sFX) showText(18,8,"ON",3,hhColor);
      else showText(18,8,"OFF",3,hhColor);
    break;
    case 6: // Temperature C or F
      showText(2,0,sysMenuStr[subMenu[SYS_MENU]-1][0],1,color);
      if (tempUnit) showText(10,8,"F",1,hhColor);
      else  showText(10,8,"C",1,hhColor);
      ht1632_putchar(18,8,127,color); // Show Degree Sign
      break;
    case 7: // Sound Volume
      byte barColor; // Color of volume bar
      showText(0,0,sysMenuStr[subMenu[SYS_MENU]-1][0],1,color);
      showText(2,9,sysMenuStr[subMenu[SYS_MENU]-1][1],1,color);
      for (int y=8;y>sndVol;y--)
         for (int x=0;x<y-sndVol;x++)
          plot ( (x+24)+(8-y),(y+8)-1, color); 
      break; 
    case 8: // Startup on/off
      showText(2,0,sysMenuStr[subMenu[SYS_MENU]-1][0],3,color);
      if (doStartup) showText(10,8,"ON",1,hhColor); 
      else showText(10,8,"OFF",1,hhColor); 
    break;
    case 9: // RF Settings
      switch (subMenu[RF_MENU]) {
        case 0: // Show Title of RF Menu
          showText(2,0,sysMenuStr[subMenu[SYS_MENU]-1][0],1,color); 
          showText(0,8,sysMenuStr[subMenu[SYS_MENU]-1][1],1,color); 
        break;
        case 1: // RFM On/Off
          showText(1,0,sysRFMStr[subMenu[RF_MENU]-1],1,color); // "Turn"
          if (isRadioPresent) showText(10,8,"ON",1,hhColor); 
          else showText(10,8,"OFF",1,hhColor);  
        break;
        case 2: // Frequency
          if (!isRadioPresent) { // Skip rest if no radio disabled. GO to exit
            subMenu[RF_MENU]=7;
            break;
          }
          showText(1,0,sysRFMStr[subMenu[RF_MENU]-1],1,color); // "FREQ"
          //snprintf(myString,sizeof(myString), "%s",RF_Frequency); // Make string
          switch (RF_Frequency) {
          case 0:
            showText(0,8,"433Mhz",1,hhColor);
          break;
          case 1:
            showText(0,8,"868Mhz",1,hhColor);
          break;
          case 2:
            showText(0,8,"915Mhz",1,hhColor);
          break;
          default:
            showText(0,8,"915Mhz",1,hhColor);
          break;
        }
        break;
        case 3: // NODE ID
          showText(1,0,sysRFMStr[subMenu[RF_MENU]-1],1,color); // "NODE"
          snprintf(myString,sizeof(myString), "ID:%d",RF_Node); // Make string
          showText(0,8,myString,1,hhColor);
        break;
        case 4: // Network ID
          showText(1,0,sysRFMStr[subMenu[RF_MENU]-1],1,color); // "NET"
          snprintf(myString,sizeof(myString), "%d",RF_Network); // Make string
          showText(10,8,myString,1,hhColor);
        break;
        case 5: // Sensor ID
          showText(1,0,sysRFMStr[subMenu[RF_MENU]-1],1,color); // "SNSR"
          snprintf(myString,sizeof(myString), "ID:%d",RF_SensorID); // Make string
          showText(0,8,myString,1,hhColor);
        break;
        case 6: // Encryption
          showText(1,0,sysRFMStr[subMenu[RF_MENU]-1],1,color); // "ENCR"
          if (RF_Encrypt) showText(20,8,"Y",1,hhColor);
          else showText(20,8,"N",1,hhColor);
        break;
        case 7: // Exit
          showText(1,0,sysRFMStr[subMenu[RF_MENU]-1],1,color); // "Exit"
          //snprintf(myString,sizeof(myString), "%d",RFMFreq); // Make string
          //showText(0,8,myString,1,hhColor);
        break;
      }
    if (subMenu[RF_MENU]) rebootPending=true;
    break;
    case 10: // IR Reciever
      showText(2,0,sysMenuStr[subMenu[SYS_MENU]-1][0],1,color); 
      if (isIRPresent) showText(10,8,"ON",1,hhColor); 
      else showText(10,8,"OFF",1,hhColor);  
    break;
    case 11: // Factory Reset (EEPROM Init)
     showText(0,0,sysMenuStr[subMenu[SYS_MENU]-1][0],1,hhColor);
     showText(0,9,sysMenuStr[subMenu[SYS_MENU]-1][1],1,hhColor);
    break;
  }
}


// =======================================================================================
// ---- User Option Settings ----
// ---- by LensDigital
// =======================================================================================
void optSetting(){
 if (subMenu[USER_MENU]) playSFX(1); // Don't play sound if not setting anything, i.e. submenu=0
 switch (subMenu[USER_MENU]) {
   case 1: // InfoDisplay
     switch (subMenu[SCROLL_MENU]) {
       case 0: // Enter Menu
         cls();
         subMenu[SCROLL_MENU]=1;
         //putstring_nl ("Enter talk Submenu");
         break;
       case 1: // Frequency
         if (decrement) infoFreq--;
         else infoFreq++;
         if (infoFreq == 0) infoFreq=9;
         else if (infoFreq > 9) infoFreq=0;
         EEPROM.write(infoFreqLoc,infoFreq);
         delay (5);
         cls();
       break;
       case 2: // Scroll Date
         putstring_nl ("Scroll Date");
         infoOptions = infoOptions ^ IO_Date; //Toggle
         EEPROM.write (infoOptionsLoc,infoOptions);
         delay (5);   
        break;
       case 3: // Scroll inernal temp
         putstring_nl ("Scroll Temp");
         infoOptions = infoOptions ^ IO_InTemp; //Toggle
         EEPROM.write (infoOptionsLoc,infoOptions);
         delay (5);   
         break;
       case 4: // Scroll External Temp
         if ( RFM_Enabled ) {
           infoOptions = infoOptions ^ IO_OutTemp; //Toggle
           EEPROM.write (infoOptionsLoc,infoOptions);
           delay (5);   
         } 
         else playSFX(4);
         break;
       case 5: // Scroll  Alarm
         infoOptions = infoOptions ^ IO_Alarms; //Toggle
         EEPROM.write (infoOptionsLoc,infoOptions);
         delay (5);   
         break;
       case 6: // Scroll  Sensor Data
       if ( RFM_Enabled ) {
         infoOptions = infoOptions ^ IO_RFStat; //Toggle
         EEPROM.write (infoOptionsLoc,infoOptions);
         delay (5);   
       }
       else playSFX(4);
       break;
       case 7: // Scroll  Internal Humidity data
         infoOptions = infoOptions ^ IO_InHum; //Toggle
         EEPROM.write (infoOptionsLoc,infoOptions);
         delay (5);   
       break;
       case 8: // Scroll  External Humidity data
       if ( RFM_Enabled ) {
         infoOptions = infoOptions ^ IO_OutHum; //Toggle
         EEPROM.write (infoOptionsLoc,infoOptions);
         delay (5);   
       }
       else playSFX(4);
       break;
       case 9: // Exit
         subMenu[SCROLL_MENU]=0;
       break;
     }
     
   break;
   case 2: // Say Items Toggle
     switch (subMenu[TALK_MENU]) {
        case 0: // Enter Menu
         cls();
         subMenu[TALK_MENU]=1;
         //putstring_nl ("Enter talk Submenu");
         break;
       case 1: // Say Time
         sayOptions = sayOptions ^ SO_Time; //Toggle
         EEPROM.write (sayOptionsLoc,sayOptions);
         delay (5);   
         break;
      case 2: // Say Date
         sayOptions = sayOptions ^ SO_Date; //Toggle
         EEPROM.write (sayOptionsLoc,sayOptions);
         delay (5);   
         break;
      case 3: // Say Internal Temp
         sayOptions = sayOptions ^ SO_InTemp; //Toggle
         EEPROM.write (sayOptionsLoc,sayOptions);
         delay (5);   
         break;
      case 4: // Say External Temp
         //Serial.println ("Saving say Ext Temp");
         if ( RFM_Enabled ) {
           sayOptions = sayOptions ^ SO_OutTemp; //Toggle
           EEPROM.write (sayOptionsLoc,sayOptions);
           delay (10);   
         }
         else playSFX(4);
         break;
     case 5:
         sayOptions = sayOptions ^ SO_InHum; //Toggle
         EEPROM.write (sayOptionsLoc,sayOptions);
         delay (10);   
         break;
     break;
     case 6: // Say External Humidity
         if ( RFM_Enabled ) {
           sayOptions = sayOptions ^ SO_OutHum; //Toggle
           EEPROM.write (sayOptionsLoc,sayOptions);
           delay (10);   
         }
         else playSFX(4);
         break;
      case 7: // Say  Alarm
         
         sayOptions = sayOptions ^ SO_Alarms; //Toggle
         EEPROM.write (sayOptionsLoc,sayOptions);
         delay (10);   
         break;
      case 8: // Exit
         subMenu[TALK_MENU]=0;
         break;
     }
  break;
  case 3: // temperature offset
     if (decrement) tmpOffset--;
     else tmpOffset++;
     if (tmpOffset == 255) tmpOffset=9;
     else if (tmpOffset>9) tmpOffset=0;
     EEPROM.write (tmpOffsetLoc,tmpOffset);
     delay (5);
     cls();
   break;
 
 }
 
}

// =======================================================================================
// ---- Show User Options setting screen ----
// By: LensDigital
// =======================================================================================
void showOpt(){
  if (!isSettingOptions) return; // Exit if not setting options
  byte color=clockColor;
  char myString[2]; 
  int blinkDigDuration =500;
  if ( (millis()-blinkTime > blinkDigDuration)) { // It's been over blinkDuration time
      blinkTime = millis(); // reset offset to current time
      if ( hhColor == BLACK ) hhColor = color; // Inverse color  
      else hhColor = BLACK;
  } 
  switch (subMenu[USER_MENU]){ 
   case 1: // InfoDisplay
   switch (subMenu[SCROLL_MENU]){ 
     case 0: // Main Title of this menu
       showText(0,0,optMenuStr[subMenu[USER_MENU]-1][0],1,color);
       showText(0,8,optMenuStr[subMenu[USER_MENU]-1][1],1,color);
     break;
     case 1: // Frequency
       showText(1,0,optInfoDispStr[0],1,color);
       if (infoFreq==0) showText(1,8,"Never",1,hhColor);
       else {
        showText(0,8,optInfoDispStr[subMenu[SCROLL_MENU]],3,color);
        snprintf(myString,sizeof(myString), "%d",infoFreq); // Make string
        showText(24,8,myString,3,hhColor);
       }
      break;
      case 2: // Scroll Date
       showText(0,0,optInfoDispStr[0],1,color);
       showText(0,8,optInfoDispStr[subMenu[SCROLL_MENU]],1,color);
       if (infoOptions & IO_Date) showText(25,8,"Y",1,hhColor) ;
       else showText(25,8,"N",1,hhColor); 
      break;
      case 3: // Scroll Inside Temp
       showText(0,0,optInfoDispStr[0],1,color);
       showText(0,8,optInfoDispStr[subMenu[SCROLL_MENU]],3,color);
       if (infoOptions & IO_InTemp) showText(25,8,"Y",1,hhColor) ;
       else showText(25,8,"N",1,hhColor);
      break;
      case 4: // Scroll Outside Temp
       if (!isRadioPresent) { // Skip if Reciever is not enabled
         subMenu[SCROLL_MENU]++;
         break;
       }
       showText(0,0,optInfoDispStr[0],1,color);
       showText(0,8,optInfoDispStr[subMenu[SCROLL_MENU]],3,color);
       if (infoOptions & IO_OutTemp) showText(25,8,"Y",1,hhColor) ;
       else showText(25,8,"N",1,hhColor);
       break;
       case 5: // Scroll Alarm
       showText(0,0,optInfoDispStr[0],1,color);
       showText(0,8,optInfoDispStr[subMenu[SCROLL_MENU]],3,color);
       if (infoOptions & IO_Alarms) showText(25,8,"Y",1,hhColor) ;
       else showText(25,8,"N",1,hhColor);
      break;
      case 6: // Scroll reciever stats
       if (!isRadioPresent) { // Skip if Reciever is not enabled
         subMenu[SCROLL_MENU]++;
         break;
       }
       showText(0,0,optInfoDispStr[0],1,color);
       showText(0,8,optInfoDispStr[subMenu[SCROLL_MENU]],3,color);
       if (infoOptions & IO_RFStat) showText(25,8,"Y",1,hhColor) ;
       else showText(25,8,"N",1,hhColor);
       break;
       case 7: // Scroll Internal Humidity
         showText(0,0,optInfoDispStr[0],1,color);
         showText(0,8,optInfoDispStr[subMenu[SCROLL_MENU]],3,color);
         if (infoOptions & IO_InHum) showText(25,8,"Y",1,hhColor) ;
         else showText(25,8,"N",1,hhColor);
       break;
       case 8: // Scroll External humidity
       if (!isRadioPresent) { // Skip if Reciever is not enabled
         subMenu[SCROLL_MENU]++;
         break;
       }
       showText(0,0,optInfoDispStr[0],1,color);
       showText(0,8,optInfoDispStr[subMenu[SCROLL_MENU]],3,color);
       if (infoOptions & IO_OutHum) showText(25,8,"Y",1,hhColor) ;
       else showText(25,8,"N",1,hhColor);
      break;
      case 9: // Exit
      showText(0,0,optInfoDispStr[subMenu[SCROLL_MENU]],1,color);
      break;
     }
   break;
   case 2: // Talk Items selection (Submenu 7)
     
     if (subMenu[TALK_MENU]==0) { // Will skip showing this text if we are deeper in submenu
       showText(0,0,optMenuStr[subMenu[USER_MENU]-1][0],1,color);
       showText(0,8,optMenuStr[subMenu[USER_MENU]-1][1],1,color);
     }
     //Serial.println (subMenu[TALK_MENU]);
     switch (subMenu[TALK_MENU]){
     case 1: // Say Time
       showText(0,0,optSayStr[0],1,color);
       showText(0,8,optSayStr[subMenu[TALK_MENU]],1,color);
       if (sayOptions & SO_Time) showText(25,8,"Y",1,hhColor) ;
       else showText(25,8,"N",1,hhColor); 
     break;
     case 2: // Say Date
       showText(0,0,optSayStr[0],1,color);
       showText(0,8,optSayStr[subMenu[TALK_MENU]],1,color);
       if (sayOptions & SO_Date) showText(25,8,"Y",1,hhColor) ;
       else showText(25,8,"N",1,hhColor); 
     break;
     case 3: // Say Inside Temperature
       showText(0,0,optSayStr[0],1,color);
       showText(0,8,optSayStr[subMenu[TALK_MENU]],3,color);
       if (sayOptions & SO_InTemp) showText(25,8,"Y",1,hhColor) ;
       else showText(25,8,"N",1,hhColor); 
     break;
     case 4: // Say Outside Temperature
       if (!isRadioPresent) { // Skip if Reciever is not enabled
         subMenu[TALK_MENU]++;
         break;
       }
       showText(0,0,optSayStr[0],1,color);
       showText(0,8,optSayStr[subMenu[TALK_MENU]],3,color);
       if (sayOptions & SO_OutTemp) showText(25,8,"Y",1,hhColor) ;
       else showText(25,8,"N",1,hhColor); 
       break;
     case 5: // Say Internal Humidity
       showText(0,0,optSayStr[0],1,color);
       showText(0,8,optSayStr[subMenu[TALK_MENU]],3,color);
       if (sayOptions & SO_InHum) showText(25,8,"Y",1,hhColor) ;
       else showText(25,8,"N",1,hhColor); 
       break;
     case 6: // Say External Humidity
       if (!isRadioPresent) { // Skip if Reciever is not enabled
         subMenu[TALK_MENU]++;
         break;
       }
       showText(0,0,optSayStr[0],1,color);
       showText(0,8,optSayStr[subMenu[TALK_MENU]],3,color);
       if (sayOptions & SO_OutHum) showText(25,8,"Y",1,hhColor) ;
       else showText(25,8,"N",1,hhColor); 
       break;
     case 7: // Say Alarm
       showText(0,0,optSayStr[0],1,color);
       showText(0,8,optSayStr[subMenu[TALK_MENU]],1,color);
       if (sayOptions & SO_Alarms) showText(25,8,"Y",1,hhColor) ;
       else showText(25,8,"N",1,hhColor); 
       break;
     case 8: // Exit
       showText(0,0,optSayStr[subMenu[TALK_MENU]],1,color);
       break;
     }
   break;
   case 3: // Temperature offset
     showText(0,0,optMenuStr[subMenu[USER_MENU]-1][0],1,color);
     showText(0,8,optMenuStr[subMenu[USER_MENU]-1][1],3,color);
     snprintf(myString,sizeof(myString), "%d",tmpOffset); // Make string
     showText(25,8,myString,1,hhColor);
   break;
  }
}



// =======================================================================================
// ---- Check Incr button to show Quick Menu ----
// ---- Press and quickly release to go thru one item at a time
// ---- Hold button for more than a second to autoamtically go thru all items.
// ---- by LensDigital
// =======================================================================================
void quickMenu(){
  
  //if (isInMenu) return; //Make sure we are not in settings menu
  if ( (!isInQMenu) && (buttonsStatus[INC_BUTTON]==B_INACTIVE) ) return; // Return if were button wasn't pressed or we not in QMenu
  // First, handle Alarms
  if ( !isInMenu && soundAlarm[0]) { // If pressed Stops Alarm (snooze)
    snoozeProc(0);
    buttonsStatus[INC_BUTTON] = B_INACTIVE;
    butActivated[INC_BUTTON]=false;
    return; //will prevent from entering menu for 1 minute since alarm sounded
  }
  if (!isInMenu && soundAlarm[1]) { // If pressed Stops Alarm (snooze)
     snoozeProc(1);
     buttonsStatus[INC_BUTTON] = B_INACTIVE;
     butActivated[INC_BUTTON]=false;
     return; //will prevent from entering menu for 1 minute since alarm sounded
  }
  if (soundAlarm[0]) interruptAlrm[0]=true; // If pressed Stops Alarm
  if (soundAlarm[1]) interruptAlrm[1]=true; // If pressed Stops Alarm
  
  // Now process Qmenu
  isInQMenu=true;
  //if ( buttonsStatus[INC_BUTTON]!=B_INACTIVE ) {// Only execute if button was pressed
  if (butActivated[INC_BUTTON]) {
    //Serial.println (F("Qmenu was pressed"));
    if (buttonsStatus[INC_BUTTON] == B_HELD) {
        talkingMenu(true); // Button was held. So play all items
        //Serial.println ("Long Talk");
        buttonsStatus[INC_BUTTON] = B_INACTIVE;
        butActivated[INC_BUTTON]=false;
    }
    else if (buttonsStatus[INC_BUTTON] == B_RELEASED) {
      talkingMenu(false); 
      //Serial.println ("Short  talk");
      buttonsStatus[INC_BUTTON] = B_INACTIVE;
      butActivated[INC_BUTTON]=false;
    }
    lastButtonTime=millis(); // Prevents premature exit from Qmenu
  }
  /*
  if (isInQMenu)
      if ( (buttonsStatus[MENU_BUTTON] == B_PRESSED) || (buttonsStatus[SET_BUTTON] == B_PRESSED) ){
         lastButtonTime = 0; // Exit Quick Menu if any button other than INC was pressed
      }
      */
  // display the menu option for 5 sec after menu button was pressed;
  if ((lastButtonTime > 0) && (millis() - lastButtonTime < 5000))
    return;
  // Finished with menus, return to normal operations
  if (isInQMenu) {
    putstring_nl ("Exiting QMenu");
    cls ();
    isInQMenu=false;
    okClock=true; // Enable Big Clock display
    isSettingDate=false;
    mbutState=1; // Reset Menu Button State
    isIncrementing = false;
    blinking = false;
    isSettingAlarm=false;
    menuItem=0; 
    subMenu[0]=0;
    subMenu[1]=0;
    mbutState=1;
    buttonsStatus[INC_BUTTON]=B_INACTIVE;
    
  }
  
}

// =======================================================================================
// ---- Play menu sounds (sFX) ----
// ---- Pass 1 for menu naviagtion, 2 chnging item , 3 for exit, 4 for error
// ---- by LensDigital
// =======================================================================================
void playSFX(byte item){
  if (!sFX) return; // Global Sound Effects are off
  turnOffRadio(); // Disable RF12B
  switch (item) {
   case 1: 
    playfile("MENU1.WAV");
    break;
   case 2:
    playfile("MENU2.WAV"); 
    break;
   case 3: 
    playfile("MENU3.WAV");
    break;
  case 4: 
    playfile("ERR1.WAV");
    break;    
  }
  //radio.Wakeup(); // Disable RF12B
}

// =======================================================================================
// ---- Write ALarm HH:MM changes to EEProm ----
// ---- Using deidcated function to slow wear down of EEProm 
// ---- by LensDigital
// =======================================================================================
void writeEEPROMAlrm (byte alrmNum) {
  //putstring_nl ("Write Alarm to EEProm");
  EEPROM.write(alarmHHLoc[alrmNum],alrmHH[alrmNum]);
  delay (15);
  EEPROM.write(alarmMMLoc[alrmNum],alrmMM[alrmNum]);
  delay (15);
  isAlarmModified[alrmNum]=false;
}

// =======================================================================================
// ---- Helper Process for Set Button for Talking Setting ----
// ---- by Bratan
// =======================================================================================
void userMenu () {
     cls ();
     isSettingAlarm = false;
     isSettingTime   = false;
     isSettingSys=false;
     isSettingOptions=true;
     if (subMenu[USER_MENU]==1) { // Do not increment this submenu (we are in Infodisplay Options menu)
        //putstring_nl ("In subMenu");
       if (subMenu[SCROLL_MENU] == 0) subMenu[USER_MENU]++; // Go to next item, we are not entering this submenu tree
       else {
         subMenu[SCROLL_MENU]++;
         if (subMenu[SCROLL_MENU] > 9) subMenu[SCROLL_MENU]=1; // Goes back to first item of this submenu 
       }
     }
     else if (subMenu[USER_MENU]==2) { // Do not increment this submenu (we are in Talking Options menu)
              //putstring_nl ("In subMenu");
              
             if (subMenu[TALK_MENU] == 0) subMenu[USER_MENU]++; // Go to next item, we are not entering this submenu tree
             else {
               subMenu[TALK_MENU]++;
               if (subMenu[TALK_MENU] > 8) subMenu[TALK_MENU]=1; // Goes back to first item of this submenu 
             }
     } 
     else subMenu[USER_MENU]++; // Increment button press count
     if (subMenu[USER_MENU] > 3) subMenu[USER_MENU]=1; // Goes back to first menu item
     cls ();
}

// =======================================================================================
// ---- Helper Process for Set Button for Time/Date Setting ----
// ---- by Bratan
// =======================================================================================
void butSetClock() {
  cls();
        startBlinking();
        isSettingAlarm = false;
        isSettingTime   = true;
        isSettingSys=false;
        subMenu[TIME_MENU]++; // Increment button press count
        if (subMenu[TIME_MENU] > TMENU_ITEMS) subMenu[TIME_MENU]=1; // Goes back to first menu item
      	switch (subMenu[TIME_MENU]) {
          case 1: // Set hours
            //putstring_nl ("SET: Hours");
            cls();
            okClock=true;
            isSettingTime   = true;
            isSettingDate = false;
            isSettingHours   = true;
        	  isSettingMinutes = false;
        	  isSettingMonth   = false;
        	  isSettingDay   = false;
        	  isSettingYear   = false;
            isSettingDST = false;
          break;
          case 2: // Set Minutes
            //putstring_nl ("SET: Mins");
            isSettingHours   = false;
        	  isSettingMinutes = true;
        	  isSettingMonth   = false;
        	  isSettingDay   = false;
        	  isSettingYear   = false;
          break;
          case 3: // Set Day
          //putstring_nl ("SET: Day");
            okClock=false;
            cls();
            isSettingDate = true;
            isSettingHours   = false;
        	  isSettingMinutes = false;
        	  isSettingMonth   = false;
        	  isSettingDay   = true;
        	  isSettingYear   = false;
          break;
          case 4: // Set Month
            //putstring_nl ("SET: Month");
            isSettingHours   = false;
        	  isSettingMinutes = false;
        	  isSettingMonth   = true;
        	  isSettingDay   = false;
        	  isSettingYear   = false;
          break;
          case 5: // Set year
            //putstring_nl ("SET: Year");
            isSettingHours   = false;
        	  isSettingMinutes = false;
        	  isSettingMonth   = false;
        	  isSettingDay   = false;
        	  isSettingYear   = true;
          break;
          case 6: // Timezone
            isSettingDST = true;
            isSettingDate = false;
            isSettingYear   = false;
            showTZ();
          break;
          
        }
}

// =======================================================================================
// ---- Display Timezone menu ----
// ---- by LensDigital
// =======================================================================================
void showTZ() {
  if (!isSettingDST) return;
  cls();
  //okClock=true;
  isSettingDate = false;
  isSettingHours   = false;
  isSettingMinutes = false;
  isSettingMonth   = false;
  isSettingDay   = false;
  isSettingYear   = false;
  showText(0,0,"TZone",2,clockColor);
  showText(0,8,TZMenuStr[currTZ],2,clockColor); 
}

// =======================================================================================
// ---- Helper Process for Set Button for Alarm Setting ----
// ---- by Bratan
// =======================================================================================
void butSetAlarm (byte alrmNum) {
     startBlinking();
     //putstring_nl ("SET: Alarm");
     cls();
     isSettingAlarm = true;
     isSettingSys=false;
     if (subMenu[alrmNum]==3) ; // Do not increment
     else subMenu[alrmNum]++; // Increment button press count
     if (subMenu[alrmNum] > 6) subMenu[alrmNum]=1; // Goes back to first menu item
     if (!(alarmon[alrmNum] & ALRM_TGL) && subMenu[alrmNum]==2)  subMenu[alrmNum]=4; // ALarm is off so we need to skip 4nd menu
     if (!isSettingAlrmCust[alrmNum] && subMenu[alrmNum]==3) subMenu[alrmNum]=4; // Custom Alarm is not set so skip to 4th menu
      switch (subMenu[alrmNum]) {
         case 1: // Set Alarm on Off
           cls();
           isSettingDate = false;
           isSettingAlarm = true;
           isSettingAlrmHH = true;
           break;
         case 2: // SET: Alarm Dalily/Weekday/Custom
           //putstring_nl ("SET: Alarm OFF/Dalily/Weekday/Custom");
           cls();
           isSettingDate = false;
           isSettingAlarm = true;
           isSettingAlrmHH = true;
           break;
          case 3: // Set Alarm Custom schedule
            if (isSettingAlrmCust[alrmNum]) {
             //putstring_nl ("SET: Alarm1. Custom Schedule");
             subMenu[4+alrmNum]++;
             if (subMenu[4+alrmNum] > 8) subMenu[4+alrmNum]=1; // Goes back to first menu item (Monday)
           }
           isSettingAlrmHH = true;
           isSettingAlrmMM = false;
           break;
          case 4: // Set Alarm Hrs
          //putstring_nl ("SET: Alarm HRS");
           cls();
           isSettingAlrmHH = true;
           isSettingAlrmMM = false;
          break;
          case 5: // Set Alarm min
          //putstring_nl ("SET: Alarm MIN");
           isSettingAlrmHH = false;
           isSettingAlrmMM = true;
           break; 
         case 6: // Set Alarm Tone
         //putstring_nl ("SET: Alarm Tone");
           isSettingAlrmHH = true; 
           isSettingAlrmMM = false; 
           break;
        }
}

// ==================================================================================================
// --- Upon exiting menu (or doing reset) INIT all vars
// ==================================================================================================
void exitMenu() {
  // Exit and reinitialize
  if (rebootPending) softReboot();
        isInMenu = false;
        isSettingTime = false;
        isSettingDate = false;
        isSettingAlarm = false;
        isSettingSys=false;
        isSettingOptions=false;
        blinking = false;
        isSettingHours   = false;
        isSettingMinutes = false;
        isSettingMonth   = false;
        isSettingDay   = false;
        isSettingYear   = false;
        menuItem=0;
        decrement=false;
       for (int i=0;i<MAX_SUBMENUS;i++) { // Initialize SubMenus
          subMenu[i]=0;
       }
        okClock=true; 
        isIncrementing = false; 
        if (isAlarmModified[0]) writeEEPROMAlrm(0);
        if (isAlarmModified[1]) writeEEPROMAlrm(1);
        wave.stop(); // Any sounds (in case Alarm Tone is playing and sFX are disabled)
        playSFX(3);
        cls();
}

