// =======================================================================================
// ---- Check button input and act upon it ----
// =======================================================================================
void buttonProc() {
  // check MENU button;
  showAlarm(clockColor); // Display Alarm setting screen
  if (!isInQMenu) mainDate(clockColor); // Show date setting screen, but only if we are not in QMenu
  
  showSys();
  showOpt();
  scanButtons(); // Scan buttons logic
  if ( buttonsStatus[MENU_BUTTON] == B_PRESSED ) processMenuButton();
  if (isInMenu)
  {
    if ( buttonsStatus[SET_BUTTON] == B_PRESSED ) processSetButton();
    //if ( (buttonsStatus[INC_BUTTON] == B_PRESSED) || (buttonsStatus[INC_BUTTON] == B_HELD) ) processIncButton();
    if ( butActivated[INC_BUTTON] ) processIncButton();
     // display the menu option for 6 seconds after menu button was pressed;
  }
  else quickMenu();
  
  // If we are in menu, let's see if we can exit and initi menu vars
  if (isInMenu) {
      if ((lastButtonTime > 0) && (millis() - lastButtonTime < 6000)) { 
        if ( millis() - lastButtonTime > 1000 )  { // Start blinking if buttons not touched for a second
           isIncrementing = false;
          if (!isInQMenu) startBlinking();
        }
        return;
      }
    
        // return the main mode if no button was pressed for 6 seconds;
        Serial.println (F("Exit menu"));
        exitMenu();
    }
  
  
}


// =======================================================================================
// ---- Process Menu Button ----
// Adopted from Florinc by Bratan 
// =======================================================================================
void processMenuButton()
{
  if (isInQMenu) return; // We are in quick menu, so don't show settings menu
  // debouncing;
  //if ((millis() - lastButtonTime) < BOUNCE_TIME_BUTTON)
  //  return;
    
 // ====  Alarm control   ====
 if (resetAlrm(0) || resetAlrm(1) ) {
  buttonsStatus[MENU_BUTTON]=B_INACTIVE; // Reset button status
  return;
 }
 
  //putstring_nl ("Is In Menu Button Proc");
  lastButtonTime = millis();
  last_ms=millis(); // Create Offset for keeping track of seconds
  isInMenu = true;
  startBlinking();
  isSettingAlarm = false;
  isSettingTime = false;
  isSettingDate = false;
  menuItem++; // Increment button press count
  if (menuItem > MAX_MENUS) menuItem=1; // Goes back to first menu item
  showMenu();
  buttonsStatus[MENU_BUTTON]=B_INACTIVE; // Reset button status
}
// ---------------------------------------------------------------------------------------



// =======================================================================================
// ---- Process Set Button ----
// Adopted from Florinc by Bratan (added date setting)
// =======================================================================================
void processSetButton()
{
  
  // debouncing;
 // if ((millis() - lastButtonTime) < BOUNCE_TIME_BUTTON)
 //   return;
 
  // If Alarms sound it will stop them and snooze
  snoozeProc(0);
  snoozeProc(1);
 
  if (!isInMenu) return; // failsafe
  lastButtonTime = millis();
  //isInMenu    = true;
  playSFX(2);
  switch (menuItem) {
    case 1: // Begin Set Alarm 1
    //putstring_nl ("SET: Alarm1");
    butSetAlarm(0);
    break; // End set Alarm 1
    case 2: // Begin Set Alarm 2
    //putstring_nl ("SET: Alarm2");
    butSetAlarm(1);
    break; // End set Alarm
    case 3: // Begin Set Time and Date
        butSetClock();
    break;

    case 4: // Begin System Settings
        cls();
        isSettingAlarm = false;
        isSettingTime   = false;
        isSettingSys=true;
        // Handle deeper levels of submenu (for RFM)
        if (subMenu[SYS_MENU]==9) { // Do not increment this submenu (we are in RF Settings)
          if (subMenu[RF_MENU]==0) subMenu[SYS_MENU]++; // Go to next item we are not entering this submenu tree
          else { // We are entered deeper level so let's increment it here
            subMenu[RF_MENU]++;
            if (subMenu[RF_MENU] > 7) subMenu[RF_MENU]=1; // Go back to 1st item of submenu
          }
        }
        else subMenu[SYS_MENU]++; // Increment button press count
        if (subMenu[SYS_MENU] > 11) subMenu[SYS_MENU]=1; // Goes back to first menu item
    break;
    case 5: // User options
        cls();
        userMenu();
    break;
    }
    buttonsStatus[SET_BUTTON]=B_INACTIVE; // Reset button status
  
}

