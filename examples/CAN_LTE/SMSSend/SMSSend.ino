#include "wio_tracker.h"

char message[128] = "Hello from Wio Traker!";

WioTracker wio = WioTracker();

void setup() {  
  wio.initialize();
  Serial.begin(115200);
  Serial.println("Power On!");
  while(!Serial);

  if(!wio.waitForNetworkRegister())
  {
    Serial.println("Network error!");
    return;
  } else {
    Serial.println("Network ready!");
  }
  
  // Change xxxxxxxxxxx to your test phone number
  if(wio.sendSMS("13750024343", message))
  {
    Serial.println("Send OK!");
  }
  else 
  {
    Serial.println("Send Error!");
  }

}

void loop() {
  AT_Bypass();
}