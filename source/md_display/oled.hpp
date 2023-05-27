/*
    Minimal SSD1306 OLED display routines and driver code

    Based on code from:

    GyverOled
    AlexGyver, alex@alexgyver.ru
    https://alexgyver.ru/
    MIT License

*/

#include <Wire.h>
#include "sony_font.h"

#define SSD1306_128x32   0
#define SSD1306_128x64   1

#define OLED_I2C 0
#define OLED_SPI 1

#define OLED_NO_BUFFER 0
#define OLED_BUFFER    1

#define OLED_CLEAR  0
#define OLED_FILL   1
#define OLED_STROKE 2

#define BUF_ADD      0
#define BUF_SUBTRACT 1
#define BUF_REPLACE  2

#define BITMAP_NORMAL 0
#define BITMAP_INVERT 1


#define OLED_WIDTH              128
#define OLED_HEIGHT_32          0x02
#define OLED_HEIGHT_64          0x12
#define OLED_64                 0x3F
#define OLED_32                 0x1F

#define OLED_DISPLAY_OFF        0xAE
#define OLED_DISPLAY_ON         0xAF

#define OLED_COMMAND_MODE       0x00
#define OLED_ONE_COMMAND_MODE   0x80
#define OLED_DATA_MODE          0x40
#define OLED_ONE_DATA_MODE      0xC0

#define OLED_ADDRESSING_MODE    0x20
#define OLED_HORIZONTAL         0x00
#define OLED_VERTICAL           0x01

#define OLED_NORMAL_V            0xC8
#define OLED_FLIP_V              0xC0
#define OLED_NORMAL_H            0xA1
#define OLED_FLIP_H              0xA0

#define OLED_CONTRAST           0x81
#define OLED_SETCOMPINS         0xDA
#define OLED_SETVCOMDETECT      0xDB
#define OLED_CLOCKDIV           0xD5
#define OLED_SETMULTIPLEX       0xA8
#define OLED_COLUMNADDR         0x21
#define OLED_PAGEADDR           0x22
#define OLED_CHARGEPUMP         0x8D

#define OLED_NORMALDISPLAY      0xA6
#define OLED_INVERTDISPLAY      0xA7

#define BUFSIZE_128x64 (128*64/8)
#define BUFSIZE_128x32 (128*32/8)

/////////////////////////////////////
// SSD1306 Initialization sequence //
/////////////////////////////////////

#define INIT_SEQ_LEN 15
static const uint8_t _oled_init[] = {
  OLED_DISPLAY_OFF,
  OLED_CLOCKDIV,
  0x80,    // value
  OLED_CHARGEPUMP,
  0x10,    // value -> 10 disable chargepump, 14 enable chargepump
  OLED_ADDRESSING_MODE,
  OLED_VERTICAL,
  OLED_NORMAL_H,
  OLED_NORMAL_V,
  OLED_CONTRAST,
  0x7F,    // contrast value -> can go to 7f
  OLED_SETVCOMDETECT,
  0x40,     // value
  OLED_NORMALDISPLAY,
  OLED_DISPLAY_ON,
};

