/* Xronos Clock V3 definitions and vars */

//#define RFM69_CHIP // Comment out for RFM12b
#define XRONOS2 // Comment out for Xronos 3

#include <Arduino.h>
#include <avr/pgmspace.h>                // Enable data to be stored in Flash Mem as well as SRAM              
#include <avr/wdt.h>
#include "HT1632.h"                     // Holtek LED driver by WestFW - updated to HT1632C by Nick Hall
#include "WaveUtil.h" // Used by wave shield
#include "WaveHC.h" // Used by wave shield (library modified by LensDigital to accomodate ATMega644p/ATMega1284p)
#include <Wire.h>
#include <Time.h>  
#include <DS1307RTC.h>  // Now supports DS3231 as well
#include <stdlib.h> // Used for string manipulations and string to int conversions
#include <EEPROM.h>
#if defined XRONOS2
  #include <OneWire.h>
  #include <DallasTemperature.h>
#else
  #include <TH02_dev.h>
#endif
#if defined (RFM69_CHIP) 
  #include <RFM69.h>
#else 
  #include <RFM12B_arssi.h>
#endif
#include <SPI.h>
#include <IRremote.h> // Comment out if IR reciever not present
#include <Timezone.h>
#include "myIR_Remote.h" // IR Codes defintion file (comment out if IR reciever not present)

// THO2 Linearization Coefficients
#define TH02_A0   -4.7844
#define TH02_A1    0.4008
#define TH02_A2   -0.00393
// TH02 Temperature compensation Linearization Coefficients
#define TH02_Q0   0.1973
#define TH02_Q1   0.00237

// Wave Shield Declarations
SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the filesystem on the card
FatReader f;      // This holds the information for the file we're play

WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time


// ============================================================================================
// Important User Hardware config settings, modify as needed
// ============================================================================================
static bool RFM_Enabled=true; // Defines if RFM69 Chip present.  Set to true to enable. Must also have ATMega1284p! Will not work with ATMega644p chip
#define AUTO_BRIGHTNESS_ON 0  //Set to 1 to disable autobrightness menu feature, 0 to enable if photocell is present.
static bool IR_PRESENT=true; // Set to True if IR reciever is present. Must also have ATMega1284p! Will not work with ATMega644p chip
#if defined XRONOS2 // Different logic for Xronos 2 button pullups
  #define BUTTON_OFF LOW
  #define BUTTON_ON HIGH  
#else
  #define BUTTON_OFF HIGH 
  #define BUTTON_ON LOW
#endif
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
#define RF_TIMEOUT 15 // Minutes
// ===================================================================================
// Buttons Declarations
// ===================================================================================
#define B_INACTIVE 0 //Descriptive name for array elements
#define B_PRESSED 1 //Descriptive name for array elements
#define B_RELEASED 2 //Descriptive name for array elements
#define B_HELD 3 //Descriptive name for array elements
#define MENU_BUTTON 0
#define SET_BUTTON 1
#define INC_BUTTON 2
#define heldTime 1000 // Time after which button is considered held
#define HOLD_TIME 2000 // 2 seconds concidered a hold
#define debounceInterval 20 //wait 20 ms for button pin to settle\

const char buttonPins[3]={MENU_BUTTON_PIN,SET_BUTTON_PIN,INC_BUTTON_PIN};
bool butCurrentState[3]={BUTTON_OFF,BUTTON_OFF,BUTTON_OFF};
bool butLastState[3]={BUTTON_OFF,BUTTON_OFF,BUTTON_OFF};
bool butDebouncedState[3]={BUTTON_OFF,BUTTON_OFF,BUTTON_OFF};
bool butHeld[3]={false,false,false};
unsigned long butLastTime[3] = {0,0,0};//store the last time the button state changed
unsigned long butHeldTime[3] = {0,0,0};
bool butActivated[3] = {false, false, false}; // Used for holding button for fast incr
char buttonsStatus[3]={0,0,0};
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
// RFM Radio module declarations (Defaults. Oveerwrite from EEPROM )
// ===================================================================================
#if defined (RFM69_CHIP) 
  #define FREQUENCY     RF69_915MHZ 
