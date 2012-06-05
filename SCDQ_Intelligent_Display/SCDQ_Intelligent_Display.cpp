/*

Square 0.134" 4-Character
5x5 Dot Matrix Serial Input Dot Addressable Intelligent Display® Devices

Yellow SCDQ5541P/Q/R
Super-red SCDQ5542P/Q/R
Green SCDQ5543P/Q/R
High Efficiency Green SCDQ5544P/Q/R


*/

#include "SCDQ_Intelligent_Display.h"

#include "font5x5.h"
// The font is stored in program memory:
#include <avr/pgmspace.h>
#include <string.h> 


SCDQ_Intelligent_Display::SCDQ_Intelligent_Display(byte pin_matrix_load, byte pin_matrix_sdata, byte pin_matrix_sdclk) {

 byte rows[MATRIX_ROWS];
 pinMode(pin_matrix_load, OUTPUT);
  _pin_matrix_load = pin_matrix_load;
 pinMode(pin_matrix_sdata, OUTPUT);
  _pin_matrix_sdata = pin_matrix_sdata;
 pinMode(pin_matrix_sdclk, OUTPUT);
  _pin_matrix_sdclk = pin_matrix_sdclk;
  //Set up matrix
  clearMatrix();
  setBrightness(0);
  _matrix_dots_text_column_pos = 0;
}

SCDQ_Intelligent_Display::
~SCDQ_Intelligent_Display() {
//Nothing to do
}


void SCDQ_Intelligent_Display::setScrollText(char t[]) {
  strcpy(_scroll_text, t); 
}

void  SCDQ_Intelligent_Display::scrollRight() {
    int max_colums = (1 * MATRIX_DIGITS * MATRIX_COLUMNS) +  (MATRIX_COLUMNS * strlen(_scroll_text));
    _matrix_dots_text_column_pos = _matrix_dots_text_column_pos +1;
    if (_matrix_dots_text_column_pos > max_colums) {
        _matrix_dots_text_column_pos = 0 ;
    }
    _setDotsforScrollText();
}


void SCDQ_Intelligent_Display::clearMatrix() {
  writeMatrixHex(B11000000);
  //_matrix_dots[MATRIX_DIGITS][MATRIX_ROWS][MATRIX_COLUMNS];
  for (byte d = 0; d < MATRIX_DIGITS; d++) {
    for (byte r = 0; r < MATRIX_ROWS; r++) {
      for (byte c = 0; c < MATRIX_COLUMNS; c++) {
        _matrix_dots[d][r][c] = false;
      }
    }
  }

}


void SCDQ_Intelligent_Display::setBrightness(byte b) {
  if (b>0 && b<7) {
    writeMatrixHex(B11110000 + b);
  }
}


void SCDQ_Intelligent_Display::_selectMatrixDigit(byte p) {
  writeMatrixHex(B10100000 + p);
}

void SCDQ_Intelligent_Display::writeMatrixHex(byte data) {
  const int timeBetweenWritesMilli =  2;
  const int clockPeriodMilli = 2; 
  byte mask = 1;

  digitalWrite(_pin_matrix_sdclk, LOW);
  digitalWrite(_pin_matrix_load, LOW);
  for (byte i = 0; i < 8; i++) {
    if (mask & data) {
      digitalWrite(_pin_matrix_sdata, HIGH);
    }
    else {
      digitalWrite(_pin_matrix_sdata, LOW);
    }

    digitalWrite(_pin_matrix_sdclk, HIGH);
//    delay(clockPeriodMilli);
    digitalWrite(_pin_matrix_sdclk, LOW);
    mask = mask*2;
  }
  digitalWrite(_pin_matrix_load, HIGH);
//  delay(timeBetweenWritesMilli);

}


