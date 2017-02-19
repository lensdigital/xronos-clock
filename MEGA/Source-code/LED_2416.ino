//#include <pins_arduino.h>
#include "LED_2416.h"
#include "font3.h"
#include "fontBig.h"
#include "fontSmall.h"
#include "fontTiny.h"

//The pins for connecting to led boards.
#define HT1632_DATA 14 // Data pin for sure module
#define HT1632_WRCLK      12    // Write clock pin (pin 5)
static const byte ht1632_cs[4] = {13,15,19,20};  // Chip_selects one for each sure module. Remember to set the DIP switches on the modules too.

//Atmega644/1284 Version of fastWrite - for pins 0-15
#define fWriteA(_pin_, _state_) ( _pin_ < 8 ? (_state_ ? PORTB |= 1 << _pin_ : \
PORTB &= ~(1 << _pin_ )) :  (_state_ ? PORTD |= 1 << (_pin_ -8) : PORTD &= ~(1 << (_pin_ -8) )))
//Atmega644/1284 Version of fastWrite - for pins 16-31 (Note: PORTA mapping reversed from others)
#define fWriteB(_pin_, _state_) ( _pin_ < 24 ? (_state_ ? PORTC |= 1 << (_pin_ -16) : PORTC &= ~(1 << (_pin_ -16))) : (_state_ ? PORTA |= 1 << (31- _pin_) : PORTA &= ~(1 << (31- _pin_) )))
//Atmega644/1284 Version of fastWrite - for pins 0-31 (combined fWriteA and fWriteB)
#define fWrite( _pin_, _state_ ) ( ( _pin_ < 16 ) ? fWriteA( _pin_, _state_ ) : fWriteB( _pin_, _state_ ) )

// sets the scrolling speed;
// value between 0 (slowest) and 4 (fastest); translates into delay from 50ms to 10ms;
// delay = 50 - nSpeed * 10;
int nSpeed = 3;	// default speed; makes for a 20ms delay;
#define MAX_SPEED   4


/*
 * ht1632_chipselect / ht1632_chipfree
 * Select or de-select a particular ht1632 chip. De-selecting a chip ends the commands being sent to a chip.
 * CD pins are active-low; writing 0 to the pin selects the chip.
 */

void ht1632_chipselect(byte chipno)
{
  fWrite(chipno, 0);
}

void ht1632_chipfree(byte chipno)
{
  fWrite(chipno, 1);
}

/*
 * ht1632_writebits
 * Write bits (up to 8) to h1632 on pins ht1632_data, ht1632_wrclk Chip is assumed to already be chip-selected
 * Bits are shifted out from MSB to LSB, with the first bit sent being (bits & firstbit), shifted till firsbit is zero.
 */
void ht1632_writebits (byte bits, byte firstbit)
{
  DEBUGPRINT(" ");
  while (firstbit) {
    DEBUGPRINT((bits&firstbit ? "1" : "0"));
    fWrite(HT1632_WRCLK, LOW);
    if (bits & firstbit) {
      fWrite(HT1632_DATA, HIGH);
    } 
    else {
      fWrite(HT1632_DATA, LOW);
    }
    fWrite(HT1632_WRCLK, HIGH);
    firstbit >>= 1;
  }
}


/*
 * ht1632_sendcmd
 * Send a command to the ht1632 chip. A command consists of a 3-bit "CMD" ID, an 8bit command, and one "don't care bit".
 *   Select 1 0 0 c7 c6 c5 c4 c3 c2 c1 c0 xx Free
 */
static void ht1632_sendcmd (byte d, byte command)
{
  ht1632_chipselect(ht1632_cs[d]);        // Select chip
  ht1632_writebits(HT1632_ID_CMD, 1<<2);  // send 3 bits of id: COMMMAND
  ht1632_writebits(command, 1<<7);        // send the actual command
  ht1632_writebits(0, 1);         	  // one extra dont-care bit in commands.
  ht1632_chipfree(ht1632_cs[d]);          //done
}


/*
 * ht1632_senddata
 * send a nibble (4 bits) of data to a particular memory location of the
 * ht1632.  The command has 3 bit ID, 7 bits of address, and 4 bits of data.
 *    Select 1 0 1 A6 A5 A4 A3 A2 A1 A0 D0 D1 D2 D3 Free
 * Note that the address is sent MSB first, while the data is sent LSB first!
 * This means that somewhere a bit reversal will have to be done to get
 * zero-based addressing of words and dots within words.
 */
