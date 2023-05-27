#include <stdint.h>

#define LCD_SYM_MONO          59,4
#define LCD_SYM_LP2           62,3
#define LCD_SYM_LP4           62,4

#define LCD_SYM_SOUND1        63,4
#define LCD_SYM_SOUND2        63,5

#define LCD_SYM_EQ_BAR_1      55,1
#define LCD_SYM_EQ_BAR_2      55,2
#define LCD_SYM_EQ_BAR_3      55,3
#define LCD_SYM_EQ_BAR_4      55,4
#define LCD_SYM_EQ_BAR_5      55,5
#define LCD_SYM_EQ_BAR_6      52,4


#define LCD_SYM_DISC_1        54,3
#define LCD_SYM_DISC_2        54,4
#define LCD_SYM_DISC_3        54,5
#define LCD_SYM_DISC_CIRCLE   58,5

#define LCD_SYM_DISC          47,4
#define LCD_SYM_BOOKMARKED    61,3
#define LCD_SYM_MELODY        47,5
#define LCD_SYM_FOLDER        47,3
#define LCD_SYM_SYNC          59,2
#define LCD_SYM_REC           59,1

#define LCD_SYM_SHUF          59,3
#define LCD_SYM_REPEAT        56,4
#define LCD_SYM_SINGLE        58,3

#define LCD_SYM_REMAIN_MIN    60,5
#define LCD_SYM_REMAIN_R      61,4

#define LCD_SYM_BAT_1         45,3
#define LCD_SYM_BAT_2         45,4
#define LCD_SYM_BAT_3         49,4
#define LCD_SYM_BAT_4         49,5
#define LCD_SYM_BAT_BOX       45,5

#define LCD_SYM_SEG1_1 42,3  
#define LCD_SYM_SEG1_2 42,4 
#define LCD_SYM_SEG1_3 62,2 
#define LCD_SYM_SEG1_4 42,2 
#define LCD_SYM_SEG1_5 42,5 
#define LCD_SYM_SEG1_6 62,1 
#define LCD_SYM_SEG1_7 42,1

#define LCD_SYM_SEG2_1 41,3 
#define LCD_SYM_SEG2_2 41,4 
#define LCD_SYM_SEG2_3 61,2 
#define LCD_SYM_SEG2_4 41,2 
#define LCD_SYM_SEG2_5 41,5 
#define LCD_SYM_SEG2_6 61,1 
#define LCD_SYM_SEG2_7 41,1

#define LCD_SYM_SEG3_1 43,3 
#define LCD_SYM_SEG3_2 43,4 
#define LCD_SYM_SEG3_3 63,2 
#define LCD_SYM_SEG3_4 43,2 
#define LCD_SYM_SEG3_5 43,5 
#define LCD_SYM_SEG3_6 63,1 
#define LCD_SYM_SEG3_7 43,1

#define LCD_SYM_SEG4_1 44,3 
#define LCD_SYM_SEG4_2 44,4 
#define LCD_SYM_SEG4_3 48,2 
#define LCD_SYM_SEG4_4 44,2 
#define LCD_SYM_SEG4_5 44,5 
#define LCD_SYM_SEG4_6 48,1 
#define LCD_SYM_SEG4_7 44,1

#define LCD_SYM_SEG5_1 46,1 
#define LCD_SYM_SEG5_2 46,2 
#define LCD_SYM_SEG5_3 50,1 
#define LCD_SYM_SEG5_4 50,2 
#define LCD_SYM_SEG5_5 46,3 
#define LCD_SYM_SEG5_6 45,2 
#define LCD_SYM_SEG5_7 50,3

#define LCD_SYM_SEG_COL 48,3

#define LCD_SYM_GET_BIT(b_buffer, b_byte, b_bit) ((b_buffer[b_byte] >> b_bit) & 1)

void lcd_init();
void lcd_byte_in(uint8_t in);

uint8_t  lcd_get_shift();
uint8_t* lcd_get_character_data();
uint8_t* lcd_get_text_data(uint8_t row);
uint8_t  lcd_get_symbol(uint8_t bbyte, uint8_t bbit);
uint8_t  lcd_get_7segment(uint8_t n);

void  lcd_set_symbol(uint8_t bbyte, uint8_t bbit, uint8_t val);
void set_text_memory(uint8_t pos, uint8_t len, uint8_t *data);
