// =======================================================================================
// Display large digit (full screen) time
// By: LensDigital
// --------------------------------------------------------------------------------------- 
void showBigTime(byte color){
  //if (menuItem > 3 || !okClock) return; // Date setting is in progress. Do not show clock
  //if (!okClock) return; // Date setting is in progress. Do not show clock
  //int blinkDotDuration =500; // How frequently dots should blink
  int blinkDigDuration = 500; // Frequencey of digit blinking during time setting
  int x=0; //offset
  if (isSettingTime){ // Setting time, so blink correct digits
    //Serial.println ("Setting time");
    //blinkColor=color; // Show steady divider dots
    if ( (millis()-blinkTime > blinkDigDuration)) { // It's been over blinkDuration time
      blinkTime = millis(); // reset offset to current time
      if (isSettingHours) {
        //putstring_nl ("Setting hours");
       if ( hhColor == BLACK ) hhColor = color; // Inverse color of Hours 
       else hhColor = BLACK;
       mmColor = color; // Minutes not blinking
       ssColor = color; // Seconds not blinking
      }
      if (isSettingMinutes) {
        //putstring_nl ("Setting minutes");
       if ( mmColor == BLACK ) mmColor = color; // Inverse color of Minutes 
       else mmColor = BLACK;
       hhColor = color; // Hours not blinking
       ssColor = color; // Seconds not blinking
      }
      if (isSettingSeconds) {
        //putstring_nl ("Setting seconds");
       if ( ssColor == BLACK ) ssColor = color; // Inverse color of seconds 
       else ssColor = BLACK;
       mmColor = color; // Minutes not blinking
       hhColor = color; // Hours not blinking
      }
    }
  }
  else { hhColor=color; mmColor=color; ssColor=color;} // We are not setting time, so show digits as usual
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
   showDigit(0,2,(myhours/10)%10,5,clockFont,hhColor); // Show 1st digit of hour
  }
  showDigit(6-x,2,myhours%10,5,clockFont,hhColor); // Always Show 2nd digit of hour
  showDivider (15,5,color);
  showDigit(15,2,(minute()/10)%10,5,clockFont,mmColor); // Show 1st digit of minute
  showDigit(22,2,minute()%10,5,clockFont,mmColor); // Show 2nd digit of minute
  showDivider (32,5,color);
  showDigit(32,2,(second()/10)%10,5,clockFont,ssColor); // Show 1st digit of second
  showDigit(39,2,second()%10,5,clockFont,ssColor); // Show 2nd digit of second
  
}

// =======================================================================================
// ---- Display Date and year (mostly for setting) ----
// By: LensDigital
// ---------------------------------------------------------------------------------------
void mainDate(byte color){
  if (!isSettingDate) return; // Not setting Date
  int blinkDigDuration = 500; // Frequencey of digit blinking during time setting
  char dateString[10]; // Var will hold generated string
  // Blinker processor (used to blink text, number during date setting
  if (isSettingDate){
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
   
   snprintf(dateString, sizeof(dateString),"%s",monthStr(month(LOCAL_TZ) ) ); // Create Month String
   showText(0,0,dateString,2,monColor); // Show month
   snprintf(dateString,sizeof(dateString), "%2d",day(LOCAL_TZ) ); // create Day strng
   showText(0,8,dateString,2,ddColor); // Show day
   snprintf(dateString, sizeof(dateString),"%2d",year(LOCAL_TZ)); // create Year String
   showText(20,8,dateString,1,yyColor); // Show year
}
// ---------------------------------------------------------------------------------------

// =======================================================================================
// ---- Scroll Date and year  ----
// By: LensDigital
// ---------------------------------------------------------------------------------------
void showDate(byte disp)
{
  if (isInMenu) return;
  if (screenInUse[disp] && !showingDate) return;
  if ((unsigned long)millis()-lastDateShow < NewScreenDelay ) return; // Check if just run this to prevent it from hogging screen space
  if (!showingDate) {
    //Serial.println ("Debug: Date scroll");
    showingDate=true;
    screenInUse[disp]=true;
  }
  snprintf(topBuff, sizeof(topBuff),"%s %2d, %4d",monthShortStr(month(LOCAL_TZ)),day(LOCAL_TZ),year(LOCAL_TZ)); // Create Month String
  if (scrollDone[disp]) { // Reset permits
    showingDate=false;
    screenInUse[disp]=false;
    scrollDone[disp]=false;
    //Serial.println (F("Debug: Done with scorlling date"));
    lastDateShow=millis();
  }
  
}

// ======================================================
// Display divider colon :
// Adjusts size and position based on clock Font and AM/PM
// by LensDigital
// Add X starting position and Y starting position
// =======================================================
void showDivider(byte x_start, byte y_start,byte color){
  int x=0; //offset
  if(time12hr && x_start<24) x=2; // Offset divider by 2 dots left for 12 hr mode
  if (clockFont<3) plot (x_start-x,y_start,color);
  plot ((x_start+1)-x,y_start,color);
  if (clockFont<3) plot (x_start-x,y_start+1,color);
  plot ((x_start+1)-x,y_start+1,color);
  if (clockFont<3) plot (x_start-x,y_start+4,color);
  plot ((x_start+1)-x,y_start+4,color);
  if (clockFont<3) plot (x_start-x,y_start+5,color);
  plot ((x_start+1)-x,y_start+5,color);
}