static void ht1632_senddata (byte d, byte address, byte data)
{
  ht1632_chipselect(ht1632_cs[d]);      // Select chip
  ht1632_writebits(HT1632_ID_WR, 1<<2); // Send ID: WRITE to RAM
  ht1632_writebits(address, 1<<6);      // Send address
  ht1632_writebits(data, 1<<3);         // Send 4 bits of data
  ht1632_chipfree(ht1632_cs[d]);        // Done.
}


/*
 * ht1632_setup
 * setup the ht1632 chips
 */
void ht1632_setup()
{
  for (byte d=0; d<NUM_DISPLAYS; d++) {
    pinMode(ht1632_cs[d], OUTPUT);

    digitalWrite(ht1632_cs[d], HIGH);  // Unselect (active low)
     
    pinMode(HT1632_WRCLK, OUTPUT);
    pinMode(HT1632_DATA, OUTPUT);
    
       ht1632_sendcmd(d, HT1632_CMD_SYSON);    // System on 
    ht1632_sendcmd(d, HT1632_CMD_LEDON);    // LEDs on 
    ht1632_sendcmd(d, HT1632_CMD_COMS01);   // NMOS Output 24 row x 24 Com mode
    
    for (byte i=0; i<128; i++)
      ht1632_senddata(d, i, 0);  // clear the display!
  }
}


/*
 * we keep a copy of the display controller contents so that we can know which bits are on without having to (slowly) read the device.
 * Note that we only use the low four bits of the shadow ram, since we're shadowing 4-bit memory.  This makes things faster, and we
 * use the other half for a "snapshot" when we want to plot new data based on older data...
 */
byte ht1632_shadowram[NUM_DISPLAYS * 96];  // our copy of the display's RAM


/*
 * plot a point on the display, with the upper left hand corner being (0,0).
 * Note that Y increases going "downward" in contrast with most mathematical coordiate systems, but in common with many displays
 * No error checking; bad things may happen if arguments are out of bounds!  (The ASSERTS compile to nothing by default
 */
void ht1632_plot (char x, char y, char val)
{
   if (x<0 || x>=X_MAX || y<0 || y>=Y_MAX)
    return;
    
  char addr, bitval;

  ASSERT(x >= 0);
  ASSERT(x <= X_MAX);
  ASSERT(y >= 0);
  ASSERT(y <= y_MAX);

  byte d;
  //select display depending on plot values passed in
  if (x >= 0 && x <=23 ) {
    d = 0;
  }  
  if (x >=24 && x <=47) {
    d = 1;
    x = x-24; 
  }   
  if (x >=48 && x <=71) {
    d = 2;
    x = x-48; 
  }   
    if (x >=72 && x <=95) {
    d = 3;
    x = x-72; 
  }
  
  /*
   * The 4 bits in a single memory word go DOWN, with the LSB (first transmitted) bit being on top.  However, writebits()
   * sends the MSB first, so we have to do a sort of bit-reversal somewhere.  Here, this is done by shifting the single bit in
   * the opposite direction from what you might expect.
   */

  bitval = 8>>(y&3);  // compute which bit will need set

  addr = (x<<2) + (y>>2);  // compute which memory word this is in 

  if (val) {  // Modify the shadow memory
    ht1632_shadowram[(d * 96)  + addr] |= bitval;
  } 
  else {
    ht1632_shadowram[(d * 96) + addr] &= ~bitval;
  }
  // Now copy the new memory value to the display
  ht1632_senddata(d, addr, ht1632_shadowram[(d * 96) + addr]);
}


/*
 * get_shadowram
 * return the value of a pixel from the shadow ram.
 */
byte get_shadowram(byte x, byte y)
{
  byte addr, bitval, d;

  //select display depending on plot values passed in
  if (x >= 0 && x <=23 ) {
    d = 0;
  }  
  if (x >=24 && x <=47) {
    d = 1;
    x = x-24; 
  }
  if (x >=48 && x <=71) {
    d = 2;
    x = x-48; 
  }   
    if (x >=72 && x <=95) {
    d = 3;
    x = x-72; 
  }  

  bitval = 8>>(y&3);  // compute which bit will need set
  addr = (x<<2) + (y>>2);       // compute which memory word this is in 
  return (0 != (ht1632_shadowram[(d * 96) + addr] & bitval));
}


