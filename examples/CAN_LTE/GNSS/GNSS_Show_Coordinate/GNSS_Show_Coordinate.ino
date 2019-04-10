#include "gnss.h"


GNSS gnss = GNSS();

void setup() {
  gnss.initialize();
  while(false == gnss.Check_If_Power_On()){
    Serial.println("Waitting for module to alvie...");
    delay(1000);
  } 
  Serial.println("\n\rPower On!");

  if(!(gnss.open_GNSS())){
    Serial.println("\n\rGNSS init failed!");
    return;
  }

  Serial.println("Open GNSS OK.");
  delay(2000);
}

void loop() {
  if(gnss.getCoordinate()){
    Serial.println();
    Serial.print("GNSS: \r\n");

    // Output double type
    Serial.print("Data type in double: ");
    Serial.print(gnss.longitude, 6);
    Serial.print(",");
    Serial.println(gnss.latitude, 6);
    
    // Output char* type
    Serial.print("Data type in string: ");
    Serial.print(gnss.str_longitude);
    Serial.print(",");
    Serial.println(gnss.str_latitude);
  } else{
    Serial.print("...");
  }
}
