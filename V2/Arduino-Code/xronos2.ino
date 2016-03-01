/***********************************************************************
* July 11, 2013 LensDigital 
* Ver 2.14 (06/22/2015): Now works witn Arduino IDE 1.6.5! (Changed font3.h vars to cons)
* Ver 2.15 (09/03/2015): Fixed serious bug with Say Menu settings not working. 
* Ver 2.16 (12/27/2015): Removed sensor uptime message
* DS3231 edition (12/20/2015): Support of DS3231 RTC Chip
* See www.xronosclock.com for project details, schematics, etc.
* NOTE: By default RFM12B is disabled. If you have one onboard, chnage RFM12B_Enabled=false to RFM12B_Enabled=true, upload. Then change setting in System Menu (Set RFM12 to YES) and reboot clock).
* NOTE: With IRRemote library ATMega644p based clock doesn't have enough RAM to run. Comment out all IR related code to run on ATmega644p chip.
**********************************************************************/
/***********************************************************************
 * Scrolling  fumction by Bill Ho 
 * adapted by roomtek http://code.google.com/p/sure-green-matrix/
 *   Copyrighted and distributed under the terms of the Berkely license
 *   (copy freely, but include this notice of original author.)
 ***********************************************************************/
#include <Wire.h>
#include "HT1632.h"
#include <avr/pgmspace.h>
#include <Time.h>  
//#include <DS3231RTC.h>  // a basic DS3231 library that returns time as a time_t
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t
#include <stdlib.h> // Used for string manipulations and string to int conversions
#include "WaveUtil.h" // Used by wave shield
#include "WaveHC.h" // Used by wave shield (library modified by LensDigital to accomodate ATMega644p/ATMega1284p)
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RFM12B.h>
#include <IRremote.h> // Comment out if IR reciever not present
#include "myIR_Remote.h" // IR Codes defintion file (comment out if IR reciever not present)

#define firmware_ver 216 // Current Firmware version


// ============================================================================================
// Importante User Hardware config settings, modify as needed
// ============================================================================================
static boolean RFM12B_Enabled=true; // Defines if RFM12B Chip present.  Set to true to enable. Must also have ATMega1284p! Will not work with ATMega644p chip
#define AUTO_BRIGHTNESS_ON 0  //Set to 1 to disable autobrightness menu feature, 0 to enable if photocell is present.
static boolean IR_PRESENT=true; // Set to True if IR reciever is present. Must also have ATMega1284p! Will not work with ATMega644p chip
// ============================End of User Hardware Settings ==================================

// Pins Delcarations
// ===================================================================================
#define BOUNCE_TIME_BUTTON  200   // bounce time in ms for the menu button
#define BOUNCE_TIME_QUICK   50  // bounce time in ms for quickMenu
#define tempPin A0 //Pin for temperature sensor DS18B20
#define photoCellPin A1 // Pin for Photo Resistor
#define SS_SD 4 // Pin for SS on SD Card
//#define SS_RF 18 // Pin for SS on RF12B reciever
#define MENU_BUTTON_PIN A4// "Menu Button" button on analog 4;
#define SET_BUTTON_PIN A3// "set time" button on digital 17 (analog 3);
#define INC_BUTTON_PIN A2// "inc time" button on digital 16 (analog 2);
#define PHOTOCELL_MIN 200 // Minimum reading from Photocell
#define PHOTOCELL_MAX 880 // Maximum reading from Photocell
#define L_READING_FREQ 1000 // Take light reading every xx milliseconds
#define RF_TEMOUT 120 // Minutes
// ===================================================================================

#define heldTime 1000 // Time after which button is considered held

// ===================================================================================
// Alarm Days of week bitmask 
// ===================================================================================
const byte ALRM_TGL = B10000000; // 128 Alarm toggle on/off
const byte WEEKDAY = B11111100; //252  Weekday (Mon-Fri bits are set)
const byte DAILY =   B11111111; //255  Daily (Mon-Sun)
const byte CUSTOM =  B11111110; //254 Custom alarm
const byte MON =     B01000000;//64 
const byte TUE =     B00100000; //32 
const byte WED =     B00010000; //16 
const byte THU =     B00001000; //8 
const byte FRI =     B00000100; //4 
const byte SAT =     B00000010; //2 
const byte SUN =     B00000001; // 1
// ===================================================================================
// RFM12 Radio module declarations
// ===================================================================================
#define NODEID           1  //network ID used for this unit
#define NETWORKID       1  //the network ID we are on
// Need an instance of the Radio Module
RFM12B radio;
// ===================================================================================
// Menus declarations
// ===================================================================================
#define MAX_MENUS 5 // Maximum number of menus
#define MAX_SUBMENUS 9 // Maximum number of menus
#define TIME_MENU 2
#define SYS_MENU 3
#define USER_MENU 6
#define SCROLL_MENU 7
#define TALK_MENU 8

