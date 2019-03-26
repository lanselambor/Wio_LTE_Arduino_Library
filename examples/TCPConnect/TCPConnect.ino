#include "ethernet.h"

Ethernet eth = Ethernet();


const char apn[10] = "CMNET";
// const char apn[10] = "UNINET";
const char URL[100] = "os.mbed.com";
char http_cmd[100] = "GET /media/uploads/mbed_official/hello.txt HTTP/1.0\n\r\n\r";
int port = 80;

int ret = 0;


void setup() {
  eth.initialize();
  Serial.begin(115200);
  Serial.println("Begin...");
  eth.Power_On();
  while(false == eth.Check_If_Power_On()){
    Serial.println("Waitting for module to alvie...");
    delay(1000);
  }

  while(!eth.init()){
    delay(1000);
    Serial.println("Accessing network...");
  }
  Serial.println("Initialize done...");

  eth.join(apn);
  Serial.print("\n\rIP: ");
  Serial.print(eth.ip_string);

  if(eth.connect(URL, port, TCP)) 
  {
    eth.write(http_cmd); 
    while(_moduleSerial.available()){
        Serial.write(_moduleSerial.read());
    }    
    eth.close(1);
  } 
  else {
    Serial.println("Connect Error!");
  }
  
}

void loop() {
  /* Debug */
  AT_bypass();
}
