#include "wiolte_cat_1.h"


void setup() {
  _debugSerial.println("Begin...");
  wiolte.initialize();
  while(false == wio.Check_If_Power_On()){
    _debugSerial.println("Waitting for module to alvie...");
    delay(1000);
  }  
  _debugSerial.println("Power On O.K!");
}

void loop() {
  /* Debug */
  AT_bypass();
}