#else 
  #define FREQUENCY     RF12_915MHZ
#endif
#define RFM_CS_PIN  18 // RFM12B Chip Select Pin
#if defined XRONOS2
  #define RFM_IRQ_PIN 10  // RFM12B IRQ Pin
#else
  #define RFM_IRQ_PIN 2  // RFM12B IRQ Pin
#endif
#define NODEID      1
#define NETWORKID   1
#define GATEWAYID   9
#define KEY         "This is Key" //has to be same 16 characters/bytes on all nodes, not more not less!
//#define LED         9
#define ACK_TIME    30  // # of ms to wait for an ack
// Need an instance of the Radio Module
#define SENSORID 5 // Main outdoor weather sensor ID (other will be ignored
byte RF_Frequency=0;

#if defined (RFM69_CHIP) 
  RFM69 radio;
#else 
  RFM12B radio;
#endif

#ifdef XRONOS2
    //Temperature chip i/o
  OneWire ds(tempPin); // Connect Temperature Sensor
  // Pass our oneWire reference to Dallas Temperature. 
  DallasTemperature sensors(&ds);
  // arrays to hold device address
  DeviceAddress insideThermometer;
#endif
// ===================================================================================
// Menus declarations
// ===================================================================================
#define MAX_MENUS 5 // Maximum number of menus
#define MAX_SUBMENUS 10 // Maximum number of menus
#define TMENU_ITEMS 6 // Number of items in Set Time/Date menu
#define TIME_MENU 2
#define SYS_MENU 3
#define USER_MENU 6
#define SCROLL_MENU 7
#define TALK_MENU 8
#define RF_MENU 9
// ===================================================================================
// Misc Declarations
// ===================================================================================
#define RFTimeout 60 // In minutes

// ===================================================================================
// SAY Options (vocal announcement) bitmask  declarations
// ===================================================================================
const byte SO_Time = B01000000; // Say Time (64)
const byte SO_Date = B00100000; // Say Date (32)
const byte SO_InTemp = B00010000; // Say Inside Temperature (16)
const byte SO_Alarms = B00001000; // Say Alarms (8)
const byte SO_OutTemp = B00000100; // Say Outside Temperature(4)
const byte SO_OutHum = B00000010; // Say Outside Humidity (2)
const byte SO_InHum = B00000001; // Say Inside Humidity (1)
// ===================================================================================
// Info Options (scrolling display) bitmask  declarations
// ===================================================================================
const byte IO_Date =    B10000000; // Scroll Date (128)
const byte IO_InTemp =  B01000000; // Inside Temperature (64)
const byte IO_OutTemp = B00100000; // Outside Temperature (32)
const byte IO_Alarms =  B00010000; // Alarms (16)
const byte IO_RFStat =  B00001000; // RF Statistics (8)
const byte IO_OutHum =  B00000100; // Outside Humidity (4)
const byte IO_InHum =  B00000001; // Outside Humidity (1)

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
unsigned long lastRFEvent = 0;//Last time RF data was recieved

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
uint8_t  extHum=0; // External Humidity
int tempCint; // Internal Temperature in C Intiger
uint8_t humidInt; // Internal Humidity in Integer

byte currStatusInc=BUTTON_OFF; // Current Status of Incremental button
byte lastStatusInc=BUTTON_OFF; // Last Status of Incremental button
boolean buttonReleased=false; 

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
bool rebootPending=false;

const int TZAddr[7]={100,148,196,244,292,340,388}; //TimeZone EEPROM Addresses (EST, CST,MST,Arizona, PST, Alaska, Hawaii)

// EEPROM Location Definitions
#define  mode24HRLoc 0   // 12/24 hour mode storage location
const byte alarmHHLoc[2]={1,9};                        // alarm hours storage location
const byte alarmMMLoc[2]={2,10};              // alarm minutes storage location
const byte alarmOnOffLoc[2]={3,11};           // alarm Off/Daily/Weekday/Custom storage locations
#define brightLoc 4   // Brightness storage location
#define clockColorLoc 5   // Time digit color storage location
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
#define clockVerLoc 25    // Firmware version storage location (2 addresses!)
#define RF_FreqLoc 27 // Frequency
#define RF_NodeLoc 30 // Node ID
#define RF_GatewayLoc 31 // Gateway
#define RF_NetworkLoc 32 // Network ID
#define RF_SensorLoc 33 // Sensor ID
#define RF_EncryptLoc 34 // Encryption on/off

