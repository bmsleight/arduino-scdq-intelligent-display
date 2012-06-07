#include <SCDQ_Intelligent_Display.h>

SCDQ_Intelligent_Display scdq(7,8,9);

int b=0;

void setup() {
  scdq.setScrollText("Jack, love my big wad of sphinx quartz! 0123456789");
}

void loop() {
  scdq.scrollRight();
  scdq.writeStoredDots();
  delay(25);
  scdq.setDot(0,0,0,1);
  scdq.writeStoredDots();
  delay(25);
  scdq.setDot(0,0,0,0);
  scdq.writeStoredDots();
  delay(25);
}

