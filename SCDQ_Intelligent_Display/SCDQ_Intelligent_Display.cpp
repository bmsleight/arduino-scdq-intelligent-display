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

SCDQ_Intelligent_Display::~SCDQ_Intelligent_Display() {
}

void SCDQ_Intelligent_Display::clearMatrix() {
  writeMatrixHex(B11000000);
  for (byte d = 0; d < MATRIX_DIGITS; d++) {
    for (byte r = 0; r < MATRIX_ROWS; r++) {
      _stored_rows[d][r] = 0;
    }
  }
}

void SCDQ_Intelligent_Display::setBrightness(byte b) {
  if (b>0 && b<7) {
    writeMatrixHex(B11110000 + b);
  }
}

void SCDQ_Intelligent_Display::setDot(byte d, byte r, byte c, bool dot) {
// _stored_rows[d][r] = 0;
  byte mask = 1 << (MATRIX_COLUMNS - c - 1);
  if (dot) {
    _stored_rows[d][MATRIX_ROWS - r - 1] |= mask;
  }
  else {
    mask = ~mask;
    _stored_rows[d][MATRIX_ROWS - r - 1] &= mask;
  }
}

void SCDQ_Intelligent_Display::setScrollText(char t[]) {
  strcpy(_scroll_text, t);
}

void SCDQ_Intelligent_Display::scrollRight() {
    int max_colums = (1 * MATRIX_DIGITS * MATRIX_COLUMNS) +  (MATRIX_COLUMNS * strlen(_scroll_text));
    _matrix_dots_text_column_pos = _matrix_dots_text_column_pos +1;
    if (_matrix_dots_text_column_pos > max_colums) {
        _matrix_dots_text_column_pos = 0 ;
    }
    _setDotsforScrollText();
}

void SCDQ_Intelligent_Display::writeMatrixHex(byte data) {
//  const int timeBetweenWritesMilli =  2;
//  const int clockPeriodMilli = 2; 
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

void SCDQ_Intelligent_Display::writeFourChars(char text[5]) {
  byte rows[MATRIX_ROWS];
  for (int i = 0; i < 4; i++) {
    _getRowsForChar(rows, text[i], 0);
    for (int r = 0; r < MATRIX_ROWS; r++) {
      _stored_rows[i][r] = rows[r];
    }
  }
}

void SCDQ_Intelligent_Display::writeStoredDots() {
  //_matrix_dots[]
  byte d;
  byte mask;
  for (int d = 0; d < MATRIX_DIGITS; d++) {
    _selectMatrixDigit(d);
    for (int r = 0; r < MATRIX_ROWS; r++) {
      writeMatrixHex( _stored_rows[d][r] + ( (1<<MATRIX_ROWS) *r) );
    }
  }
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

bool SCDQ_Intelligent_Display::_getMatrixDots(byte d, byte r, byte c) {
  // TO DO FIX ME
  return true;
}

void SCDQ_Intelligent_Display::_selectMatrixDigit(byte p) {
  writeMatrixHex(B10100000 + p);
}

void SCDQ_Intelligent_Display::_setDotsforScrollText() {
  byte rows[MATRIX_ROWS];
  int offset = (_matrix_dots_text_column_pos / MATRIX_COLUMNS) - MATRIX_COLUMNS; //int as can be -ve
  byte offset_mod = (_matrix_dots_text_column_pos % MATRIX_COLUMNS);

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
    for (int r = 0; r < MATRIX_ROWS; r++) {
      _stored_rows[d][r] = rows[r];
    }
  }

  if (offset_mod>0) {
    for (int d = 0; d < MATRIX_DIGITS; d++) {
      for (int r = 0; r < MATRIX_ROWS; r++) {
        _stored_rows[d][r] <<= offset_mod; // Shift (scroll) to the right
        _stored_rows[d][r] &= ((1 << MATRIX_DIGITS) - 1); // Zero the digits greater than 5
        _stored_rows[d][r] += (_stored_rows[d+1][r] >> (MATRIX_DIGITS -offset_mod +1)); // add from the right
      }
    }
  }
}


