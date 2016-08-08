// =======================================================================================
// ---- Initialize Radio ----
// =======================================================================================
void radioInit() {
 if ( !RFM_Enabled ) return;
 if (!isRadioPresent) return;
 int currFreq;
 char buff[50];
  #if defined (RFM69_CHIP) 
     switch (RF_Frequency) {
      case 0:
        currFreq=RF69_433MHZ;
      break;
      case 1:
        currFreq=RF69_868MHZ;
      break;
      case 2:
        currFreq=RF69_915MHZ;
      break;
      default:
        currFreq=RF69_915MHZ;
      break;
    }
    radio.setCS(18); // Switch CS to 18 (for Xronos 3 PCB)
    radio.initialize(currFreq, RF_Node, RF_Network);
    if (RF_Encrypt) radio.encrypt(KEY);
    radio.sleep();
    sprintf(buff, "%d Mhz. NODE:%d, NET:%d", currFreq==RF69_433MHZ ? 433 : RF_Frequency==RF69_868MHZ ? 868 : 915,RF_Node,RF_Network);
  #else
    switch (RF_Frequency) {
      case 0:
        currFreq=RF12_433MHZ;
        //Serial.println("433");
      break;
      case 1:
        currFreq=RF12_868MHZ;
        //Serial.println("868");
      break;
      case 2:
        currFreq=RF12_915MHZ;
        //Serial.println("915");
      break;
      default:
        currFreq=RF12_915MHZ;
        //Serial.println("915 (default)");
      break;
    }
    /*
    if ( radio.isPresent(RFM_CS_PIN, RFM_IRQ_PIN) )
      Serial.println(F("RFM12B Detected OK!"));
    else
      Serial.println(F("RFM12B Detection FAIL! (is chip present?)"));
      */
   // if (isRadioPresent) radio.Initialize(RF_Node, RF_Frequency, RF_Network);  
    radio.Initialize(RF_Node, currFreq, RF_Network); 
    if (RF_Encrypt) radio.Encrypt(KEY);
    //radio.Sleep();
    sprintf(buff, "%d Mhz. NODE:%d, NET:%d", currFreq==RF12_433MHZ ? 433 : RF_Frequency==RF12_868MHZ ? 868 : 915,RF_Node,RF_Network);
  #endif
  Serial.println (buff);
  //Serial.println (RF_Frequency);//debug
}




// =======================================================================================
// ---- Recieves Temperature from remote RFM transmitter ----
// =======================================================================================
void recieveTemp() {
  if (!isRadioPresent) return;
  if (!radioOn) return; // Means radio disabled for WAV playback
  if (isInMenu) return;
  //Serial.println ("Radio on");
  if (isInQMenu) return;
  //unsigned long tNow = now();
  int iRF,iNow; // Keeps track in minutes when last time RF singal was recieved.
  /*
  if (!radioOn) {
    //radio.WAKEUP(); // Turn on Radio
    radio.receiveDone();
    radioOn=true;
    //putstring_nl ("Radio Wake up!");
  }
  */

 //if (  ((millis()-last_RF) > 900000 ) || last_RF==0) plot (31,0,RED); //Indicate that temp hasn't been recieved in over 15 min
  // Handle RF Indicator LED
  iRF=hour(last_RF)*60 + minute(last_RF); // Get time of last recieved signal in minutes
  iNow=myhours*60 + minute(now()); // Get current time in minutes
  if ((iNow-iRF) > RF_TIMEOUT ) RFRecieved=false; // Didn't recieve signal in RF_TEMOUT minutes
  if (!RFRecieved)plot (31,0,RED); //Indicate that temp hasn't been recieved in some time
  //else if ( (millis()-last_RF) > 5000 ) plot (31,0,BLACK); //Hide indicator
  else  plot (31,0,BLACK); //Hide indicator
  char tempC[3];
  char humidity[3];
  //digitalWrite(SS_SD,HIGH); // Disable SD Card / Audio
  #if defined (RFM69_CHIP) // Version for RFM69
    if (radio.receiveDone())
      {
        //Serial.println (now());
       putstring_nl ("Receiving Data...");
       plot (31,0,GREEN); //Plot Indicator dot (radio signal was recieved)
       if ( (radio.DATALEN) > 2 && (radio.SENDERID==RF_SensorID) ) { // Make sure recieved data is not empty and came from rigth node
        last_RF= now();
        RFRecieved=true;
       memset(RFBuffer, 0, sizeof(RFBuffer)); // Clear Buffer
       for (int i=0; i<radio.DATALEN;i++){ // Fill buffer
              RFBuffer[i]=radio.DATA[i];
              //Serial.print (RFBuffer[i]);
              
            }
            parseSensorData();
            if (radio.ACKRequested())
          {
            radio.sendACK();
            //Serial.print(" - ACK sent");
          }
          }
          /*
         // Debug unknown Transmission
         else {
            Serial.println (F("Unknown Sender/Invalid Data"));
            for (int i=0; i<radio.DATALEN;i++){ // Fill buffer
                RFBuffer[i]=radio.DATA[i];
                Serial.print (RFBuffer[i]);
           }
         }
            */
        delay (10);
        radio.sleep();
        Serial.println ("card init");
        card.init(); // Reset SD Card
        //else
          //Serial.print("BAD-CRC");
      }
      #else // RFM12b
       if (radio.ReceiveComplete())
        {
          if (radio.CRCPass())
          {
            //Serial.println (now());
           putstring_nl ("Receiving Data...");
           //plot (31,0,GREEN); //Plot Indicator dot (radio signal was recieved)
           if (radio.GetDataLen() > 2) { // Make sure recieved data is not empty 
            last_RF= now();
            lastRFEvent=millis();
            RFRecieved=true;
           memset(RFBuffer, 0, sizeof(RFBuffer)); // Clear Buffer
           for (int i=0; i<radio.GetDataLen();i++){ // Fill buffer
                  RFBuffer[i]=radio.Data[i];
                  //Serial.print (RFBuffer[i]);
                  
                }
                parseSensorData();
               if (radio.ACKRequested()) {
                radio.SendACK();
               }
          }
         }
        }
      #endif
}

