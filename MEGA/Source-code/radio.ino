// =======================================================================================
// ---- Initialize RFM settings ----
// By: LensDigital
// =======================================================================================
void radioInit() {
  if (!isRadioPresent) return;
  int currFreq;
  char buff[50];
  /*if ( radio.isPresent( RFM_CS_PIN, RFM_IRQ_PIN) )
    Serial.println(F("RFM12B Detected OK!"));
  else
    Serial.println(F("RFM12B Detection FAIL! (is chip present?)"));
    */
  switch (RF_Frequency) {
    case 0:
      currFreq=RF12_915MHZ;
    break;
    case 1:
      currFreq=RF12_433MHZ;
    break;
    case 2:
      currFreq=RF12_868MHZ;
    break;
    default:
      currFreq=RF12_915MHZ;
    break;
  }
  radio.Initialize(RF_Node, currFreq, RF_Network);  
  //radio.Initialize(1, RF12_915MHZ, 1);
  sprintf(buff, "%d Mhz. NODE:%d, NET:%d", currFreq==RF12_433MHZ ? 433 : currFreq==RF12_868MHZ ? 868 : 915,RF_Node,RF_Network);
  Serial.println (buff);
}

// =======================================================================================
// ---- Recieves Temperature/Humidity from remote sensor ----
// =======================================================================================
void recieveData() {
  if (!isRadioPresent) return;
  if (isInMenu) return;
  if (!radioOn) {
    radio.Wakeup(); // Turn on Radio
    radioOn=true;
    //putstring_nl ("Radio Wake up!");
  }
  if (  (((unsigned long)millis()-lastRFEvent) > 900000 ) || lastRFEvent==0) indicatorLED (RED_LED); //Indicate that temp hasn't been recieved in over 15 min
  //else if ( ((unsigned long)millis()-lastRFEvent) > 5000 ) plot (31,0,BLACK); //Hide indicator
  else  {
    if ((millis()-last_RF) > 3000 ) {
    indicatorLED (BLACK_LED);
    plot (47,0,GREEN); //Hide indicator
    }
  }
  char tempC[3];
  char humidity[3];
  if (radio.ReceiveComplete())
    {
      if (radio.CRCPass())
      {
        //Serial.println (now());
        Serial.println  (F("Receiving Data..."));
        
        indicatorLED (BLUE_LED);
        //if ((radio.GetDataLen() > 2) && (radio.GetSender()==RF_SensorID)) { // Make sure recieved data is not empty
        if (radio.GetDataLen() > 2) { // Make sure recieved data is not empty
          RFRecieved=true;
          tempUpdated=true;
          humUpdated = true;
          last_RF= now();
          lastRFEvent=millis();
          for (int i=0; i<radio.GetDataLen();i++){ // Fill buffer
            RFBuffer[i]=radio.Data[i];
          }
          Serial.println (RFBuffer);
          parseSensorData();
        }
        if (radio.ACKRequested())
        {
          radio.SendACK();
          Serial.print(" - ACK sent");
        }
      }
      else
        Serial.print("BAD-CRC");
      
      Serial.println();
      
    }
}

void parseSensorData() {
   int tempInt;
   float tempF;
   char tempBuff[4];
   char humBuff[5];
   char batt[6];
    // Serial.print ("lengh:"); Serial.println (buffL);
    int i=0;
    int j=0;
    while (RFBuffer[i]!='\0') {
    // -----------------------------------------------------
      if (RFBuffer[i]=='B') // Battery voltage string
      {
        i++;// advanced to next letter
        j=0;
        Serial.print (F("Battery:"));
        while (RFBuffer[i]!=',') {
          Serial.print (RFBuffer[i]);
          batt[j]=RFBuffer[i];
          j++;
          i++;
        }
        Serial.println (F("V"));
        batt[j]='\0'; // Terminate with null
        sBatt = atol(batt);// Store sensor battery voltage
      }     
      if (RFBuffer[i]=='T') // Temperature string
      {
        i++; // Advance pass "T"
        j=0;
       while (RFBuffer[i]!=',') { // Read until find comma separator
          //Serial.print (RFBuffer[i]);
          tempBuff[j]=RFBuffer[i];
          i++;
          j++;
        }
        //Serial.print ("j:"); Serial.println (j);
        tempBuff[j]='\0'; // Terminate with null
        //Serial.print ("Temp before atol:"); Serial.println (tempBuff);
        Serial.print ("Temperature: ");
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
           humBuff[j]=RFBuffer[i];
           //Serial.println (humBuff[j]);
          j++;
          i++;
        }
        humBuff[j]='\0'; // Terminate with null
        extHum = atol(humBuff); // Assign external Temp variable Integer number
        Serial.print(extHum);
        Serial.println ("%");
      }
     //if (RFBuffer[i]!='\0') i++; // Failsafe so we don't go over end of string
     i++;
    // ------------------------------------------------------
    }
}

// ==================================================
// -- Turns off radio and keeps track of it's status
void turnOffRadio() {
  if (!isRadioPresent) return;
 if (radioOn) {
   radio.Sleep();
   radioOn=false;
  //putstring_nl ("Radio Sleep!"); 
 }
}

// Will transmit text message 
void sendTxtMessage(char *myString, int msgLengh) {
  transmitMsg(myString,msgLengh);
}


// Will transmit integer value message 
void sendIntMessage(int myMsg,int msgLengh) {
  char myString[msgLengh];
  snprintf(myString, msgLengh,"%d",myMsg); 
  transmitMsg(myString,msgLengh);
}

// Will transmit Long integer value message 
void sendFloatMessage(int myMsg,unsigned long msgLengh) {
  char myString[msgLengh];
  snprintf(myString, msgLengh,"%.2f",myMsg); 
  Serial.print ("Message is:"); Serial.println (myString);
  transmitMsg(myString,msgLengh);
}


// Actual function that sends text string
void transmitMsg (char *myString,int msgLengh) {
  
  if (msgLengh>MAX_MSG_SIZE-1) {
    Serial.println ("Error Message exists maximum size");
    return; 
  }
  radio.Wakeup();
  /* Serial.println("Transmitting following message:");
  for (int i=0;i<msgLengh;i++) Serial.print (myString[i]); // Print whole message to serial
  Serial.println (""); // Blank line
  */
  radio.Send(GATEWAYID, myString, msgLengh, requestACK);
  if (requestACK)
  {
    Serial.print(" - waiting for ACK...");
    if (waitForAck()) Serial.print("ok!");
    else Serial.print("nothing...");
  }
  //Serial.println ("Done");
  radio.Sleep();
}

// wait a few milliseconds for proper ACK, return true if received
static bool waitForAck() {
  long now = millis();
  while (millis() - now <= ACK_TIME)
    if (radio.ACKReceived(GATEWAYID))
      return true;
  return false;
}

void getGPSTime() {
  while (Serial.available()) {
    if (gps.encode(Serial.read())) { // process gps messages
      // when TinyGPS reports new data...
      unsigned long age;
      int Year;
      byte Month, Day, Hour, Minute, Second;
      gps.crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, NULL, &age);
      if (age < 500) {
        // set the Time to the latest GPS reading
        setTime(Hour, Minute, Second, Day, Month, Year);
        //adjustTime(offset * SECS_PER_HOUR);
      }
    }
  }
  if (timeStatus()!= timeNotSet) {
    if (now() != prevDisplay) { //update the display only if the time has changed
      prevDisplay = now();
      digitalClockDisplay();  
    }
  }
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
  
  snprintf(myString, 11,"%d-%d-%d",month(),day(),year()); 
  sendTxtMessage (myString,11);
}
void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
