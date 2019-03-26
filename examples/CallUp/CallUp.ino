#include "wio_tracker.h"

WioTracker wio = WioTracker();

void setup() {
  Serial.println("Wait for power on...");
  wio.initialize();  

  while(!wio.init()){
    delay(1000);
    Serial.println("Accessing network...");
  }
  Serial.println("Initialize done...");

  bool ret = wio.waitForNetworkRegister();
  if(true == ret){
      Serial.println("Network accessed!");
  }else {
      Serial.println("Network failed!");
      return;
  }

  // Make a phone call
  wio.callUp("xxxxxxxx");
  Serial.println("Calling...");

}

void loop() {
  /* Debug */
  AT_bypass();
}