void parseSensorData() {
   if (RFBuffer[0]!='B') {
     if (RFBuffer[0]=='U') parseSensorUptime(); 
     return;// This is not battery/temperature/humidity data pocket!
   }
   int tempInt;
   float tempF;
   char tempBuff[4];
   char batt[6];
    // Serial.print ("lengh:"); Serial.println (buffL);
    int i=0;
    int j=0;
    while (RFBuffer[i]!='\0') {
    // -----------------------------------------------------
      if (RFBuffer[i]=='B') // Battery voltage string
      {
        i++;// advanced to next letter
        Serial.print (F("Battery:"));
        while (RFBuffer[i]!=',') {
          Serial.print (RFBuffer[i]);
          batt[j]=RFBuffer[i];
          j++;
          i++;
        }
        Serial.println (F("V"));
        batt[j+1]='\0'; // Terminate with null
        sBatt = atol(batt);// Store sensor battery voltage
      }     
      if (RFBuffer[i]=='T') // Temperature string
      {
        i++; // Advance pass "T"
        j=0;
        Serial.print ("Temperature: ");
        while (RFBuffer[i]!=',') { // Read until find comma separator
          //Serial.print (RFBuffer[i]);
          tempBuff[j]=RFBuffer[i];
          i++;
          j++;
        }
        tempBuff[j+1]='\0'; // Terminate with null
        extTemp = atol(tempBuff);// Store External temperature
        Serial.print (extTemp);
        Serial.print ("C/");
        tempF=(atof(tempBuff)*9)/5 + 32; // Convert to Farenheight
        Serial.print ( tempF,1); Serial.println ("F");
    
      }
     
      if (RFBuffer[i]=='H') // Humidity string
      {
        i++; // Advance pass "H"
        j=0;
        Serial.print ("Humidity: ");
        while (RFBuffer[i]!='\0') { // Read until end of string
           Serial.print (RFBuffer[i]);
           tempBuff[j]=RFBuffer[i];
          j++;
          i++;
        }
        Serial.println("");
        tempBuff[j]='\0'; // Terminate with null
        extHum = atol(tempBuff); // Assign external Temp variable Integer number
        Serial.print(extHum);
        Serial.println ("%");
      }
     if (RFBuffer[i]!='\0') i++; // Failsafe so we don't go over end of string
  
    // ------------------------------------------------------
    }
}

void parseSensorUptime() {
   if (RFBuffer[0]!='U') return;// This is not battery/temperature/humidity data pocket!
   Serial.println ("recieving uptime");
   int i=1; // Skipping first character
   while (RFBuffer[i]!='\0') { // just fill uptime buffer
  
      sUptime[i-1]=RFBuffer[i];
      i++;
   }
   sUptime[i]='\0'; // Terminate with null
  
}

// ==================================================
// -- Turns off radio and keeps track of it's status
void turnOffRadio() {
 if (!isRadioPresent) return;
 //radio.Sleep();
 if (radioOn) {
   #if defined (RFM69_CHIP) // Version for RFM69
   EIMSK &= ~(1<<INT2); // Disable External interrupt INT2
   #else 
    radio.Sleep();
   #endif
  radioOn=false;
  putstring_nl ("Radio Off!"); 
  delay (5);
  //detachInterrupt(digitalPinToInterrupt(2));
  //EIMSK &= ~(1<<INT2); // Disable External interrupt INT2
 }
}

// ====================================================================
// -- Re-initializes radio after it was disabled by interrupt function
void reEnableRadio() {
  if (!isRadioPresent) return;
  if (radioOn) return;
  if (isInMenu) return;
  if (soundAlarm[0] || soundAlarm[1]) return;
  if (wave.isplaying) return;
  if (isInQMenu) return;
  #if defined (RFM69_CHIP) // Version for RFM69
    EIMSK |= (1 << INT2);// Enable External interrupt INT2
  #else
     digitalWrite(SS_SD,HIGH); // Disable SD Card / Audio
     radio.Wakeup(); // Turn on Radio
  #endif
  putstring_nl("Re-enable Radio");
  radioOn=true;
  //card.init(); // Reset SD Card
}