// ===================================================================================
// SAY Options (vocal announcement) bitmask  declarations
// ===================================================================================
const byte SO_Time = B01000000; // Say Time (64)
const byte SO_Date = B00100000; // Say Date (32)
const byte SO_InTemp = B00010000; // Say Inside Temperature (16)
const byte SO_Alarms = B00001000; // Say Alarms (8)
const byte SO_OutTemp = B00000100; // Say Outside Temperature(4)
const byte SO_OutHum = B00000010; // Say Outside Humidity (2)
// ===================================================================================
// Info Options (scrolling display) bitmask  declarations
// ===================================================================================
const byte IO_Date =    B10000000; // Scroll Date (128)
const byte IO_InTemp =  B01000000; // Inside Temperature (64)
const byte IO_OutTemp = B00100000; // Outside Temperature (32)
const byte IO_Alarms =  B00010000; // Alarms (16)
const byte IO_RFStat =  B00001000; // RF Statistics (8)
const byte IO_OutHum =  B00000100; // Outside Humidity (4)


boolean isSettingTime = false;
boolean isInMenu = false;
boolean isInQMenu = false; // Are we in Quick menu now?
boolean isSettingDate = false;
boolean isSettingAlarm = false;
boolean isSettingSys = false;
boolean isSettingHours   = false;
boolean isSettingMinutes = false;
boolean isSettingMonth   = false;
boolean isSettingDay   = false;
boolean isSettingYear   = false;
boolean isSettingDST = false; 
boolean isSettingAlrmMM   = false;
boolean isSettingAlrmHH   = false;
boolean isSettingAlrmCust[2]; // Use to track custom alarm schedule setting
boolean isAlarmModified[2]={false,false}; // If hh:mm of alarm was changed, settings will be written to EEPROM
boolean isSettingOptions = false;
boolean okClock = true; // Can we show time? Normally true, unless we showing something else
boolean time12hr; // Showing time in 24 or 12 hr format
boolean interruptAlrm[2] = {false,false};
boolean soundAlarm[2] = {false,false};
boolean interruptAlrm2 = false;
boolean soundAlarm2 = false;
boolean isIncrementing = false;
boolean blinking=false;
boolean sFX=true; // Menu Effects on/off
boolean buttonPressedInc=false; // Tracks High state of INC button
boolean doStartup; // Startup sequence enable/disable
boolean radioOn=true;
boolean isRadioPresent; // Defines in RFM12B Chip present. Stored in EEPROM
boolean isIRPresent; // Defines if IR reciever is present
boolean decrement; // Only used with IR remote to decrement digits (--)
boolean RFRecieved=false; // Semaphor that keeps track if no IR recieved in certain time


byte alrmHH[2]; // Alarm Hours
byte alrmMM[2]; // Alarm Minutes
byte alarmon[2]; // Alarm Freq. Controlled by 8 bits. If first bit is 0 alarm is off. Example in in decimal (not counting 1st bit): Mon=64,Tue=32,Wed=16,Thu=8,Fri=4,Sat=2,Sun=1,Daily =127,Weekday on =124,Custom=126
byte alrmVol[2]={7,7}; // Alarm Volume (0-12, smaller = louder)
byte sndVol=0; //Normal Sounds volume (0-12, smaller = louder)
byte alrmToneNum[2]; // Number of alarm tone
byte tmpOffset; // temperature offset (minus)

const byte weekdays[8]={0,1,64,32,16,8,4,2}; // Lookup table to convert Weekday number to my day code used for Custom Alarm schedule

unsigned long blinkTime,alarmBlinkTime=0; // controls blinking of the dots
unsigned long last_ms=0; // for setting seconds, etc.
unsigned long last_RF=0; // Keeps track since last RF signal recieved
static unsigned long lastLReading = 0; // Keeps track of last time Autobrightness took reading
volatile unsigned long lastButtonTime = 0;// last time a button was pushed; used for debouncing

