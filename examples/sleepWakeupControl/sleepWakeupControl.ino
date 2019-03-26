#include "wio_tracker.h"


WioTracker wio = WioTracker();

void setup() {
  pinMode(wio.DTR_PIN, INPUT);

  Serial.println("Begin...");
  wio.initialize();
  while(false == wio.Check_If_Power_On()){
    Serial.println("Waitting for module to alvie...");
    delay(1000);
  }  
  Serial.println("Power On O.K!");
}

void loop() {
  Serial.println("sleep...");
  while(!wio.module_sleep()){
    Serial.println("Try to sleep...");
    delay(1000);    
  }
  Serial.println("sleep...");
  delay(4000);
  Serial.println("wakeup...");
  while(!wio.module_wakeup()){
    Serial.println("Try to wakeup...");
    delay(1000);    
  }
  Serial.println("wakeup...");
  delay(4000);
}