#define currTZLoc 36 // Location of current time zone number (0-6)
// Timezon locations. Each takes 48 Bytes
#define ESTLoc 100  // US Eastern
#define CSTLoc ESTLoc+48 // US Central
#define MSTLoc CSTLoc+48 // US Mountain
#define AZSTLoc MSTLoc+48 // US Arizona
#define PSTLoc AZSTLoc+48 // US Pacific
#define AKSTLoc PSTLoc+48 // US Alaska
#define HSTLoc AKSTLoc+48 // US Hawaii

// Vars that hold Network values from EEPROM

byte RF_Node;
byte RF_Gateway;
byte RF_Network;
byte RF_SensorID; 
boolean RF_Encrypt;

char RFBuffer[14];// Buffer holds data recieved from weather sensor
unsigned int sBatt; // Sensor battery voltage
char sUptime[12];// Sensor uptime string (dd:hh:mm:ss)

// ===================================================================================
// Time Zone definitions
//US Eastern Time Zone (New York, Detroit)
TimeChangeRule usEdt = {"EDT", Second, Sun, Mar, 2, -240};    //UTC - 4 hours
TimeChangeRule usEst = {"EST", First, Sun, Nov, 2, -300};     //UTC - 5 hours
Timezone usEastern(usEdt, usEst);
// US Central Time Zone
TimeChangeRule usCdt = {"CDT", Second, Sun, Mar, 2, -300};    //UTC - 5 hours
TimeChangeRule usCst = {"CST", First, Sun, Nov, 2, -360};     //UTC - 6 hours
Timezone usCentral(usCdt, usCst);
// US Mountain Time Zone
TimeChangeRule usMdt = {"MDT", Second, Sun, Mar, 2, -360};    //UTC - 6 hours
TimeChangeRule usMst = {"MST", First, Sun, Nov, 2, -420};     //UTC - 7 hours
Timezone usMountain(usMdt, usMst);
// US Arizona Time Zone (no DST)
TimeChangeRule usAzdt = {"AZDT", Second, Sun, Mar, 2, -420};    //UTC - 7 hours
TimeChangeRule usAzst = {"AZST", First, Sun, Nov, 2, -420};     //UTC - 7 hours
Timezone usArizona(usAzdt, usAzst);
// US Pacific Time Zone
TimeChangeRule usPdt = {"PDT", Second, Sun, Mar, 2, -420};    //UTC - 7 hours
TimeChangeRule usPst = {"PST", First, Sun, Nov, 2, -480};     //UTC - 8 hours
Timezone usPacific(usPdt, usPst);
// Alaska Time Zone
TimeChangeRule usAkdt = {"AKDT", Second, Sun, Mar, 2, -480};    //UTC - 8 hours
TimeChangeRule usAkst = {"AKST", First, Sun, Nov, 2, -540};     //UTC - 9 hours
Timezone usAlaska(usAkdt, usAkst);
// Hawaii Time Zone (no DST)
TimeChangeRule usHdt = {"HDT", Second, Sun, Mar, 2, -600};    //UTC - 10 hours
TimeChangeRule usHst = {"HST", First, Sun, Nov, 2, -600};     //UTC - 10 hours
Timezone usHawaii(usHdt, usHst);

// Set Timezone (Read from EEPROM)
byte currTZ=EEPROM.read(currTZLoc); // Read Current Timezone Index
//if (currTZ > 7) currTZ=0; // Failsafe in case EEPROM was not initialized
Timezone myTZ(TZAddr[currTZ]);
TimeChangeRule *tcr;        //pointer to the time change rule, use to get TZ abbrev
time_t utc, local;

#define LOCAL_TZ myTZ.toLocal(now(), &tcr) // Conversion to current timezone

