/* Xronos Mega Clock definitions and vars */
#include <Arduino.h>
#include <avr/pgmspace.h>                // Enable data to be stored in Flash Mem as well as SRAM              
#include <avr/wdt.h>
#include "LED_2416.h"                     // Holtek LED driver by WestFW - updated to HT1632C by Nick Hall
#include <Wire.h>
#include <Time.h>  
#include <DS1307RTC.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RFM12B_arssi.h>
#include <IRremote.h> // Comment out if IR reciever not present
#include "myIR_Remote.h" // IR Codes defintion file
#include <TinyGPS.h> 
#include <Timezone.h>



// ============================================================================================
// Importante User Hardware config settings, modify as needed
// ============================================================================================
static boolean RFM12B_Enabled=true; // Defines if RFM12B Chip present.  Set to true to enable. Must also have ATMega1284p! Will not work with ATMega644p chip
#define AUTO_BRIGHTNESS_ON 0  //Set to 1 to disable autobrightness menu feature, 0 to enable if photocell is present.
static boolean IR_PRESENT=true; // Set to True if IR reciever is present. Must also have ATMega1284p! Will not work with ATMega644p chip
// ============================End of User Hardware Settings ==================================

#define SerialGPS Serial


int xpos[2] = {X_MAX,X_MAX}; // Position of X for scrolling msg

char msgbuffer[40];
//String SD_lat = "invalid";


// Offset hours from gps time (UTC)
//const int offset = 1;   // Central European Time
//float UTCoffset = -5;  // Eastern Standard Time (USA)
//const int offset = -4;  // Eastern Daylight Time (USA)
//const int offset = -8;  // Pacific Standard Time (USA)
//const int offset = -7;  // Pacific Daylight Time (USA)

// ----------------------------------------------------------------
// Global Vars for Scrolling text functions
// ----------------------------------------------------------------
//char msgBuffer[20]="No GPS Data"; // Message buffer

bool isScrolling[2] = {false,false};
bool scrollPermit[2] = {false,false};
bool scrollDone[2] ={false,false};
bool okMsg = true;
bool okDate = true;
bool pauseScroll = false;
bool setGPSTime = true; // Initially when clock reboots GPS gets set
char myString[20]; // FOR DEBUGING ONLY

long lat, lon; // Latitude, longitude for sattelite

// Unique to Mega Clock
float tempC;

// Pins Delcarations
// ===================================================================================
#define BOUNCE_TIME_BUTTON  200   // bounce time in ms for the menu button
#define BOUNCE_TIME_QUICK   50  // bounce time in ms for quickMenu
#define tempPin A0 //Pin for temperature sensor DS18B20
#define photoCellPin A1 // Pin for Photo Resistor
#define redLED A7
#define blueLED A6
#define greenLED A5
#define SS_SD 4 // Pin for SS on SD Card
#define SS_RF 18 // Pin for SS on RF12B reciever
#define MENU_BUTTON_PIN A4// "Menu Button" button on analog 4;
#define SET_BUTTON_PIN A3// "set time" button on digital 17 (analog 3);
#define INC_BUTTON_PIN A2// "inc time" button on digital 16 (analog 2);
// ===================================================================================

#define PHOTOCELL_MIN 20 // Minimum reading from Photocell
#define PHOTOCELL_MAX 400 // Maximum reading from Photocell
#define heldTime 1000 // Time after which button is considered held
#define BLACK_LED 0
#define RED_LED 1
#define GREEN_LED 2
#define BLUE_LED 3
#define YELLOW_LED 4 // (Red + Green)
#define MAG_LED 5 // Magenta (Red + Blue)
#define CYA_LED 6 // Cyan (Green + Blue
#define WHITE_LED 7 // Red+Green+Blue

// Days of week bitmask in decimal
// ===================================================================================
#define WEEKDAY 252 // Weekday (Mon-Fri bits are set)
#define DAILY 255 // Daily (Mon-Sun)
#define MON 64 
#define TUE 32 
#define WED 16 
#define THU 8 
#define FRI 4 
#define SAT 2 
#define SUN 1 
// ===================================================================================
// RFM12 Radio module declarations
// ===================================================================================
#define RFM_CS_PIN  18 // RFM12B Chip Select Pin
#define RFM_IRQ_PIN 10  // RFM12B IRQ Pin
#define FREQUENCY RF12_915MHZ // Frequency
#define NODEID           1  //network ID used for this unit
#define NETWORKID       1  //the network ID we are on
#define GATEWAYID     5  //the node ID we're sending to
#define ACK_TIME     50  // # of ms to wait for an ack
#define MAX_MSG_SIZE 40 // Maximum message size in characters
#define SENSORID 5 // Main outdoor weather sensor ID (other will be ignored)
bool requestACK=false;
// Vars that hold Network values from EEPROM
byte RF_Frequency=0; // 0=915Mhz, 1=433Mhz, 2=868Mhz
byte RF_Node;
byte RF_Gateway;
byte RF_Network;
byte RF_SensorID; 
boolean RF_Encrypt;