void SCDQ_Intelligent_Display::_setDotsforScrollText() {
  // The scroll function will stop columns from overflowing...
  bool _matrix_dot_tmp[MATRIX_DIGITS+1][MATRIX_ROWS][MATRIX_COLUMNS];
  byte rows[MATRIX_ROWS];
  int offset = (_matrix_dots_text_column_pos / MATRIX_COLUMNS) -  MATRIX_DIGITS;
  int offset_mod = MATRIX_COLUMNS - (_matrix_dots_text_column_pos % MATRIX_COLUMNS) -1;
  byte mask;

  for (int d = 0; d < MATRIX_DIGITS+1; d++) {
    if ((offset+d)<0) {
      _getRowsForChar(rows, ' ', false);
    }
    else if ((offset+d)>=strlen(_scroll_text)) {
      _getRowsForChar(rows, ' ', false);
    }
    else {
      _getRowsForChar(rows, _scroll_text[(offset+d)], false);
	    }
    for (int r = 0; r < MATRIX_ROWS; r++){
    mask = 1;
      for (byte c = 0; c < MATRIX_COLUMNS; c++) {
        if(rows[r] & mask) {
          _matrix_dot_tmp[d][r][c] = true;
        }
        else {
          _matrix_dot_tmp[d][r][c] = false;
        }
        mask=mask*2;
      }
    }
  }

// Offset by _matrix_dots_text_column_pos % MATRIX_COLUMNS
  for (byte d = 0; d < MATRIX_DIGITS; d++) {
    for (byte r = 0; r < MATRIX_ROWS; r++) {
      for (byte c = 0; c < MATRIX_COLUMNS; c++) {
        if (c + offset_mod > MATRIX_COLUMNS-1) {
          _matrix_dots[d][r][c] = _matrix_dot_tmp[d][r][c+offset_mod-MATRIX_COLUMNS];
        }
        else {
          _matrix_dots[d][r][c] = _matrix_dot_tmp[d+1][r][c+offset_mod];
        }
      }
    }
  }
  _writeStoredDots();
}



void SCDQ_Intelligent_Display::_getRowsForChar(byte rows[MATRIX_ROWS], char c, bool header) {
  byte bit_mask;
  if (header) {
    bit_mask = B11111111;
  }
  else {
    bit_mask = B00011111;
  }
  for (int i = 0; i < MATRIX_ROWS; i++) {
//    rows[i] =                 Font5x5[((c- 0x20) * MATRIX_ROWS) + i] & bit_mask;
      rows[i] = (pgm_read_byte(&Font5x7[((c- 0x20) * MATRIX_ROWS) + i]))  & bit_mask;

  }
}


void SCDQ_Intelligent_Display::writeFourChars(char text[5]) {
  byte rows[MATRIX_ROWS];
  for (int i = 0; i < 4; i++) {
    _selectMatrixDigit(i);
    _getRowsForChar(rows, text[i], 1);
    for (int r = 0; r < MATRIX_ROWS; r++) {
      writeMatrixHex(rows[r]);
    }
  }
}


void SCDQ_Intelligent_Display::_writeStoredDots() {
  //_matrix_dots[]
  byte row_bits = pow(2, MATRIX_ROWS);
  byte d;
  byte mask;
  for (int digit = 0; digit < MATRIX_DIGITS; digit++) {
    _selectMatrixDigit(digit);
    for (int r = 0; r < MATRIX_ROWS; r++) {
      d = 0;
      mask = 1;
      for (int c = 0; c < MATRIX_COLUMNS; c++) {
        if (_getMatrixDots(digit, r, c)) {
          d = d + mask;
        }
        mask=mask*2;
      }
      d = d + (row_bits*r);
      writeMatrixHex(d);
    }
  }
}


void SCDQ_Intelligent_Display::setDot(int d, int r, int c, bool dot, bool write) {
  _matrix_dots[d][MATRIX_ROWS-r-1][MATRIX_COLUMNS-c-1] = dot;
  if (write) {
    _writeStoredDots();
  }
}


bool SCDQ_Intelligent_Display::_getMatrixDots(int d, int r, int c) {
  return _matrix_dots[d][r][c];
}