byte clockColor;
byte alarmColor;
byte clockFont; 
byte blinkColor=BLACK; // Default off
byte hhColor=BLACK; // Set color of the 2 hour digits
byte mmColor=BLACK; // Set color of the 2 minute digits
byte monColor=BLACK; // Set color of the month
byte ddColor=BLACK; // Set color of the day
byte yyColor=BLACK; // Set color of the year (last 2 digits)
byte dateColor=BLACK; // Set color of the date
byte alrmonColor=BLACK; // Set color of the 2 hour digits
byte alrmhhColor=BLACK; // Set color of the 2 hour digits
byte alrmmmColor=BLACK; // Set color of the 2 minute digits
byte snoozeTime[2]={10,10}; // Keeps last digit of minutes for snooze
int  extTemp=300; // External Temperature in C
int  extHum=300; // External Humidity

byte currStatusInc=LOW; // Current Status of Incremental button
byte lastStatusInc=LOW; // Last Status of Incremental button
boolean buttonReleased=false; 

//Temperature chip i/o
OneWire ds(tempPin); // Connect Temperature Sensor
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&ds);
// arrays to hold device address
DeviceAddress insideThermometer;

byte hours;
byte myhours; // used for 12/24 conversion
byte minutes;
byte seconds;
//int pSeconds; // Precise seconds
byte months;
byte days;
byte years; //Last 2 digits of a year
byte menuItem=0; // Counts presses of the Set button 
byte mbutState=1; // Menu button option 
byte subMenu[MAX_SUBMENUS]={0,0,0,0,0,0,0,0,0}; // 0 = setting Alarm1, 1 = setting Alarm 2, 2 for setting Time/Date, 3 for System Settings, 4 for setting custom alrm 1, 5 = custom alarm 2, 6 = UserOptions, 7= Infodisplay options, 8 = Voice Prompts
byte brightness; // LED Display Brightness
byte lightLevel; // Light level from photocensor
byte prevBrightness =0; // Previous Brightness (to detect brightness level change)
boolean tempUnit; // Temperature units (True=F or False=C)
byte infoFreq; // Info Display Freq options
byte sayOptions;  // Say items options
byte infoOptions;  // Info Display items options

// EEPROM Location Definitions
#define	mode24HRLoc 0		// 12/24 hour mode storage location
const byte alarmHHLoc[2]={1,9};                        // alarm hours storage location
const byte alarmMMLoc[2]={2,10};           		// alarm minutes storage location
const byte alarmOnOffLoc[2]={3,11};		        // alarm Off/Daily/Weekday/Custom storage locations
#define	brightLoc 4		// Brightness storage location
#define	clockColorLoc 5		// Time digit color storage location
const byte alarmToneLoc[2]={7,12};                    // Alarm  Tone storage location
#define clockFontLoc 8               // Clock's font number location
#define sFXLoc 13               // Menu SFX on/off location
#define tempUnitLoc 14               // Degreen Units location
#define sndVolLoc 15               // Sound Volume control location
#define infoFreqLoc 16               // Info Display frequency and what to show
#define sayOptionsLoc 17               // Define which options to say
#define doStartupLoc 18               
#define tmpOffsetLoc 20              // Temperature Offset sotrage location
#define infoOptionsLoc 21               // Info Display frequency and what to show
#define radioOnLoc 22          // Defines if RF reciever is enabled
#define IROnLoc 23          // Defines if IR reciever is enabled
#define	clockVerLoc 25		// Firmware version storage location (2 addresses!)

// Wave Shield Declarations
SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the filesystem on the card
FatReader f;      // This holds the information for the file we're play

WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time
// we will track if a button is just pressed, just released, or 'pressed' (the current state
//volatile byte pressed[2], justpressed[2], justreleased[2];
char RFBuffer[14];// Buffer holds data recieved from weather sensor
unsigned int sBatt; // Sensor battery voltage
char sUptime[12];// Sensor uptime string (dd:hh:mm:ss)

