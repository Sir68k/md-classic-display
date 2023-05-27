#include "lcd_emu.h"
#include <string.h>
#include <Arduino.h>

#define LCD_ROWS 4
#define LCD_COLS 16
#define LCD_SIZE (LCD_ROWS*LCD_COLS)
#define LCD_CHAR_MEM_SIZE (8*16)

uint8_t character_data[LCD_CHAR_MEM_SIZE];
uint8_t text_data[LCD_SIZE];

uint8_t lcd_shift = 0;

//////////
// Init //
//////////

void lcd_init() {    
  memset(character_data,  0, LCD_CHAR_MEM_SIZE); 
  memset(text_data,     ' ', LCD_SIZE);

  lcd_shift = 0;
}

/////////////////
// TEXT MEMORY //
/////////////////

void set_text_memory(uint8_t pos, uint8_t len, uint8_t *data) {
  for(uint8_t i=0; i<len; i++)
    text_data[(pos + i) % LCD_SIZE] = (char)data[i];
}

void cmd_c0(uint8_t *data) {
  // c0 p b0 b1 b2 b3 b4
  uint8_t p = data[1];
  set_text_memory(p, 5, &data[2]);
};

////////////////////////////////
// CHARACTER + SYMBOL MEMORY //
///////////////////////////////

//normally it's just 8*8, but lets extend the spec


void set_char_memory(uint16_t pos, uint16_t len, uint8_t *data) {
  for(uint16_t i=0; i<len; i++)
    character_data[(pos + i) % LCD_CHAR_MEM_SIZE] = (char)data[i];
}


void cmd_d0(uint8_t *data) { 
  // d0 p b0 b1 b2 b3 b4 b5
  uint8_t p = data[1];
  set_char_memory(p, 6, &data[2]);
}


void cmd_07(){
  // reset

  lcd_init();
}

void cmd_0f(){
  // display on
  // probably should trigger timer
}

void cmd_2x(uint8_t cmd){
  // contrast
  // ignore for now, maybe remap to brightness
}

void cmd_48p(uint8_t cmd){
  // shift cmd-48
  lcd_shift = cmd & 0b111;
}

/////////////
// DATA IN //
/////////////

#define BUF_SIZE 32

volatile uint8_t byte_buffer[BUF_SIZE];
volatile uint8_t cpos = 0;
volatile uint8_t remaining = 0;
volatile uint8_t reading = 0;

void lcd_byte_in(uint8_t in) {
  if (reading) {
    byte_buffer[cpos] = in;
    
    cpos++;
    remaining--;

    if (remaining == 0) {
      reading = 0;

      switch(byte_buffer[0]) {
        case 0xC0:
          cmd_c0((uint8_t*)byte_buffer);
          break;
        case 0xD0:
          cmd_d0((uint8_t*)byte_buffer);
          break;
        default:
          //nothing;
          break;
      }
    }

    return;
  }

  if      (in == 0x07)
    cmd_07();
  else if (in == 0x0f)
    cmd_0f();
  else if (in >= 0x20 && in <= 0x2F)
    cmd_2x(in);
  else if (in >= 0x48 && in <= 0x4F)
    cmd_48p(in);
  else if (in == 0xC0 || in == 0xD0) {
    remaining = in == 0xC0 ? 6 : 7;
    reading = 1;
    byte_buffer[0] = in;
    cpos = 1;
  }
  else {
    // ignore
  }
}

/////////////
// READING //
/////////////

uint8_t lcd_get_shift() {
  return lcd_shift;
}

uint8_t* lcd_get_character_data() {
  return character_data;
}

uint8_t* lcd_get_text_data(uint8_t row) {
  return &text_data[row * LCD_COLS];   
}

uint8_t lcd_get_symbol(uint8_t bbyte, uint8_t bbit) {
  return LCD_SYM_GET_BIT(character_data, bbyte, bbit);
}

void lcd_set_symbol(uint8_t bbyte, uint8_t bbit, uint8_t val) {
  bitWrite(character_data[bbyte],  bbit, val);
}

uint8_t  lcd_get_7segment(uint8_t n) {
  uint8_t res = 0;
  
  switch (n) {
    case 1:
      res = (lcd_get_symbol(LCD_SYM_SEG1_1) << 0) | 
            (lcd_get_symbol(LCD_SYM_SEG1_2) << 1) | 
            (lcd_get_symbol(LCD_SYM_SEG1_3) << 2) | 
            (lcd_get_symbol(LCD_SYM_SEG1_4) << 3) | 
            (lcd_get_symbol(LCD_SYM_SEG1_5) << 4) | 
            (lcd_get_symbol(LCD_SYM_SEG1_6) << 5) | 
            (lcd_get_symbol(LCD_SYM_SEG1_7) << 6);
      break;

    case 2:
      res = (lcd_get_symbol(LCD_SYM_SEG2_1) << 0) | 
            (lcd_get_symbol(LCD_SYM_SEG2_2) << 1) | 
            (lcd_get_symbol(LCD_SYM_SEG2_3) << 2) | 
            (lcd_get_symbol(LCD_SYM_SEG2_4) << 3) | 
            (lcd_get_symbol(LCD_SYM_SEG2_5) << 4) | 
            (lcd_get_symbol(LCD_SYM_SEG2_6) << 5) | 
            (lcd_get_symbol(LCD_SYM_SEG2_7) << 6);
      break;

    case 3:
      res = (lcd_get_symbol(LCD_SYM_SEG3_1) << 0) | 
            (lcd_get_symbol(LCD_SYM_SEG3_2) << 1) | 
            (lcd_get_symbol(LCD_SYM_SEG3_3) << 2) | 
            (lcd_get_symbol(LCD_SYM_SEG3_4) << 3) | 
            (lcd_get_symbol(LCD_SYM_SEG3_5) << 4) | 
            (lcd_get_symbol(LCD_SYM_SEG3_6) << 5) | 
            (lcd_get_symbol(LCD_SYM_SEG3_7) << 6);
      break;

    case 4:
      res = (lcd_get_symbol(LCD_SYM_SEG4_1) << 0) | 
            (lcd_get_symbol(LCD_SYM_SEG4_2) << 1) | 
            (lcd_get_symbol(LCD_SYM_SEG4_3) << 2) | 
            (lcd_get_symbol(LCD_SYM_SEG4_4) << 3) | 
            (lcd_get_symbol(LCD_SYM_SEG4_5) << 4) | 
            (lcd_get_symbol(LCD_SYM_SEG4_6) << 5) | 
            (lcd_get_symbol(LCD_SYM_SEG4_7) << 6);
      break;

    case 5:
      res = (lcd_get_symbol(LCD_SYM_SEG5_1) << 0) | 
            (lcd_get_symbol(LCD_SYM_SEG5_2) << 1) | 
            (lcd_get_symbol(LCD_SYM_SEG5_3) << 2) | 
            (lcd_get_symbol(LCD_SYM_SEG5_4) << 3) | 
            (lcd_get_symbol(LCD_SYM_SEG5_5) << 4) | 
            (lcd_get_symbol(LCD_SYM_SEG5_6) << 5) | 
            (lcd_get_symbol(LCD_SYM_SEG5_7) << 6);
      break;
  }

  return res;
}
