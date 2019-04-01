#include "wio_tracker.h"

WioTracker wio = WioTracker();

void setup() {
  wio.initialize();
  Serial.println("Power On!");
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

}
void loop() {
  int rssi;
  wio.getSignalStrength(&rssi);
  Serial.print("RSSI: ");
  Serial.print(rssi);
  Serial.print(" ");
   
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
  delay(1000);
}
