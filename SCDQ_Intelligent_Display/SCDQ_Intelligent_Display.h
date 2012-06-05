#ifndef SCDQ_Intelligent_Display_H
#define SCDQ_Intelligent_Display_H

#define MATRIX_DIGITS 4 
#define MATRIX_ROWS 5 
#define MATRIX_COLUMNS 5 
#define MATRIX_TEXT 5 // MATRIX_DIGITS + 1
#define MATRIX_TEXT_LENGTH 247 //(255-MATRIX_DIGITS-MATRIX_DIGITS) 

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


class SCDQ_Intelligent_Display {
  public:
    SCDQ_Intelligent_Display(byte pin_matrix_load, byte pin_matrix_sdata, byte pin_matrix_sdclk);
    ~SCDQ_Intelligent_Display();
    void clearMatrix();
    void setBrightness(byte b);
    void setDot(int d, int r, int c, bool dot, bool write);
    void setScrollText(char t[]);
    void scrollRight();
    void writeMatrixHex(byte data);
    void writeFourChars(char text[5]);

  private:
    void _getRowsForChar(byte rows[MATRIX_ROWS], char c, bool header);
    bool _getMatrixDots(int d, int r, int c);
    void _selectMatrixDigit(byte p);
    void _setDotsforScrollText();
    void _writeStoredDots();

    byte _pin_matrix_load;
    byte _pin_matrix_sdata;
    byte _pin_matrix_sdclk;
    int  _matrix_dots_text_column_pos;
    bool _matrix_dots[MATRIX_DIGITS][MATRIX_ROWS][MATRIX_COLUMNS];
    char _scroll_text[MATRIX_TEXT_LENGTH];

};

#endif