// ===================================================================================
// Menus declarations
// ===================================================================================
#define MAX_MENUS 3 // Maximum number of menus
#define MAX_SUBMENUS 5 // Maximum number of menus
#define TMENU_ITEMS 7 // Number of items in Set Time/Date menu
#define SYSMENU_ITEMS 9 // Number of items in System Menu (1=Mode, 2=Brightness, 3=Font, 4=Temp, 5=RFM, 6=LED, 7=GPS, 8=Reset)

#define TIME_MENU 0
#define SYS_MENU 1
#define RF_MENU 2
#define USER_MENU 3
#define SCROLL_MENU 4

#define RFTimeout 60 // In minutes
#define GPSTimeout 1500 // In minutes

byte menuItem=0; // Counts presses of the Set button 
byte subMenu[MAX_SUBMENUS]={0,0,0,0,0}; // 0 = setting Time/Date, 1 = System Settings, 2 = RF Options, 3 = UserOptions, 4= Infodisplay Options

// ===================================================================================
// Info Options (scrolling display) bitmask  declarations
// ===================================================================================
const byte IO_Date =    B10000000; // Scroll Date (128)
const byte IO_InTemp =  B01000000; // Inside Temperature (64)
const byte IO_OutTemp = B00100000; // Outside Temperature (32)
const byte IO_OutHum =  B00010000; // Outside Humidity (16)
const byte IO_GPSStat =  B00001000; // GPS Statistics (8)

bool isInMenu=false;

bool isSettingTime = false;
//bool isInMenu = false; // Defined in very beginning. Used by LED Scrolling functions
bool isInQMenu = false; // Are we in Quick menu now?
bool isSettingDate = false;
bool isSettingAlarm = false;
bool isSettingSys = false;
bool isSettingHours   = false;
bool isSettingMinutes = false;
bool isSettingSeconds = false;
bool isSettingMonth   = false;
bool isSettingDay   = false;
bool isSettingYear   = false;
bool isSettingTZ = false;
bool isSettingAlrmMM   = false;
bool isSettingAlrmHH   = false;
bool isSettingAlrmCust[2]; // Use to track custom alarm schedule setting
bool isAlarmModified[2]={false,false}; // If hh:mm of alarm was changed, settings will be written to EEPROM
bool isSettingOptions = false;
bool okClock = true; // Can we show time? Normally true, unless we showing something else
bool time12hr; // Showing time in 24 or 12 hr format
bool interruptAlrm[2] = {false,false};
bool soundAlarm[2] = {false,false};
bool interruptAlrm2 = false;
bool soundAlarm2 = false;
bool isIncrementing = false;
bool blinking=false;
bool sFX=true; // Menu Effects on/off
bool buttonPressedInc=false; // Tracks High state of INC button
bool doStartup; // Startup sequence enable/disable
bool radioOn=true;
bool isRadioPresent; // Defines in RFM12B Chip present. Stored in EEPROM
boolean isIRPresent; // Defines if IR reciever is present
bool decrement; // Only used with IR remote to decrement digits (--)
bool irBlink=false; // For blinking indicator during IR receipt
bool LEDEnabled; // Controls Indicator RGB LED 
bool GPSEnabled = false;
bool rebootPending = false;
bool GPSAlive = false;

char alrmHH[2]; // Alarm Hours
char alrmMM[2]; // Alarm Minutes
char alarmon[2]; // Alarm Freq. Controlled by 8 bits. If first bit is 0 alarm is off. Example in in decimal (not counting 1st bit): Mon=64,Tue=32,Wed=16,Thu=8,Fri=4,Sat=2,Sun=1,Daily =127,Weekday on =124,Custom=126
char alrmVol[2]={7,7}; // Alarm Volume (0-12, smaller = louder)
char sndVol=0; //Normal Sounds volume (0-12, smaller = louder)
char alrmToneNum[2]; // Number of alarm tone
char tmpOffset; // temperature offset (minus)

//const char weekdays[8]={0,1,64,32,16,8,4,2}; // Lookup table to convert Weekday number to my day code used for Custom Alarm schedule

const int TZAddr[7]={100,148,196,244,292,340,388}; //TimeZone EEPROM Addresses (EST, CST,MST,Arizona, PST, Alaska, Hawaii)

