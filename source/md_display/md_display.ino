#include "oled.hpp"
#include "lcd_emu.h"
#include "icons.h"

#include <SPI.h>
#include <avr/io.h>
#include <avr/interrupt.h>

///////////
/// SPI ///
///////////

void SPI0_init(void);

/* SPI as Slave
 * Pin Assignment
 * 2. PA4 : SS
 * 11.PA1 : MOSI
 * 12.PA2 : MISO
 * 13.PA3 : SCK
 */
 
#define MOSI_bp 1
#define MOSI_bm (1 << MOSI_bp)
#define MISO_bp 2
#define MISO_bm (1 << MISO_bp)

#define SCK_bp 3
#define SCK_bm (1 << SCK_bp)
#define SS_bp 4
#define SS_bm (1 << SS_bp)


void SPI0_init(void) 
{
  PORTA.DIRSET = MISO_bm;

  SPI0.CTRLB = SPI_MODE3;
  SPI0.CTRLA = SPI_DORD_bm    /* LSB is transmitted first */
    | SPI_ENABLE_bm           /* Enable module */
    | (!SPI_MASTER_bm);       /* SPI module in Slave mode */
    SPI0.INTCTRL = SPI_IE_bm; /* SPI Interrupt enable */
}

ISR(SPI0_INT_vect)
{
  //uint8_t receiveData = SPI0.DATA;
  lcd_byte_in((uint8_t)SPI0.DATA);
  SPI0.DATA = 0;
  SPI0.INTFLAGS = SPI_IF_bm; /* Clear the Interrupt flag by writing 1 */
}

//////////
// OLED //
//////////

OLED<SSD1306_128x32> oled;

///////////
// SETUP //
///////////

void setup() {
  //Serial.begin(9600);

  SPI0_init();
  sei();

  pinMode(PIN_PA6, OUTPUT);
  digitalWrite(PIN_PA6, HIGH);


  pinMode(PIN_PA5, OUTPUT);
  digitalWrite(PIN_PA5, LOW);
  

  delay(100);
  digitalWrite(PIN_PA5, HIGH);
  
  oled.init();
  oled.flipV(1);
  oled.flipH(1);


  lcd_init();

  oled_intro();
  delay(250);
}

void oled_intro() {
  set_text_memory(0, 6, "Hello ");
  set_text_memory(6, 6, "World!");
  
  lcd_set_symbol(LCD_SYM_LP2, 1);
  lcd_set_symbol(LCD_SYM_MELODY, 1);
  lcd_set_symbol(LCD_SYM_REC, 1);
  lcd_set_symbol(LCD_SYM_SHUF, 1);
  lcd_set_symbol(LCD_SYM_SOUND1, 1);
  lcd_set_symbol(LCD_SYM_BOOKMARKED, 1);  

  lcd_set_symbol(LCD_SYM_BAT_BOX, 1);
  lcd_set_symbol(LCD_SYM_BAT_2, 1);
  
  lcd_set_symbol(LCD_SYM_DISC_1, 1);
  lcd_set_symbol(LCD_SYM_DISC_2, 1);
  lcd_set_symbol(LCD_SYM_DISC_3, 1);
  
  display_n510();
}

void loop() {
  display_n510();
}


