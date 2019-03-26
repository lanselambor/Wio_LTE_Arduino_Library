#include "gnss.h"


char nmea_sentence[192];
char nmea_GSV_sentence[512];
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
  gnss.enable_NMEA_mode();  // Set output sentence in NMEA mode
}

void loop() {  
  clean_buffer(nmea_sentence, 192);
  gnss.read_NMEA(GGA, nmea_sentence);
  Serial.print(nmea_sentence);

  clean_buffer(nmea_sentence, 192);
  gnss.read_NMEA(RMC, nmea_sentence);
  Serial.print(nmea_sentence);

  clean_buffer(nmea_sentence, 192);
  gnss.read_NMEA(GSA, nmea_sentence);
  Serial.print(nmea_sentence);

  clean_buffer(nmea_sentence, 192);
  gnss.read_NMEA(VTG, nmea_sentence);
  Serial.print(nmea_sentence);

  clean_buffer(nmea_GSV_sentence, 512);
  gnss.read_NMEA_GSV(nmea_sentence);
  Serial.print(nmea_sentence);
  
  Serial.println("\r\n");

  delay(1000);
}