/*
 * snapshot_shadowram
 * Copy the shadow ram into the snapshot ram (the upper bits)
 * This gives us a separate copy so we can plot new data while
 * still having a copy of the old data.  snapshotram is NOT
 * updated by the plot functions (except "clear")
 */
void snapshot_shadowram()
{
  for (byte i=0; i< sizeof ht1632_shadowram; i++) {
    ht1632_shadowram[i] = (ht1632_shadowram[i] & 0x0F) | ht1632_shadowram[i] << 4;  // Use the upper bits
  }

}

/*
 * get_snapshotram
 * get a pixel value from the snapshot ram (instead of
 * the actual displayed (shadow) memory
 */
byte get_snapshotram(byte x, byte y)
{

  byte addr, bitval;
  byte d = 0;

  //select display depending on plot values passed in 
  if (x >=24 && x <=47) {
    d = 1;
    x = x-24; 
  }  
    if (x >=47 && x <=72) {
    d = 2;
    x = x-48; 
  }   
    if (x >=72 && x <=95) {
    d = 3;
    x = x-72; 
  }

  bitval = 128>>(y&3);  // user upper bits!
  addr = (x<<2) + (y>>2);   // compute which memory word this is in 
  if (ht1632_shadowram[(d * 96) + addr] & bitval)
    return 1;
  return 0;
}


/*
 * ht1632_clear
 * clear the display, and the shadow memory, and the snapshot
 * memory.  This uses the "write multiple words" capability of
 * the chipset by writing all 96 words of memory without raising
 * the chipselect signal.
 */
void ht1632_clear()
{
  char i;
  for(byte d=0; d<NUM_DISPLAYS; d++)
  {
    ht1632_chipselect(ht1632_cs[d]);  // Select chip
    ht1632_writebits(HT1632_ID_WR, 1<<2);  // send ID: WRITE to RAM
    ht1632_writebits(0, 1<<6); // Send address
    for (i = 0; i < 96/2; i++) // Clear entire display
      ht1632_writebits(0, 1<<7); // send 8 bits of data
    ht1632_chipfree(ht1632_cs[d]); // done
    for (i=0; i < 96; i++)
      ht1632_shadowram[96*d + i] = 0;
  }
}

// =======================================================================================
// ---- Sets LED brightness ----
// ---- Setting of 1 = brightness of one, rest is x3 up to 5
// ---------------------------------------------------------------------------------------
void setBrightness(byte nLevel)
{
  byte multi; // Multiplyer
  if (nLevel ==1) multi=1; // Make sure we set lowest setting
  else multi=3; // Do brightness in 3x increments
  if (nLevel > 5)
      nLevel = 5;
  
  
  for (byte i=0; i<=4; i++)
    ht1632_sendcmd(i, HT1632_CMD_PWM + nLevel*multi);
  
}

void fade_down() {
  char intensity;
  for (intensity=14; intensity >= 0; intensity--) {
    ht1632_sendcmd(0, HT1632_CMD_PWM + intensity); //send intensity commands using CS0 for display 0
    ht1632_sendcmd(1, HT1632_CMD_PWM + intensity); //send intensity commands using CS0 for display 1
    delay(FADEDELAY);
  }
  //clear the display and set it to full brightness again so we're ready to plot new stuff
  cls();
  ht1632_sendcmd(0, HT1632_CMD_PWM + 15);
  ht1632_sendcmd(1, HT1632_CMD_PWM + 15);
}


/*
 * fade_up
 * fade the display up to full brightness
 */
void fade_up() {
  char intensity;
  for ( intensity=0; intensity < 15; intensity++) {
    ht1632_sendcmd(0, HT1632_CMD_PWM + intensity); //send intensity commands using CS0 for display 0
    ht1632_sendcmd(1, HT1632_CMD_PWM + intensity); //send intensity commands using CS0 for display 1
    delay(FADEDELAY);
  }
}

/* ht1632_putchar
 * Copy a 5x7 character glyph from the myfont data structure to display memory, with its upper left at the given coordinate
 * This is unoptimized and simply uses plot() to draw each dot.
 */
