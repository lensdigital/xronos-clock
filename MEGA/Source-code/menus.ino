// =======================================================================================
// Menu Text definitions. Some are 2 dimental arrays first element 1st row, 2nd is second row
// =======================================================================================
char *TZMenuStr[8]={"Eastern","Central","Mountain","Arizona","Pacific","Alaska","Hawaii","UTC"};
char *sysRFMStr[7]={"Enabled","Freq.","Node","Network","Sensor","Encrypt","Exit"};

// =======================================================================================
// ---- Show Menu items and set vars ----
// ---- by LensDigital
// =======================================================================================
void showMenu()
{
//  playSFX(2);
 switch (menuItem) {
   case 1: // Time and Date setting menu
        Serial.println  (F("Display Date/Time Menu"));
        okClock=false;
        isSettingSys=false;
        isSettingOptions=false;
        cls();
        showText(0,0,"Set Time",1,GREEN);
        showText(5,8,"& Date",1,GREEN);
        break;
   case 2: // System Settings (preferences/setup)
        Serial.println  (F("Display System Menu"));
        okClock=false;
        cls();
        showText(5,0,"System",1,GREEN);
        showText(5,8,"Setup",1,GREEN);
        break;
   case 3: // User Options menu
       Serial.println  (F("Display Options Menu"));
       okClock=false;
       isSettingSys=false;
       cls();
       showText(5,0,"User",1,GREEN);
       showText(5,8,"Options",1,GREEN);
       break;
  }
  
}

