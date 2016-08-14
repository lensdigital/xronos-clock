// =======================================================================================
// ---- Check button input and act upon it ----
// Adopted from Florinc by Bratan
// =======================================================================================
void buttonProc(){
  // check MENU button;
  if (!isInQMenu) mainDate(clockColor); // Show date setting screen, but only if we are not in QMenu
  mainDate(clockColor);
  showSys();
  showOpt();
  if (digitalRead(MENU_BUTTON_PIN) == HIGH)
  {
    processMenuButton();
  }
   // check SET button;
  if (isInMenu)
  {
    if (digitalRead(SET_BUTTON_PIN) == HIGH)
    {
      // "Set" button was pressed. Go into this level of menu and stay until exit form it or timeout
      processSetButton();
     }
     if (digitalRead(INC_BUTTON_PIN) == HIGH)
    {
      // "INC" button was pressed. Start changing according values
      processIncButton();
     }
  }
  // display the menu option for 6 seconds after menu button was pressed;
  if ((lastButtonTime > 0) && (millis() - lastButtonTime < 6000)) {
    if ( millis() - lastButtonTime > 1000 )  { // Start blinking if buttons not touched for a second
       isIncrementing = false;
      if (!isInQMenu) startBlinking();
    }
    return;
  }

  // return the main mode if no button was pressed for 5 seconds;
   if (isInMenu)
  {
    exitMenu(); // Exit and reinitalize
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
  if ((millis() - lastButtonTime) < BOUNCE_TIME_BUTTON)
    return;
    
 // ====  Alarm control   ====
// if (resetAlrm(0) || resetAlrm(1) ) return;
 
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
  
}
// ---------------------------------------------------------------------------------------

// =======================================================================================
// ---- Process Set Button ----
// Adopted from Florinc by Bratan (added date setting)
// =======================================================================================
void processSetButton()
{
  
  // debouncing;
  if ((millis() - lastButtonTime) < BOUNCE_TIME_BUTTON)
    return;
  if (!isInMenu) return; // failsafe
  lastButtonTime = millis();
  //isInMenu    = true;
//  playSFX(2);
  switch (menuItem) {
    case 1: // Begin Set Time and Date
        showSetClock();
    break;

    case 2: // Begin System Settings
       sysSetHelper();
    break;
    case 3: // User options
        cls();
        userMenu();
    break;
    }
}

// =======================================================================================
// ---- Process Increment button ----
// Adopted from Florinc by Bratan
// =======================================================================================
void processIncButton()
{  
  // debouncing;
  if ((millis() - lastButtonTime) < BOUNCE_TIME_BUTTON)
    return;
  isIncrementing = true;
  stopBlinking();
  lastButtonTime = millis();
  switch (menuItem) {
    case 1: // Setting Time and Date
      setTimeDate();
      showTZ();
      break;
    case 2: // System Settings
      sysSetting();
      break;
    case 3: // User Options settings
      userSetting();
    break;
  }
}