// =======================================================================================
// ---- Process Increment button ----
// Adopted from Florinc by Bratan
// =======================================================================================
void processIncButton()
{  
  int incrSpeed=300; // Default incrementing speed
  if (buttonsStatus[INC_BUTTON]==B_RELEASED) {
    butActivated[INC_BUTTON]=false;
    buttonsStatus[INC_BUTTON]=B_INACTIVE;
    return;
  }
  if (buttonsStatus[INC_BUTTON]==B_HELD) incrSpeed=50;
  if ((unsigned long)millis()-lastButtonTime < incrSpeed )return;
    isIncrementing = true;
    stopBlinking();
    lastButtonTime = millis();
    switch (menuItem) {
      case 1: // Setting Alarm 1
        setAlarm(0);
        break;
      case 2: // Setting Alarm 2
        setAlarm(1);
        break;  
      case 3: // Setting Time and Date
        setTimeDate();
        showTZ();
        break;
      case 4: // System Settings
        sysSetting();
        break;
      case 5: // User Options settings
        optSetting();
      break;
    }
    //if (buttonsStatus[INC_BUTTON]==B_RELEASED) buttonsStatus[INC_BUTTON]=B_INACTIVE; // Reset button status
    //buttonsStatus[INC_BUTTON]=B_INACTIVE; // Reset button status
    
  
}


// =======================================================================================
// ---- Scans and debounces all buttons ----
// =======================================================================================
void scanButtons() {
  for (byte i=0;i<3;i++) {
    butCurrentState[i]=digitalRead (buttonPins[i]);
    if (butCurrentState[i] != butLastState[i]){
        butLastTime[i] = millis();
    }
    if (millis() - butLastTime[i] > debounceInterval){//if enough time has passed
       if (butCurrentState[i] != butDebouncedState[i]){//if the current state is still different than our last stored debounced state
        butDebouncedState[i] = butCurrentState[i];//update the debounced state
        //trigger an event
        if (butDebouncedState[i] == BUTTON_ON){
          //Serial.print (i+1); Serial.println(" button pressed");
          buttonsStatus[i]=B_PRESSED;
          butActivated[i]=true;
          butHeldTime[i]=millis();
          butHeld[i]=true; // Allows button hold logic to procede
          //butAction=true;
        } else {
          //Serial.print (i+1); Serial.println(" button released"); 
          //if (buttonsStatus[i]!=B_INACTIVE) buttonsStatus[i]=B_RELEASED; //bug fix
          buttonsStatus[i]=B_RELEASED;
        }
      }
      // Logic that detects button HOLD event
      if (butDebouncedState[i] == BUTTON_ON) { // If we get here, button is still held
        if ( butHeld[i] ) if (  millis()- butHeldTime[i] > HOLD_TIME) 
        { //If we allowed, check how long it passed since button was pressed
          //Serial.println (F("Button Held!")); // Bingo, it is held!
          butHeld[i]=false;  // This makes sure that we skip button held check logic, we only need to go here once per hold
          if (buttonsStatus[i]!=B_INACTIVE) buttonsStatus[i]=B_HELD;
          //butAction=true;
        }
      }
    }
    
    butLastState[i] = butCurrentState[i];
  }
}