// =======================================================================================
// ---- Show menu for Time/Date Setting ----
// ---- by Bratan
// =======================================================================================
void showSetClock() {
       // if (!isSettingTime) return; // Exit if not setting Time
        cls();
        startBlinking();
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
            isSettingSeconds = false;
            isSettingMonth   = false;
            isSettingDay   = false;
            isSettingYear   = false;
            isSettingTZ = false;
            showText(14,0,"Set",2,clockColor); // Show Help text
            showText(7,8,"Hours",2,clockColor); // Show Help text
          break;
          case 2: // Set Minutes
            //putstring_nl ("SET: Mins");
            isSettingHours   = false;
            isSettingMinutes = true;
            isSettingSeconds = false;
            isSettingMonth   = false;
            isSettingDay   = false;
            isSettingYear   = false;
            showText(14,0,"Set",2,clockColor); // Show Help text
            showText(5,8,"Minutes",2,clockColor); // Show Help text
          break;
          case 3: // Set Seconds
            isSettingDate = false;
            isSettingHours   = false;
            isSettingMinutes = false;
            isSettingSeconds = true;
            isSettingMonth   = false;
            isSettingDay   = false;
            isSettingYear   = false;
            showText(14,0,"Set",2,clockColor); // Show Help text
            showText(5,8,"Seconds",2,clockColor); // Show Help text
          break;
          case 4: // Set Day
            //putstring_nl ("SET: Day");
            okClock=false;
            cls();
            isSettingTime = false;
            isSettingDate = true;
            ssColor=clockColor; // Make sure seconds are now showing
            isSettingHours   = false;
            isSettingMinutes = false;
            isSettingSeconds = false;
            isSettingMonth   = false;
            isSettingDay   = true;
            isSettingYear   = false;
          break;
          case 5: // Set Month
           //putstring_nl ("SET: Month");
            isSettingTime = false;
            isSettingHours   = false;
            isSettingSeconds = false;
            isSettingMinutes = false;
            isSettingMonth   = true;
            isSettingDay   = false;
            isSettingYear   = false;
          break;
          case 6: // Set year
            //putstring_nl ("SET: Year");
            isSettingHours   = false;
            isSettingMinutes = false;
            isSettingSeconds = false;
            isSettingMonth   = false;
            isSettingDay   = false;
            isSettingYear   = true;
          break;
          case 7: // Set Timezone
            Serial.println (F("SET: Timezone"));
            isSettingTZ = true;
            showTZ();
          break;
        }
}
// =======================================================================================
// ---- Display Timezone menu ----
// ---- by LensDigital
// =======================================================================================
void showTZ() {
  if (!isSettingTZ) return;
  cls();
  okClock=true;
  isSettingDate = false;
  isSettingHours   = false;
  isSettingMinutes = false;
  isSettingSeconds = false;
  isSettingMonth   = false;
  isSettingDay   = false;
  isSettingYear   = false;
  showText(0,0,"TimeZone",2,clockColor);
  showText(0,8,TZMenuStr[currTZ],2,clockColor); 
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
  
//  playSFX(1);
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
    case 3: // Set Seconds
      // putstring_nl ("Setting Seconds");
      seconds =0;
      break;
    case 4: // Set Days
      if (decrement) days--;
      else  days++;
      if (days == 255) days = 31;
      else if (days >31) days = 0;
      break;
    case 5: // Set months
       if (decrement) months--;
       else months++;
       if (months == 0) months=12;
      else if (months > 12) months=1;
      showText(0,0,"         ",2,monColor); // Clear Month text
      break;
    case 6: // Set Years 
      if (decrement) years--;
      else years++;
      if (years < 16 ) years = 40;
      else if (years > 40) years = 16; // Default to 2016
      break;
    case 7: // Set Timezone 
      if (decrement) currTZ--;
      else currTZ++;
      if (currTZ < 0 ) currTZ = 6;
      else if (currTZ > 7) currTZ = 0; // Default to first TZ
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
// ---- Show System setting screen ----
// By: LensDigital
// =======================================================================================
void showSys(){
  if (!isSettingSys) return; // Exit if not setting system
  byte color=clockColor;
  int blinkDigDuration =500;
  if ( (millis()-blinkTime > blinkDigDuration)) { // It's been over blinkDuration time
      blinkTime = millis(); // reset offset to current time
      if ( miscColor == BLACK ) miscColor = color; // Inverse color  
      else miscColor = BLACK;
  } 
  switch (subMenu[SYS_MENU]){ 
    case 1: // We are setting 12/24 Hour mode
      showText(0,0,"HRs Mode",1,color);
      if (time12hr)showText(1,8,"12 Hours ",1,miscColor);
      else showText(1,8,"24 Hours",1,miscColor);
      break;
    case 2:  // Adjust Brightness Level
      showText(0,0,"Brightns",1,color);
      switch (brightness) {
      case 0:
        showText(1,8,"Auto",1,miscColor);
        break;
      case 1:
        showText(1,8,"Night",1,miscColor);
        break;
      case 2:
        showText(8,8,"Low",1,miscColor);
        break;
      case 3:
        showText(1,8,"Medium",1,miscColor);
        break;
      case 4: 
        showText(5,8,"High",1,miscColor);
        break;
      case 5: 
        showText(0,8,"Maximum",1,miscColor);
        break;
      }
    break;
    case 3: // Set Clock Font
      char myString[8];
      showText(1,0,"Font:",1,color);
      snprintf(myString,sizeof(myString), "Font%d",clockFont); // Show current font number
      showText(1,8,myString,1,miscColor);
    break;
    case 4: // Temperature C or F
      showText(0,0,"Temp.",1,color);
      showText(0,8,"Units:",1,color);
      if (tempUnit) showText(40,8,"F",1,miscColor);
      else  showText(40,8,"C",1,miscColor);
      //ht1632_putchar(43,8,127,color); // Show Degree Sign
      break;
   case 5: // RF Setup
    switch (subMenu[RF_MENU]) {
      case 0: // Show Title of RF Menu
       showText(10,0,"Radio",1,color);
       showText(0,8,"Settings",1,color); 
      break;
      case 1: // RFM On/Off
        showText(1,0,sysRFMStr[subMenu[RF_MENU]-1],1,color); // "Enabled"
        if (isRadioPresent) showText(15,8,"ON",1,miscColor); 
        else showText(15,8,"OFF",1,miscColor);  
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
            showText(5,8,"915 Mhz",1,miscColor);
          break;
          case 1:
            showText(5,8,"433 Mhz",1,miscColor);
          break;
          case 2:
            showText(5,8,"868 Mhz",1,miscColor);
          break;
          default:
            showText(5,8,"915 Mhz",1,miscColor);
          break;
        }
        
      break;
      case 3: // NODE ID
        showText(5,0,sysRFMStr[subMenu[RF_MENU]-1],1,color); // "NODE"
        snprintf(myString,sizeof(myString), "ID:%d",RF_Node); // Make string
        showText(15,8,myString,1,miscColor);
      break;
      case 4: // Network ID
        showText(1,0,sysRFMStr[subMenu[RF_MENU]-1],1,color); // "NET"
        snprintf(myString,sizeof(myString), "ID:%d",RF_Network); // Make string
        showText(15,8,myString,1,miscColor);
      break;
      case 5: // Sensor ID
        showText(1,0,sysRFMStr[subMenu[RF_MENU]-1],1,color); // "SNSR"
        snprintf(myString,sizeof(myString), "ID:%d",RF_SensorID); // Make string
        showText(15,8,myString,1,miscColor);
      break;
      case 6: // Encryption
        showText(1,0,sysRFMStr[subMenu[RF_MENU]-1],1,color); // "Encrypt"
        if (RF_Encrypt) showText(20,8,"Y",1,hhColor);
        else showText(20,8,"N",1,miscColor);
      break;
      case 7: // Exit
        showText(1,0,sysRFMStr[subMenu[RF_MENU]-1],1,color); // "Exit"
        //snprintf(myString,sizeof(myString), "%d",RFMFreq); // Make string
        //showText(0,8,myString,1,hhColor);
      break;
    }
    break;
    case 6: // LED Indicator
      showText(2,0,"LED",1,color); 
      if (LEDEnabled) showText(10,8,"ON",1,miscColor); 
      else showText(10,8,"OFF",1,miscColor);  
    break;
    case 7: // GPS Reciever
      showText(2,0,"GPS",1,color); 
      if (GPSEnabled) showText(10,8,"ON",1,miscColor); 
      else showText(10,8,"OFF",1,miscColor);
    break;
    case 8: // IR Reciever
      showText(0,0,"IR Remote",1,color); 
      if (isIRPresent) showText(10,8,"ON",1,miscColor); 
      else showText(10,8,"OFF",1,miscColor);
    break;
    case 9: // Factory Reset
      showText(2,0,"FACTORY",1,miscColor); 
      showText(0,8,"RESET!!!",1,miscColor);
    break;
  }
}

