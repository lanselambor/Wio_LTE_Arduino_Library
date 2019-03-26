#include "wio_tracker.h"


WioTracker wio = WioTracker();

void setup() {
  // Turn on Grove socket power
  pinMode(wio.ENABLE_VCCB_PIN, OUTPUT);
  digitalWrite(wio.ENABLE_VCCB_PIN, HIGH);
  
  Serial.begin(115200);
  Serial.println("Begin...");  
}

void loop() {
  Serial.println("Grove UART is sending message.");
  Serial.println("From Grove UART.");
  delay(1000);
  
}
