#include "Quectel_LTE.h"


Quectel_LTE LTE;

void setup() {
  delay(200);
  Serial.begin(115200);  
  Serial.println("## Demo AT Bypass");
  
  LTE.initialize();
  if(LTE.isAlive(1000))
  {
    Serial.println("### Module not alived");
    return;
  }
}

void loop() {
  /* Debug */
  LTE._AtSerial.AT_Bypass();
}