// =======================================================================================
// ---- System Settings ----
// ---- by LensDigital
// =======================================================================================
void sysSetting(){
Serial.println  (F("Setting System"));
// if (subMenu[SYS_MENU]) playSFX(1); // Don't play sound if not setting anything, i.e. submenu=0
 switch (subMenu[SYS_MENU]) {
    case 1: // 12/24 Hour Mode
        if (!time12hr) time12hr=true; 
        else time12hr=false ; 
        EEPROM.write(mode24HRLoc,time12hr);
        delay (15);
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
     case 3: // Set Clock Font (0-5)
       cls();
       if (decrement) clockFont--;
       else clockFont++;
       if (clockFont ==255) clockFont=4;
       else if (clockFont>4) clockFont=0;
       EEPROM.write(clockFontLoc,clockFont);
       delay (5);
       break;
     case 4: // Temperature Units
       cls();
       if (tempUnit) tempUnit=false;
       else  tempUnit=true;
       EEPROM.write(tempUnitLoc,tempUnit);
       delay (15);
       break;
      case 5: // RF Setup
        cls();
        if (subMenu[RF_MENU]>0) rebootPending=true; // Reboot after settings done
        switch (subMenu[RF_MENU]) {
        case 0: // Enter Menu
          subMenu[RF_MENU]=1;
        break;
        case 1: // RFM On/Off
          if ( RFM12B_Enabled ) { // If chip phisically present
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
            //playSFX(4);
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
           if (RF_Node>20) RF_Node=1;
           EEPROM.write(RF_NodeLoc,RF_Node);
           delay (5);
        break;
        case 4: // Network ID
           if (decrement) RF_Network-- ;
           else RF_Network++ ;
           if (RF_Network>20) RF_Network=1;
           EEPROM.write(RF_NetworkLoc,RF_Network);
           delay (5);
        break;
        case 5: // Sensor ID
           if (decrement) RF_SensorID-- ;
           else RF_SensorID++ ;
           if (RF_SensorID>20) RF_SensorID=1;
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
      case 6: // LED Indicator
        cls();
        if (LEDEnabled) LEDEnabled=false;
        else LEDEnabled=true;
        //playSFX(1);
        EEPROM.write(LEDEnabledLoc,LEDEnabled);
        delay (15);
       break;
       case 7: // GPS
        cls();
        if (GPSEnabled) GPSEnabled=false;
        else GPSEnabled=true;
        //playSFX(1);
        EEPROM.write(GPSEnabledLoc,GPSEnabled);
        delay (15);
        rebootPending=true;
       break;
      case 8: // IR
      cls();
      if (isIRPresent) isIRPresent=false;
      else isIRPresent=true;
      //playSFX(1);
      EEPROM.write(IROnLoc,isIRPresent); 
        delay (5);
      rebootPending=true;
      break;
      case 9: // Factory Reset
        cls(); 
        initEPROM();
        getEEPROMSettings ();
        rebootPending=true;
        exitMenu();
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
      if ( miscColor == BLACK ) miscColor = color; // Inverse color  
      else miscColor = BLACK;
  } 
  switch (subMenu[USER_MENU]){ 
   case 1: // InfoDisplay
      if (subMenu[SCROLL_MENU]==0) { // Will skip showing this text if we are deeper in submenu
       showText(0,0,"Srolling",1,color);
       showText(0,8,"Settings",1,color);
       }
     switch (subMenu[SCROLL_MENU]){  
     case 1: // Frequency
       showText(5,0,"Scroll",1,color);
       showText(0,8,"Speed:",1,color);
       snprintf(myString,sizeof(myString), "%d",infoFreq); // Make string
       showText(40,8,myString,1,miscColor);
      break;
      case 2: // Scroll Date
       showText(5,0,"Show",1,color);
       showText(5,8,"Date",1,color);
       if (infoOptions & 128) showText(40,8,"Y",1,miscColor) ;
       else showText(40,8,"N",1,miscColor); 
      break;
      case 3: // Scroll Inside Temp
       showText(5,0,"Show",1,color);
       showText(0,8,"Int.Temp.",3,color);
       if (infoOptions & 64) showText(40,8,"Y",1,miscColor) ;
       else showText(40,8,"N",1,miscColor);
      break;
      case 4: // Scroll Outside Temp
       showText(5,0,"Show",1,color);
       showText(0,8,"Ext.Temp.",3,color);
       if (infoOptions & 32) showText(40,8,"Y",1,miscColor) ;
       else showText(40,8,"N",1,miscColor);
      break;
      case 5: // Scroll Outside Humidity
       showText(5,0,"Show",1,color);
       showText(0,8,"Humidity",3,color);
       if (infoOptions & 16) showText(40,8,"Y",1,miscColor) ;
       else showText(40,8,"N",1,miscColor);
      break;
      case 6: // Scroll GPS updated time
       showText(5,0,"Show",1,color);
       showText(0,8,"GPS Data",3,color);
       if (infoOptions & 8) showText(40,8,"Y",1,miscColor) ;
       else showText(40,8,"N",1,miscColor);
      break;
      case 7: // Exit
      showText(10,0,"Exit",1,color);
      break;
     }
   break;
  case 2: // Temperature offset
     showText(0,0,"IntTemp",1,color);
     showText(0,8,"Offset:-",3,color);
     snprintf(myString,sizeof(myString), "%d",tmpOffset); // Make string
     showText(40,8,myString,1,miscColor);
   break;
  }
}

// =======================================================================================
// ---- User Option Settings ----
// ---- by LensDigital
// =======================================================================================
void userSetting(){
 switch (subMenu[USER_MENU]) {
   case 1: // InfoDisplay Scrolling
     switch (subMenu[SCROLL_MENU]) {
       case 0: // Enter Menu
         cls();
         subMenu[SCROLL_MENU]=1;
         Serial.println  (F("Enter Scroll Submenu"));
         break;
       case 1: // Speed
         if (decrement) infoFreq--;
         else infoFreq++;
         if (infoFreq == 0) infoFreq=9;
         else if (infoFreq > 9) infoFreq=0;
         EEPROM.write(infoFreqLoc,infoFreq);
         delay (5);
         cls();
       break;
       case 2: // Scroll Date
         Serial.println  (F("Scroll Date"));
         infoOptions = infoOptions ^ 128; //Toggle
         EEPROM.write (infoOptionsLoc,infoOptions);
         delay (5);   
        break;
       case 3: // Scroll inernal temp
         Serial.println  (F("Scroll Temp"));
         infoOptions = infoOptions ^ 64; //Toggle
         EEPROM.write (infoOptionsLoc,infoOptions);
         delay (5);   
         break;
       case 4: // Scroll External Temp
         infoOptions = infoOptions ^ 32; //Toggle
         EEPROM.write (infoOptionsLoc,infoOptions);
         delay (5);   
         break;
       case 5: // Scroll External Humidity
         infoOptions = infoOptions ^ 16; //Toggle
         EEPROM.write (infoOptionsLoc,infoOptions);
         delay (5);   
         break;
       case 6: // Scroll GPS data
         infoOptions = infoOptions ^ 8; //Toggle
         EEPROM.write (infoOptionsLoc,infoOptions);
         delay (5);   
         break;
       case 7: // Exit
         subMenu[SCROLL_MENU]=0;
       break;
     }
   break;
  case 2: // temperature offset
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
// ---- Helper Process for Set Button for User Options ----
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
         if (subMenu[SCROLL_MENU] > 7) subMenu[SCROLL_MENU]=1; // Goes back to first item of this submenu 
       }
     }
     else subMenu[USER_MENU]++; // Increment button press count
     if (subMenu[USER_MENU] > 2) subMenu[USER_MENU]=1; // Goes back to first menu item
     cls ();
}

// =======================================================================================
// ---- Helper Process for Set Button for System Settings ----
// ---- by Bratan
// =======================================================================================
void sysSetHelper(){
  cls();
  isSettingAlarm = false;
  isSettingTime   = false;
  isSettingSys=true;
  // Handle deeper levels of submenu (for RFM)
  if (subMenu[SYS_MENU]==5) { // Do not increment this submenu (we are in RF Settings)
    if (subMenu[RF_MENU]==0) subMenu[SYS_MENU]++; // Go to next item we are not entering this submenu tree
    else { // We are entered deeper level so let's increment it here
      subMenu[RF_MENU]++;
      if (subMenu[RF_MENU] > 7) subMenu[RF_MENU]=1; // Go back to 1st item of submenu
    }
  }
  else subMenu[SYS_MENU]++; // Increment button press count
  if (subMenu[SYS_MENU] > SYSMENU_ITEMS) subMenu[SYS_MENU]=1; // Goes back to first menu item
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
    isSettingTZ - false;
    menuItem=0;
    decrement=false;
   for (int i=0;i<MAX_SUBMENUS;i++) { // Initialize SubMenus
      subMenu[i]=0;
   }
    okClock=true; 
    isIncrementing = false; 
    cls();
}