unsigned long blinkTime=0; // controls blinking of the dots
unsigned long last_ms=0; // for setting seconds, etc.
unsigned long last_RF=millis(); // Keeps track since last RF signal recieved
volatile unsigned long lastButtonTime = 0;// last time a button was pushed; used for debouncing
unsigned long lastRFEvent = 0;//Last time RF data was recieved
unsigned long lastGPSEvent = 0;//Last time GPS data was recieved

byte clockColor;
byte clockFont; 
byte blinkColor=BLACK; // Default off
byte hhColor=BLACK; // Set color of the 2 hour digits
byte mmColor=BLACK; // Set color of the 2 minute digits
byte ssColor=BLACK;
byte monColor=BLACK; // Set color of the month
byte ddColor=BLACK; // Set color of the day
byte yyColor=BLACK; // Set color of the year (last 2 digits)
byte dateColor=BLACK; // Set color of the date
byte alrmonColor=BLACK; // Set color of the 2 hour digits
byte alrmhhColor=BLACK; // Set color of the 2 hour digits
byte alrmmmColor=BLACK; // Set color of the 2 minute digits
byte miscColor=BLACK; // Blinking of other parameters like yes/now, etc.
byte snoozeTime[2]={10,10}; // Keeps last digit of minutes for snooze
int extTemp=300; // External Temperature in C
int  extHum=300; // External Humidity
byte timestamp[3]; // Stores time hhmmss

byte currStatusInc=LOW; // Current Status of Incremental button
byte lastStatusInc=LOW; // Last Status of Incremental button
boolean buttonReleased=false; 

byte hours;
byte myhours; // used for 12/24 conversion
byte minutes;
byte seconds;
//int pSeconds; // Precise seconds
byte months;
byte days;
byte years; //Last 2 digits of a year
byte mbutState=1; // Menu button option 
byte brightness; // LED Display Brightness
byte lightLevel; // Light level from photocensor
byte prevBrightness =0; // Previous Brightness (to detect brightness level change)
boolean tempUnit; // Temperature units (True=F or False=C)
byte infoFreq; // Info Display Freq options
byte sayOptions;  // Say items options
byte infoOptions;  // Info Display items options
byte screenInUse=0; // Makes screens to take turns
//byte currTZ=0; // Current Time Zone (Default EST)

// EEPROM Location Definitions
#define  mode24HRLoc 0   // 12/24 hour mode storage location
//const byte alarmHHLoc[2]={1,9};                        // alarm hours storage location
//const byte alarmMMLoc[2]={2,10};              // alarm minutes storage location
//const byte alarmOnOffLoc[2]={3,11};           // alarm Off/Daily/Weekday/Custom storage locations
#define brightLoc 4   // Brightness storage location
#define clockColorLoc 5   // Time digit color storage location
#define clockVerLoc 6   // Firmware version storage location
//const byte alarmToneLoc[2]={7,12};                    // Alarm  Tone storage location
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
#define LEDEnabledLoc 23          // Defines if RF reciever is enabled
#define currTZLoc 24 // Location of current time zone number (0-6)
#define GPSEnabledLoc 25          // Defines if GPS reciever is enabled
#define RF_FreqLoc 27 // Frequency
#define RF_NodeLoc 30 // Node ID
#define RF_GatewayLoc 31 // Gateway
#define RF_NetworkLoc 32 // Network ID
#define RF_SensorLoc 33 // Sensor ID
#define RF_EncryptLoc 34 // Encryption on/off
#define IROnLoc 36          // Defines if IR reciever is enabled
// Timezon locations. Each takes 48 Bytes
#define ESTLoc 100  // US Eastern
#define CSTLoc ESTLoc+48 // US Central
#define MSTLoc CSTLoc+48 // US Mountain
#define AZSTLoc MSTLoc+48 // US Arizona
#define PSTLoc AZSTLoc+48 // US Pacific
#define AKSTLoc PSTLoc+48 // US Alaska
#define HSTLoc AKSTLoc+48 // US Hawaii
#define UTCLoc HSTLoc+48 // US Hawaii

char RFBuffer[14];// Buffer holds data recieved from weather sensor

RFM12B radio;

TinyGPS gps;
// Used by GPS Time debug (remove please!)
time_t prevDisplay = 0; // when the digital clock was displayed

// ===================================================================================
// Time Zone definitions
// UTC not DST
TimeChangeRule UTC = {"UTC", Second, Sun, Mar, 2, 0};    //UTC - 4 hours
//TimeChangeRule usUTC = {"EST", First, Sun, Nov, 2, -300};     //UTC - 5 hours
Timezone Universal(UTC, UTC);
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
// ===================================================================================


//Temperature chip i/o
OneWire ds(tempPin); // Connect Temperature Sensor
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&ds);
// arrays to hold device address
DeviceAddress insideThermometer;
