#include "Quectel_LTE.h"

Quectel_LTE LTE;

void setup() {
  delay(1000);
  Serial.println("## Demo getting module rssi");
  
  LTE.initialize(); 
  
  if(!LTE.isAlive(10000))
  {
    Serial.println("### ERROR:module not alive");
    return;
  }

  if(!LTE.waitForNetworkRegister())
  {
    Serial.println("### ERROR:network not registered");
    return;
  }
}
void loop() {
  int rssi;
  if(!LTE.getSignalStrength(&rssi))
  {
    Serial.println("### ERROR:getSignalStrength");
  }
  else
  {
    Serial.print("## RSSI: ");
    Serial.print(rssi);
    Serial.print(", ");
    
    if(rssi == 99) {
      rssi = 9999;
    }
    else if(rssi == 0) {
      rssi = -113;
    }
    else if(rssi == 1) {
      rssi = -111;
    }
    else if(rssi >= 2 && rssi <= 30) {
      rssi = -109 + 2*(rssi-2);
    }
    else if(rssi > 30) {
      rssi = -51 + (rssi-30)/2;   // approximate
    }
    
    Serial.print(rssi);
    Serial.println("dBm");
  }  

  delay(1000);
}