void ht1632_putchar_old(byte x, byte y, char c)
{
  byte dots;
  if (c >= 'A' && c <= 'Z' || (c >= 'a' && c <= 'z') ) {
    c &= 0x1F;   // A-Z maps to 1-26
  } 
  else if (c >= '0' && c <= '9') {
    c = (c - '0') + 31;
  } 
  else if (c == ' ') {
    c = 0; // space
  }
  else if (c == '.') {
    c = 27; // full stop
  }
  else if (c == '\'') {
    c = 28; // single quote mark
  }  
  else if (c == ':') {
    c = 29; // clock_mode selector arrow
  }
  else if (c == '>') {
    c = 30; // clock_mode selector arrow
  }

  for (char col=0; col< 5; col++) {
    dots = pgm_read_byte_near(&myfont[c][col]);
    for (char row=0; row < 7; row++) {
      if (dots & (64>>row))   	     // only 7 rows.
        plot(x+col, y+row, 1);
      else 
        plot(x+col, y+row, 0);
    }
  }
}

//*********************************************************************************************************
/*
 * Copy a character glyph from the myfont data structure to
 * display memory, with its upper left at the given coordinate
 * This is unoptimized and simply uses plot() to draw each dot.
 * (fc, Jan 30/2011) display character using the specified color;
 */
void ht1632_putchar(int x, int y, char c, byte color)
{
	// fonts defined for ascii 32 and beyond (index 0 in font array is ascii 32);
	byte charIndex;

	// replace undisplayable characters with blank;
	if (c < 32 || c > 127) charIndex =0;
        else charIndex =c - 32;
       	// move character definition, pixel by pixel, onto the display;
	// fonts are defined as one byte per row;
	for (byte row=0; row<8; row++)
	{
		byte rowDots = pgm_read_byte_near(&myfont[charIndex][row]);
		for (byte col=0; col<6; col++)
		{
			if (rowDots & (1<<(5-col)))
				ht1632_plot(x+col, y+row,color);
			else 
				ht1632_plot(x+col, y+row, 0);
		}
	}
}

//*********************************************************************************************************
/*
 * Copy a character glyph from the myfont data structure to
 * display memory, with its upper left at the given coordinate
 * This is unoptimized and simply uses plot() to draw each dot.
 * LensDigital: Will not display character beyond specified limit
 */
void ht1632_putchar_limit(int x, int y, char c, byte color, int limit)
{
	// fonts defined for ascii 32 and beyond (index 0 in font array is ascii 32);
	int charIndex;

	// replace undisplayable characters with blank;
	if (c < 32 || c > 127) charIndex =0;
        else charIndex =c - 32;
       	// move character definition, pixel by pixel, onto the display;
	// fonts are defined as one byte per row;
	for (byte row=0; row<8; row++)
	{
		byte rowDots = pgm_read_byte_near(&myfont[charIndex][row]);
		for (byte col=0; col<6; col++)
		{
		  if (x<200){	// Failsafe in case coordinate goes beyond 255 byte size
                    if (x+col >= limit) {
                          if (rowDots & (1<<(5-col)))
				ht1632_plot(x+col, y+row,color);
  			  else 
  				ht1632_plot(x+col, y+row, BLACK);
                        }
                  }
		}
	}
}


//*********************************************************************************************************
/*
 * Copy a character glyph from the smallFont data structure to
 * display memory, with its upper left at the given coordinate
 * This is unoptimized and simply uses plot() to draw each dot.
 */
void ht1632_putSmallChar(int x, int y, char c, byte color)
{
	// fonts defined for ascii 32 and beyond (index 0 in font array is ascii 32);
	byte charIndex;

	// replace undisplayable characters with blank;
	if (c < 32 || c > 127)
	{
		charIndex	=	0;
	}
	else
	{
		charIndex	=	c - 32;
	}

	// move character definition, pixel by pixel, onto the display;
	// fonts are defined as one byte per row;
	for (byte row=0; row<8; row++)
	{
		byte rowDots	=	pgm_read_byte_near(&smallFont[charIndex][row]);
		for (byte col=0; col<6; col++)
		{
			if (rowDots & (1<<(5-col)))
				ht1632_plot(x+col, y+row, color);
			else 
				ht1632_plot(x+col, y+row, BLACK);
		}
	}
}