template <int _TYPE> class OLED {

  public:
    OLED(uint8_t address = 0x3C) : _address(address) {}

    void init() {
      Wire.begin();

      beginCommand();
      for (uint8_t i = 0; i < INIT_SEQ_LEN; i++)
        sendByte(_oled_init[i]);
      endTransm();

      beginCommand();
      sendByte(OLED_SETCOMPINS);
      sendByte(_TYPE ? OLED_HEIGHT_64 : OLED_HEIGHT_32);
      sendByte(OLED_SETMULTIPLEX);
      sendByte(_TYPE ? OLED_64 : OLED_32);
      endTransm();

      setCursorXY(0, 0);
      setWindow(0, 0, _maxX, _maxRow);
    }

    void clear() {
      fill(0);
    }

    void setContrast(uint8_t value) {
      sendCommand(OLED_CONTRAST, value);
    }
    void setPower(bool mode)        {
      sendCommand(mode ? OLED_DISPLAY_ON : OLED_DISPLAY_OFF);
    }
    void flipH(bool mode)           {
      sendCommand(mode ? OLED_FLIP_H : OLED_NORMAL_H);
    }
    void invertDisplay(bool mode)   {
      sendCommand(mode ? OLED_INVERTDISPLAY : OLED_NORMALDISPLAY);
    }
    void flipV(bool mode)           {
      sendCommand(mode ? OLED_FLIP_V : OLED_NORMAL_V);
    }

    void drawChar(uint8_t chr, int x = -1, int y = -1) {
      if (x == -1) x = _x;
      if (y == -1) y = _y;

      if (chr < 16) {
        // ignore char mem for now
      } else {
        for (int yy = 0; yy < SONY_FONT_HEIGHT; yy++) {
          uint8_t row = _sonyFont[chr - 16][yy];
          for (int xx = 0; xx < SONY_FONT_WIDTH; xx++) {
            dot(x + xx * _scale, y + yy * _scale, (row >> (5 - xx)) & 1);
          }
        }
      }

      _x += SONY_FONT_WIDTH * _scale;
    }

    void drawString(const char* str, int x = -1, int y = -1) {
    
      if (x >= 0) _x = x;
      if (y >= 0) _y = y;

      while (*str != 0) {
        drawChar(*str);
        str++;
      }
    }

    void drawIcon7(uint8_t* icon, int x = -1, int y = -1) {
      if (x >= 0) _x = x;
      if (y >= 0) _y = y;

      for (int xx = 0; xx < 7; xx++) {
        uint8_t row = icon[xx];
        for (int yy = 0; yy < 7; yy++) {
          dot(x + xx * _scale, y + yy * _scale, (row >> yy) & 1);
        }
      }

      _x += 7 * _scale;
    }

    void home() {
      setCursorXY(0, 0);
    }

    void setCursor(int x, int y) {
      setCursorXY(x, y);
    }

    void setCursorXY(int x, int y) {
      _x = x;
      _y = y;
    }

    void setScale(uint8_t scale) {
      _scale = scale;
    }

    void invertText(bool inv) {
      _invState = inv;
    }
    void textMode(byte mode) {
      _mode = mode;
    }
    bool isEnd() {
      return (_y > _maxRow);
    }

    void dot(int x, int y, byte fill = 1) {
      for (int xx = 0; xx < _scale; xx++)
        for (int yy = 0; yy < _scale; yy++)
          dot_pure(x + xx, y + yy, fill);
    }

    void dot_pure(int x, int y, byte fill = 1) {
      if (x < 0 || x > _maxX || y < 0 || y > _maxY) return;

      bitWrite(_oled_buffer[_bufIndex(x, y)], y & 0b111, fill);
    }

    void fastLineH(int y, int x0, int x1, byte fill = 1) {
      _x = 0;
      _y = 0;

      if (x0 > x1)
        _swap(x0, x1);

      if (y < 0 || y > _maxY)
        return;

      if (x0 == x1) {
        dot(x0, y, fill);
        return;
      }

      x1++;
      x0 = constrain(x0, 0, _maxX);
      x1 = constrain(x1, 0, _maxX);

      for (int x = x0; x < x1; x++)
        dot(x, y, fill);
    }

    
    void fastLineV(int x, int y0, int y1, byte fill = 1) {
      _x = 0;
      _y = 0;

      if (y0 > y1)
        _swap(y0, y1);

      if (x < 0 || x > _maxX)
        return;

      if (y0 == y1) {
        dot(x, y0, fill);
        return;
      }

      y1++;
      y0 = constrain(y0, 0, _maxY);
      y1 = constrain(y1, 0, _maxY);

      for (int y = y0; y < y1; y++)
        dot(x, y, fill);
    }

    void fill(uint8_t data) {
      memset(_oled_buffer, data, _bufSize);
      setCursorXY(_x, _y);
    }

    void update() {
      setWindow(0, 0, _maxX, _maxRow);
      beginData();
      for (int i = 0; i < (_TYPE ? 1024 : 512); i++)
        sendByte(_oled_buffer[i]);
      endTransm();
    }

    void update(int x0, int y0, int x1, int y1) {
      y0 >>= 3;
      y1 >>= 3;

      setWindow(x0, y0, x1, y1);

      beginData();
      for (int x = x0; x < x1; x++)
        for (int y = y0; y < y1 + 1; y++)
          if (x >= 0 && x <= _maxX && y >= 0 && y <= _maxRow)
            sendByte(_oled_buffer[y + x * (_TYPE ? 8 : 4)]);
      endTransm();
    }

    void writeData(byte data, byte offsetY = 0, byte offsetX = 0, int mode = 0) {
      switch (mode) {
        case 0: _oled_buffer[_bufIndex(_x + offsetX, _y) + offsetY] |= data;
          break;
        case 1: _oled_buffer[_bufIndex(_x + offsetX, _y) + offsetY] &= ~data;
          break;
        case 2: _oled_buffer[_bufIndex(_x + offsetX, _y) + offsetY] = data;
          break;
      }
    }

    void sendByte(uint8_t data) {
      sendByteRaw(data);

      _writes++;
      if (_writes >= 16) {
        endTransm();
        beginData();
      }

    }

    void sendByteRaw(uint8_t data) {
      Wire.write(data);
    }

    void sendCommand(uint8_t cmd1) {
      beginOneCommand();
      sendByteRaw(cmd1);
      endTransm();
    }

    void sendCommand(uint8_t cmd1, uint8_t cmd2) {
      beginCommand();
      sendByteRaw(cmd1);
      sendByteRaw(cmd2);
      endTransm();
    }

    void setWindow(int x0, int y0, int x1, int y1) {
      beginCommand();
      sendByteRaw(OLED_COLUMNADDR);
      sendByteRaw(constrain(x0, 0, _maxX));
      sendByteRaw(constrain(x1, 0, _maxX));
      sendByteRaw(OLED_PAGEADDR);
      sendByteRaw(constrain(y0, 0, _maxRow));
      sendByteRaw(constrain(y1, 0, _maxRow));
      endTransm();
    }

    void beginData() {
      startTransm();
      sendByteRaw(OLED_DATA_MODE);
    }

    void beginCommand() {
      startTransm();
      sendByteRaw(OLED_COMMAND_MODE);
    }

    void beginOneCommand() {
      startTransm();
      sendByteRaw(OLED_ONE_COMMAND_MODE);
    }

    void endTransm() {
      Wire.endTransmission();
      _writes = 0;
    }

    void startTransm() {
      Wire.beginTransmission(_address);
    }


    // ====== PROPERTIES =======

    const uint8_t _address = 0x3C;
    const uint8_t _maxRow  = (_TYPE ? 8 : 4) - 1;
    const uint8_t _maxY    = (_TYPE ? 64 : 32) - 1;
    const uint8_t _maxX    = OLED_WIDTH - 1;

  private:

    bool _invState  = 0;
    bool _println   = false;
    bool _getn      = false;

    int _x = 0, _y = 0;
    uint8_t _lastChar;
    uint8_t _writes = 0;
    uint8_t _mode = 2;
    uint8_t _scale = 1;

    int _bufsizeX, _bufsizeY;
    int _bufX, _bufY;
    uint8_t *_buf_ptr;
    bool _buf_flag = false;

    const int _bufSize = _TYPE ? BUFSIZE_128x64 : BUFSIZE_128x32;
    uint8_t _oled_buffer[(_TYPE ? BUFSIZE_128x64 : BUFSIZE_128x32)];

    int _bufIndex(int x, int y) {
      return ((y) >> 3) + ((x) << (_TYPE ? 3 : 2));
    }

    void _swap(int& x, int& y) {
      int z = x;
      x = y;
      y = z;
    }

    bool _inRange(int x, int mi, int ma) {
      return x >= mi && x <= ma;
    }
};
