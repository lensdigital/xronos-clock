
// =======================================================================================
// ---- Recieves Temperature from remote RF12B transmitter ----
// =======================================================================================
void recieveTemp() {
  if (!isRadioPresent) return;
  if (isInMenu) return;
  //Serial.println ("Radio on");
  if (soundAlarm[0] || soundAlarm[1]) return;
  if (wave.isplaying) return;
  //if (isInQMenu) return;
  //unsigned long tNow = now();
  int iRF,iNow; // Keeps track in minutes when last time RF singal was recieved.
  if (!radioOn) {
    radio.Wakeup(); // Turn on Radio
    radioOn=true;
    //putstring_nl ("Radio Wake up!");
  }
  //if (  ((millis()-last_RF) > 900000 ) || last_RF==0) plot (31,0,RED); //Indicate that temp hasn't been recieved in over 15 min
  // Handle RF Indicator LED
  iRF=hour(last_RF)*60 + minute(last_RF); // Get time of last recieved signal in minutes
  iNow=hour(now())*60 + minute(now()); // Get current time in minutes
  if ((iNow-iRF) > RF_TEMOUT ) RFRecieved=false; // Didn't recieve singla in RF_TEMOUT minutes
  if (!RFRecieved)plot (31,0,RED); //Indicate that temp hasn't been recieved in some time
  //else if ( (millis()-last_RF) > 5000 ) plot (31,0,BLACK); //Hide indicator
  else  plot (31,0,BLACK); //Hide indicator
  char tempC[3];
  char humidity[3];
  digitalWrite(SS_SD,HIGH); // Disable SD Card / Audio
  if (radio.ReceiveComplete())
    {
       if (radio.CRCPass())
      {
        //Serial.println (now());
        putstring_nl ("Receiving Data...");
        last_RF= now();
        RFRecieved=true;
        plot (31,0,GREEN); //Plot Indicator dot (radio signal was recieved)
        if (radio.GetDataLen() > 2) { // Make sure recieved data is not empty
          for (int i=0; i<radio.GetDataLen();i++){ // Fill buffer
            RFBuffer[i]=radio.Data[i];
            
          }
          parseSensorData();
        }
        //Serial.print ("Temp: "); Serial.println (extTemp);
        //Serial.print ("Humidity: "); Serial.println (extHum);
       
        if (radio.ACKRequested())
        {
          radio.SendACK();
          //Serial.print(" - ACK sent");
        }
      }
      //else
        //Serial.print("BAD-CRC");
      
      //Serial.println();
      
    }
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
          // Serial.print (RFBuffer[i]);
           tempBuff[j]=RFBuffer[i];
          j++;
          i++;
        }
        tempBuff[j+1]='\0'; // Terminate with null
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
   radio.Sleep();
   radioOn=false;
   //putstring_nl ("Radio Sleep!"); 
  delay (5);
 }
}


