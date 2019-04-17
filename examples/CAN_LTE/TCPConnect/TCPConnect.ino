
#include "Quectel_LTE.h"

Quectel_LTE LTE;

// const char apn[10] = "CMNET";
const char apn[10] = "UNINET";
const char URL[100] = "45.77.14.150";
char http_cmd[100] = "GET /media/uploads/mbed_official/hello.txt HTTP/1.0\n\r\n\r";
int port = 5000;

char data[128] = {'\0'};
uint16_t inSize = 0;

void setup() {  
  Serial.begin(115200);
  while(!Serial);
  Serial.println("## TCP Client Demo");

  LTE.initialize();

  Serial.println("## Waitting for module to alvie..."); 
  if(!LTE.isAlive(10000)) 
  {
    Serial.println("## Module not alive");
    return;
  }

  Serial.println("## Wait for network ready");
  while(!LTE.waitForNetworkRegister())
  {
    Serial.println("## Network not ready");
    return;
  }

  Serial.println("## Active network");
  if(!LTE.Activate(apn, "", ""))
  {
    Serial.println("### Failed to active network");
    return;
  }

  if(!LTE.getIPAddr())
  {    
    return;
  }
  Serial.print("## IP: ");
  Serial.println(LTE._str_ip);

  if(!LTE.getOperator())
  {
    return;
  }
  Serial.print("## Operator: ");
  Serial.println(LTE._operator);

  
  Serial.println("## Socket open");
  if(!LTE.sockOpen(URL, port, TCP)) 
  {
    Serial.println("### Socket open error");
    goto sock_err;
  }

  Serial.println("## Socket write");
  if(!LTE.sockWrite(0, http_cmd))
  {
    Serial.println("### Socket write error");
    goto sock_err;
  }
  delay(1000);

  Serial.println("## Socket receive");   
  while(true)
  {
    LTE._AtSerial.CleanBuffer(data, 128);
    inSize = LTE.sockReceive(0, data, 128, 2000);
    Serial.println(data);    
    if(128 > inSize)
    {      
      break;
    } 
  } 

  Serial.println("## Socket close");
  if(!LTE.sockClose(0))
  {
    Serial.println("### Socket close error");
  }

sock_err:
  Serial.println("## Socket close");
  if(!LTE.sockClose(0))
  {
    Serial.println("### Socket close error");
  }  
}

void loop() {
  /* Debug */
  LTE._AtSerial.AT_Bypass();
}