#include "gnss.h"

// Instance of GNSS class
GNSS gnss = GNSS();

void setup(){	
	// Module power on
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

void loop(){
  gnss.dataFlowMode();
  delay(2000);
}