void setup ()  
{
  Serial.begin(115200);
  //initEPROM();
  
  // Print FW Version
    char welcome[15];
    int ver=EEPROMReadInt (clockVerLoc); // Read 3 digit version number
    byte first=ver/100; // Thousand
    byte scnd =(ver%100)/10;
    byte third=ver%10;
    snprintf(welcome, sizeof(welcome),"Firmware:V%d.%d%d",first,scnd,third); 
    Serial.println (welcome);
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if(timeStatus()!= timeSet) 
     putstring_nl("Unable to sync with the RTC");
  else
     putstring_nl("RTC has set the system time");    
  ht1632_setup();  // Setup LED Deisplay
  // Uncomment following two lines and modify to set time. After setting time, commend them and re-upload sketch
  //setTime(8,45,0,20,12,15); // Set time on Arduino (hr,min,sec,day,month,yr). Use with RTC.set(now());
  //RTC.set(now()); // Write Time data to RTC Chip. Use with previous command
   // ========= Read Settings from EEPROM ===============================
  getEEPROMSettings ();
  wave.volume=sndVol; // Change System Sound Volume
  WaveShieldInit();
  TempInit();
 // Set initial brightness
 if (brightness==0) autoBrightness();
 else
  setBrightness(brightness);
 // Initialize Radio module 
 if (isRadioPresent) radio.Initialize(NODEID, RF12_915MHZ, NETWORKID);  
 if (isIRPresent)  irrecv.enableIRIn(); // Start the IR receiver. Comment out if IR not present
  startup(); // Show welcoming screen
 //Serial.println (FreeRam());
 //radio.Sleep();
}



// =====================================================================================
// >>>>>>>>>>>>>>>>>>>>>>>>> MAIN LOOP <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// =====================================================================================
void loop ()
{
 showBigTime(clockColor);
  procAlarm(0);
  procAlarm(1);
  buttonProc();
  quickMenu();
  rearmAlrm(0);
  rearmAlrm(1);
  infoDisplay();
  autoBrightness ();
  recieveTemp();
  IR_process();
  
}
// =====================================================================================
// >>>>>>>>>>>>>>>>>>>>>>>>> END MAIN LOOP <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// =====================================================================================

/*
void myDebug(){
  //Serial.print ("soundAlarm=");Serial.println(soundAlarm);
   //Serial.print ("interruptAlarm=");Serial.println (interruptAlrm); 
  // Serial.print ("SnozeTime="); Serial.println (snoozeTime); 
}



// Better (low memory) version of Serial print (by David Pankhurst)
// * http://www.utopiamechanicus.com/article/low-memory-serial-print/
void StreamPrint_progmem(Print &out,PGM_P format,...)
{
  // program memory version of printf - copy of format string and result share a buffer
  // so as to avoid too much memory use
  char formatString[128], *ptr;
  strncpy_P( formatString, format, sizeof(formatString) ); // copy in from program mem
  // null terminate - leave last char since we might need it in worst case for result's \0
  formatString[ sizeof(formatString)-2 ]='\0';
  ptr=&formatString[ strlinfoFreqen(formatString)+1 ]; // our result buffer...
  va_list args;
  va_start (args,format);
  vsnprintf(ptr, sizeof(formatString)-1-strlen(formatString), formatString, args );
  va_end (args);
  formatString[ sizeof(formatString)-1 ]='\0';
  out.print(ptr);
}

#define Serialprint(format, ...) StreamPrint_progmem(Serial,PSTR(format),##__VA_ARGS__)
#define Streamprint(stream,format, ...) StreamPrint_progmem(stream,PSTR(format),##__VA_ARGS__)

*/

// ===================================================================
// * DS18B20 Temperature sensor iniitialization *
// ===================================================================
void TempInit(){
 // ===================================================================
  // Temperature sensor init
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
  
}



