#include "wio_tracker.h"

uint16_t newSMSNumber = -1;
char message[128];
char phone[32];
char dateTime[32];


WioTracker wio = WioTracker();

void setup() {
  wio.Power_On();
  Serial.println("Power On!");
  Serial.println("Wait for network registered...");

  if(!wio.waitForNetworkRegister())
  {
    Serial.println("Network error!");
    return;
  } else {
    Serial.println("Network ready!");
  }
  wio.readAllRecUnreadSMS();  // Set all "REC UNREAD SMS" to "REC READ SMS"
}

void loop() {
  int id = wio.detectRecUnreadSMS();
  if(-1 != id){
    newSMSNumber = id;
    wio.readSMS(newSMSNumber, message, 128, phone, dateTime);
    Serial.println("++++++++++++++ Start +++++++++++++++++");
    Serial.print("From: ");
    Serial.println(phone);
    Serial.print("Date: ");
    Serial.println(dateTime);
    Serial.println(message);
    Serial.println("++++++++++++++++ End +++++++++++++++");
  } else {
    Serial.println("Waiting for new SMS!");
  }

  delay(1000);
}