void display_n510() {
  uint8_t  shift     = lcd_get_shift();

  oled.clear();
  oled.home();

  oled.setScale(1);

  ////////////////
  // AUDIO TYPE //
  ////////////////
    
  if (lcd_get_symbol(LCD_SYM_MONO))
    oled.drawString("MONO");
  else if (lcd_get_symbol(LCD_SYM_LP2))
    oled.drawString("LP2");
  else if (lcd_get_symbol(LCD_SYM_LP4))
    oled.drawString("LP4");
  else
    oled.drawString("SP");

  ////////////////////
  // SOUND SETTINGS //
  ////////////////////

  if (lcd_get_symbol(LCD_SYM_SOUND1))
    oled.drawString("/S1");
  else if (lcd_get_symbol(LCD_SYM_SOUND2))
    oled.drawString("/S2");

  ///////////////
  // PLAY MODE //
  ///////////////

  if (lcd_get_symbol(LCD_SYM_SHUF))
    oled.drawString("/SHUF");
  else if (lcd_get_symbol(LCD_SYM_REPEAT))
    oled.drawString("/REP");
  else if (lcd_get_symbol(LCD_SYM_SINGLE))
    oled.drawString("/1");

  ///////////
  // OTHER //
  ///////////

  if (lcd_get_symbol(LCD_SYM_SYNC))
    oled.drawString("/SYNC");

  if (lcd_get_symbol(LCD_SYM_BOOKMARKED))
    oled.drawString("/\xA3\xA4"); // bookmark char
  
  /////////
  // REC //
  /////////
  
  if (lcd_get_symbol(LCD_SYM_REC)) {
    oled.setCursorXY(128-14, 0);
    oled.drawString("\x98\x99"); // rec char
  }

  /////////////
  // BATTERY //
  /////////////



  if (lcd_get_symbol(LCD_SYM_BAT_BOX)) {
      uint8_t* battery_icon;

     if (lcd_get_symbol(LCD_SYM_BAT_4))
      battery_icon = icons_7x7[9];
     else if (lcd_get_symbol(LCD_SYM_BAT_3))
      battery_icon = icons_7x7[10];
     else if (lcd_get_symbol(LCD_SYM_BAT_2))
      battery_icon = icons_7x7[11];
     else if (lcd_get_symbol(LCD_SYM_BAT_1))
      battery_icon = icons_7x7[12];
     else
      battery_icon = icons_7x7[13];

     oled.drawIcon7(battery_icon, 118, 12);
  }


  //oled.drawIcon7(icons_7x7[4], 110, 11);

  /////////////////
  // DISC STATUS //
  /////////////////

  if (lcd_get_symbol(LCD_SYM_DISC_1))
    oled.fastLineV(127, 0, 9);

  if (lcd_get_symbol(LCD_SYM_DISC_2))
    oled.fastLineV(127, 11, 20);

  if (lcd_get_symbol(LCD_SYM_DISC_3))
    oled.fastLineV(127, 22, 31);

  ////////////////
  // LABEL MODE //
  ////////////////


  oled.setCursorXY(0, 10);
  
  if (lcd_get_symbol(LCD_SYM_MELODY))
    oled.drawString("\x94\x95"); // melody char
  if (lcd_get_symbol(LCD_SYM_FOLDER))
    oled.drawString("\x92\x93"); // folder char
  
  /////////////////////
  // NUMBER SEGMENTS //
  /////////////////////
  
  draw7segment(92+ 0, 25, 0x7f);//lcd_get_7segment(1));
  draw7segment(92+ 6, 25, 0x7f);//lcd_get_7segment(2));
  draw7segment(92+12, 25, 0x7f);//lcd_get_7segment(3));

  if (lcd_get_symbol(LCD_SYM_SEG_COL)) {
    oled.setCursorXY(92+17, 25);
    oled.drawString(":");
  }

  draw7segment(92+22, 25, 0x7f);//lcd_get_7segment(4));
  draw7segment(92+28, 25, 0x7f);//lcd_get_7segment(5));
  
  ///////////////
  // TEXT DATA //
  ///////////////

  
  oled.setScale(2);
  oled.setCursorXY(0, 18);
  
  uint8_t* txt = lcd_get_text_data(0);
  for (int i=0; i<7; i++) {
    oled.drawChar(txt[i]);  
  }


  /*
  for (int r=0; r<3; r++) { 
    oled.setCursorXY(0, 9 + r*8);
    uint8_t* txt = lcd_get_text_data(r);
    
    for (int i=0; i<10; i++) {
      oled.drawChar(txt[i]);  
    }
  }*/
  
  oled.update();
  delay(10);
}

void draw7segment(uint8_t x, uint8_t y, uint8_t v) {

  // top segment
  if (v & 0b0000001){
    oled.dot(x+1, y, 1);  
    oled.dot(x+2, y, 1); 
    oled.dot(x+3, y, 1); 
  }

  // middle
  if (v & 0b0001000){
    oled.dot(x+1, y+3, 1);  
    oled.dot(x+2, y+3, 1); 
    oled.dot(x+3, y+3, 1); 
  }

  // bottom
  if (v & 0b1000000){
    oled.dot(x+1, y+6, 1);  
    oled.dot(x+2, y+6, 1); 
    oled.dot(x+3, y+6, 1); 
  }

  // top right
  if (v & 0b0000010){
    oled.dot(x+4, y+1, 1);  
    oled.dot(x+4, y+2, 1); 
  }

  // top left
  if (v & 0b0000100){
    oled.dot(x, y+1, 1);  
    oled.dot(x, y+2, 1); 
  }

  // bottom right
  if (v & 0b0010000){
    oled.dot(x+4, y+4, 1);  
    oled.dot(x+4, y+5, 1); 
  }

  // bottom left
  if (v & 0b0100000){
    oled.dot(x, y+4, 1);  
    oled.dot(x, y+5, 1); 
  }
}