//*********************************************************************************************************
/*
 * Copy a character glyph from the tinyFont data structure to
 * display memory, with its upper left at the given coordinate
 * This is unoptimized and simply uses plot() to draw each dot.
*/
void ht1632_putTinyChar(int x, int y, char c, byte color)
{
	// fonts defined for ascii 32 and beyond (index 0 in font array is ascii 32);
	byte charIndex;

	// replace undisplayable characters with blank;
	if (c < 32 || c > 126)
	{
		charIndex	=	0;
	}
	else
	{
		charIndex	=	c - 32;
	}

	// move character definition, pixel by pixel, onto the display;
	// fonts are defined as one byte per row;
	for (byte row=0; row<8; row++)
	{
		byte rowDots	=	pgm_read_byte_near(&tinyFont[charIndex][row]);
		for (byte col=0; col<4; col++)
		{
			if (rowDots & (1<<(3-col)))
				ht1632_plot(x+col, y+row, color);
			else 
				ht1632_plot(x+col, y+row, BLACK);
		}
	}
}
// =======================================================================================
// ---- Display Static text at specified location ----
//  PLEASE NOTE M_OFFSET shifts text to bottom screen if it's set to 48!
// By: Bratan
// parameters: x - starting position, y - vertical position
//             myString - array of chars, 
//             fntsz - Font Size. 1-Normal, 2-Small, 3-Tiny
//             color -  GREEEN, or BLACK 
// ---------------------------------------------------------------------------------------
void showText(int x, int y, char * myString, int fntsz, byte color){
//  if (color==4)color=random(3)+1; // Select random color
  x=x+M_OFFSET;
  for (int i=0;i< (strlen(myString));i++){
     switch (fntsz)
    {
     case 1: // Normal
     ht1632_putchar(i*6+x,y,myString[i],color); // i*8 mutliplies position by char width "8"
     break;
     case 2: // Small
     ht1632_putSmallChar(x+i*6,y,myString[i],color); // i*8 mutliplies position by char width "8"
     break;
     case 3: // Tiny 
     ht1632_putTinyChar(x+i*4,y,myString[i],color); // i*8 mutliplies position by char width "8"
     break;
     default: // Normal (default)
     ht1632_putchar(i*6+x,y,myString[i],color); // i*8 mutliplies position by char width "8"
    }
    
   }
}

/*
 ===========================================================================================
 * "Scrolls" up to point set by x_stop. Must be called constantly from main loop for "scroll" effect,
 * otherwise it just shows static message
 * -- disp can be 0 or 1  and will show scrolling msg on 1st half of the screen or second
 * by LensDigital
 * Based on  functions by Bill Ho
  =========================================================================================*/
void scrolltextlimit(byte disp, int x_stop,int y,char Str1[ ], int delaytime){
  //if (!scrollPermit) return;
  int messageLength = strlen(Str1)+ 1;
     if (!isScrolling[disp]) {
      xpos[disp] = X_MAX;
      isScrolling[disp]=true;
    }
    if (xpos[disp] > (-1 * ( 8* messageLength))+x_stop) {
      showTextSplit (x_stop,y, Str1,disp);
      //if ( (digitalRead(SET_BUTTON_PIN) == HIGH) || (digitalRead(MENU_BUTTON_PIN) == HIGH) || (digitalRead(INC_BUTTON_PIN) == HIGH)  ) {cls (); return; } // Interrupt
      //delay(delaytime);// reduce speed of scroll
      xpos[disp]--;
      lastScroll[disp]=millis();
    }
    else { 
      isScrolling[disp] =false;
      scrollDone[disp]=true;
      xpos[disp] = X_MAX;
      //scrollPermit=false;
    }

}


// ======================================================================
// by LensDigital: Shows static text line
// ======================================================================
void showTextSplit (int x_stop, int y, char Str1[ ],byte disp)
{
  int messageLength = strlen(Str1)+ 1;
  for (int i = 0; i < messageLength; i++) {
        if ( (xpos[disp] + (8 * i)) < x_stop-5) ;
        else ht1632_putchar_limit(xpos[disp] + (8 * i), y,Str1[i],GREEN,x_stop);
        
      }
}

// =======================================================================================
// ---- Display single digit at requested location in requested color ----
// By: Bratan
// ---------------------------------------------------------------------------------------
void showDigit(int x, int y, int digit, int fontW, byte fontNum,byte color){
  int myOffset=7;// For char/digits font 8 px wide
  int fontOffset = fontNum * CHARS_IN_FONT;
  fontW=fontW-1; // Font width -1
  // Font height
  for (int row=0; row<BYTES_PER_CHARS; row++){
   for (int i=fontW;i>=0;i--){
    if (pgm_read_byte_near(&bigFont[digit+fontOffset][row]) & (1<<i) ) { plot ( (myOffset+x)-i,row+y,color); } 
    else { plot ( (myOffset+x)-i,row+y,BLACK ); }
   }
  }
}
