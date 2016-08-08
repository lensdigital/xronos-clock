/////////////////////////////////// Wave Shield Functions
/*
 * print error message and halt
 */
void error_P(const char *str) {
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  while(1);
}
/*
 * print error message and halt if SD I/O error, great for debugging!
 */
void sdErrorCheck(void) {
  
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
  
}
// =======================================================================================
// ---- Plays a full file from beginning to end with no pause.   ----
// =======================================================================================
// 
void playcomplete(char *name) {
  wdt_disable(); // Disable Watchdog 
  wdt_reset(); // Resets Watchdog so clock doesn't reboot (keepalive)
  // call our helper to find and play this name
 turnOffRadio(); // Disable RF69
  playfile(name);
  while (wave.isplaying) {
  // do nothing while its playing
  }
  // now its done playing
  //radio.Wakeup(); // Re-enable RF12B
  wdt_enable(WDTO_8S); // Re-enable watchdog
}

// =======================================================================================
// ---- Plays a WAVE until new Play requested   ----
// =======================================================================================
// 
void playfile(char *name) {
  
  turnOffRadio(); // Disable RF69
  
  // see if the wave object is currently doing something
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
  }
  //sdReset(); // Reinitialize SD card (SPI conflict with RFM69)
  // look in the root directory and open the file
  if (!f.open(root, name)) {
    //putstring("Couldn't open file "); 
    delay (20);
    // Retry
    if (!f.open(root, name)) {
      putstring("Couldn't open file "); Serial.print(name);   
      softReboot(); // Rebooting to fix SPI lock
      //return; 
    }
  }
  
  // OK read the file and turn it into a wave object
  if (!wave.create(f)) {
    putstring_nl("Not a valid WAV"); return;
  }
  wave.volume=sndVol; // Set Playback Sound
  // ok time to play! start playback
  wave.play();
 
}

// =======================================================================================
// ---- Plays a WAVE for alarm if nothing currently playing.   ----
// =======================================================================================
void playalarmfile(char *name, byte alrmnum) {
  wdt_disable(); // Disable Watchdog 
  turnOffRadio(); // Disable RF69
  // see if the wave object is currently doing something
  if (wave.isplaying) {// already playing something, so just skip
    return;
  }
  //card.init(); // Reset SD Card
  // look in the root directory and open the file
   if (!f.open(root, name)) {
    //putstring("Couldn't open file "); 
    delay (20);
    // Retry
    if (!f.open(root, name)) {
      putstring("Couldn't open file "); Serial.print(name);  
      softReboot(); // Rebooting to fix SPI lock
      //return; 
    }
  }
  // OK read the file and turn it into a wave object
  if (!wave.create(f)) {
    putstring_nl("Not a valid WAV");  return;
  }
  // Check if Alarm should be escalated
  if (alrmToneNum[alrmnum]<6){
    // Escalate alarm volume
    if (alrmVol[alrmnum]>0) alrmVol[alrmnum]--;
  }
  else alrmVol[alrmnum]=0;
  wave.volume=alrmVol[alrmnum]; // Set Alarm Volume
  
  // ok time to play! start playback
  wave.play();
//  radio.Wakeup(); // Disable RF12B
  wdt_enable(WDTO_8S); // Re-enable watchdog
}

// =======================================================================================
// ---- Initialize Wave Shield and SD card fucntion   ----
// =======================================================================================
void WaveShieldInit () {
 // ===================================================================
 //putstring_nl("Card init. started");
 // ------ Wave Shield init
 // if (!card.init(true)) { //play with 4 MHz spi if 8MHz isn't working for you
  if (!card.init()) {         //play with 8 MHz spi (default faster!)  
    putstring_nl("Card init. failed!");  // Something went wrong, lets print out why
    sdErrorCheck();
    while(1);                            // then 'halt' - do nothing!
  }
  // enable optimize read - some cards may timeout. Disable if you're having problems
  card.partialBlockRead(true);
 // Now we will look for a FAT partition!
  uint8_t part;
  for (part = 0; part < 5; part++) {     // we have up to 5 slots to look in
    if (vol.init(card, part)) 
      break;                             // we found one, lets bail
  }
  if (part == 5) {                       // if we ended up not finding one  :(
    putstring_nl("No valid FAT partition!");
    sdErrorCheck();      // Something went wrong, lets print out why
    while(1);                            // then 'halt' - do nothing!
  }
   // Lets tell the user about what we found
  //putstring("Using partition ");
  //Serial.print(part, DEC);
 // putstring(", type is FAT");
 // Serial.println(vol.fatType(),DEC);     // FAT16 or FAT32?
  // Try to open the root directory
  if (!root.openRoot(vol)) {
    putstring_nl("Can't open root dir!"); // Something went wrong,
    while(1);                             // then 'halt' - do nothing!
  }
  // Whew! We got past the tough parts.
  //  putstring_nl("Ready!"); 
  
}



