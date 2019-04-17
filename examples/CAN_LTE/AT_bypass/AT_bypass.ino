#include "wio_tracker.h"


WioTracker wio = WioTracker();

void setup() {
  Serial.println("Begin...");
  wio.initialize();
  while(false == wio.Check_If_Power_On()){
    Serial.println("Waitting for module to alvie...");
    delay(1000);
  }  
  Serial.println("Power On O.K!");
}

void loop() {
  /* Debug */
  AT_Bypass();
}
