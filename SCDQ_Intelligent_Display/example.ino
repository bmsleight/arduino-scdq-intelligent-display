#include <SCDQ_Intelligent_Display.h>

SCDQ_Intelligent_Display scdq(7,8,9);

int b=0;

void setup() {
  scdq.setScrollText("Hello World 1234567890 !,. ");
}

void loop() {
  delay(50);
  scdq.setDot(0,0,0,1,1);
  delay(50);
  scdq.setDot(0,0,0,0,1);
  scdq.scrollRight();
  scdq.setBrightness(b);
  b=b+1;
  if (b>7) {
    b = 0;
  }
}