// ===================================================================
// * READ EEPROM Settings *
// ===================================================================
void getEEPROMSettings () {
  byte alrmTst;
  for (byte i=0;i<2;i++) {
    alarmon[i]=EEPROM.read(alarmOnOffLoc[i]); // read Alarm off/Daily/Weekday from EEPROM
    alrmHH[i]=EEPROM.read(alarmHHLoc[i]);  // Read Alarm Hours from EEPROM
    alrmMM[i]=EEPROM.read(alarmMMLoc[i]); // Read Alarm Minutes from EEPROM
    alrmToneNum[i]=EEPROM.read(alarmToneLoc[i]); // Read Alarm Tone number from EEPROM
    // Check if custom alarm schdule is set
    alrmTst=alarmon[i]<<1; // Shift left one to get rid of 1st bit
    alrmTst=alrmTst>>1; // Shift right one to get rid of 1st bit
    if ( (alrmTst == 124) || (alrmTst == 127) ) isSettingAlrmCust[i]=false; // i.e. Daily or Weekday alarm is set
    else isSettingAlrmCust[i]=true; // It's set to custom day
  }
  time12hr=EEPROM.read(mode24HRLoc); // Read time mode (12/24 hours) from EEPROM
  brightness=EEPROM.read(brightLoc); // Read Brightness setting from EEPROM
  clockColor=EEPROM.read(clockColorLoc); // Read Clock Color from EEPROM
  if (clockColor!=RED && clockColor!=GREEN && clockColor!=ORANGE) clockColor=ORANGE;// Failsafe for when EEPROM location is blank or corrupted, so clock will be visible
  clockFont=EEPROM.read(clockFontLoc); // Read Alarm Tone number from EEPROM
  sFX=EEPROM.read(sFXLoc); // Read Sound FX on/of for menu system
  tempUnit=EEPROM.read(tempUnitLoc); // Read temp unit
  sndVol=EEPROM.read(sndVolLoc); // Read Sound Volume
  infoFreq=EEPROM.read(infoFreqLoc); // Read infodisplay options
  infoOptions=EEPROM.read(infoOptionsLoc); // Read infodisplay options
  sayOptions=EEPROM.read(sayOptionsLoc); // Read say prompt options
  doStartup=EEPROM.read(doStartupLoc); // Read say prompt options
  tmpOffset=EEPROM.read (tmpOffsetLoc); // Read Temperature offset
  if ( RFM12B_Enabled ) isRadioPresent=EEPROM.read (radioOnLoc);
  else isRadioPresent=false;
  
  if (!isRadioPresent) { // Disable saying ext temperature/humidity
    if (sayOptions & SO_OutTemp) sayOptions = sayOptions ^ SO_OutTemp;
    if (sayOptions & SO_OutHum) sayOptions = sayOptions ^ SO_OutHum;
  }
  if (EEPROMReadInt (clockVerLoc) != firmware_ver) { //Write current firmware version to EEPROM if it's different from stored
    putstring_nl("Writing new Fw version");
    EEPROMWriteInt (clockVerLoc,firmware_ver); 
    delay (15); 
  }
  if ( IR_PRESENT ) isIRPresent=EEPROM.read (IROnLoc); // IR reciever setting
  else isIRPresent=false;
}

// ===================================================================
// * Decode IR Codes *
// Comment out if IR reciever not present
// ===================================================================
void IR_process () {
  if (!isIRPresent) return;
  if (irrecv.decode(&results)) {
    switch (results.value) {
      case IR_ON:
      //Serial.println ("Recieved ON/OFF");
      lastButtonTime=millis()+ BOUNCE_TIME_BUTTON;
      processMenuButton();
      break;
      case IR_PLUS:
      //Serial.println ("Recieved PLUS");
      decrement=false;
      lastButtonTime=millis()+ BOUNCE_TIME_BUTTON;
      processIncButton();
      break;
      case IR_MINUS:
      //Serial.println ("Recieved MINUS");
      decrement=true;
      lastButtonTime=millis()+ BOUNCE_TIME_BUTTON;
      processIncButton();
      break;
      case IR_UP:
      //Serial.println ("Recieved UP");
      lastButtonTime=millis()+ BOUNCE_TIME_BUTTON;
      processSetButton();
      break;
      case IR_DOWN:
      //Serial.println ("Recieved DOWN");
      lastButtonTime=millis()+ BOUNCE_TIME_BUTTON;
      processSetButton();
      break;
      case IR_ENTER: // Talk All Items
      //Serial.println ("Recieved ENTER");
      lastButtonTime=millis()+BOUNCE_TIME_QUICK;
      buttonReleased=true;
      last_ms=millis()+heldTime;
      quickDisplay();
      break;
      case IR_TALK: // Start Talk function
      //Serial.println ("Recieved MUTE");
      lastButtonTime=millis()+BOUNCE_TIME_QUICK;
      buttonReleased=true;
      last_ms=millis();
      quickDisplay();
      
      break;

    //Serial.println(results.value, HEX);
    
    }
    irrecv.resume(); // Receive the next value
  }
}

