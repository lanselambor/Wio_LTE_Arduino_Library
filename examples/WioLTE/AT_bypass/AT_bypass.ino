#include "WioLTE.h"
#include "Quectel_LTE.h"

WioLTE Wio;
Quectel_LTE LTE;

void setup() {  
  delay(200);
  SerialUSB.println("## AT Bypass Mode");
  
  Wio.initialize();  
  Wio.reset();  

  SerialUSB.println("Waitting for module to alvie..."); 

  if(!LTE.isAlive(10000)) 
  {
    SerialUSB.println("### Module not alived");
    return;
  }

  SerialUSB.println("## Begin AT Bypass Mode");    
}

void loop() {
  /* Debug */
  Wio._AtSerial.AT_Bypass();
}
