
#include "Quectel_LTE.h"

Quectel_LTE LTE;

// const char *APN = "CMNET";
const char *APN = "UNINET";
const char *USERNAME = "";
const char *PASSWD = "";

const char *HOST = "example.com";
int port = 80;

char *param = "GET / HTTP/1.1\r\n\r\n";
// "Host: example.com\r\n"
// "Connection:  keep-alive\r\n"
// "Upgrade-Insecure-Requests: 1\r\n"
// "Accep:  text/html,application/xhtml+xml,application/xml\r\n"
// "Accept-Encoding: gzip, deflate\r\n"
// "Accept-Language: en\r\n"
// "Content-Type: application/x-www-form-urlencoded\r\n"
// "cache-control: no-cache\r\n"
// "Postman-Token: d08eefe0-616d-4afe-964d-45e0f5647e5c\r\n";

char recvData[1024] = {'\0'};
uint16_t recvSize = 0;

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
  if(!LTE.Activate(APN, USERNAME, PASSWD))
  {
    Serial.println("### Failed to active network");
    return;
  }

  Serial.println("## Socket open");
  if(!LTE.sockOpen(HOST, port, TCP)) 
  {
    Serial.println("### Socket open error");
    goto Exit;
  }

  Serial.println("## Socket write");
  if(!LTE.sockWrite(0, param))
  {
    Serial.println("### Socket write error");
    goto Exit;
  }
  delay(1000);

  Serial.println("## Socket receive");   
  Serial.println("********** request page ************");
  while(true)
  {
    LTE._AtSerial.CleanBuffer(recvData, 1024);
    recvSize = LTE.sockReceive(0, recvData, 128); 
    char *p = (char *)recvData;
    while((*p++) != '\0') Serial.write(*p);    
    if(128 > recvSize)
    {      
      break;
    } 
  }  
  Serial.println("\r\n**************** END ***************");

  Serial.println("## Socket close");
  if(!LTE.sockClose(0))
  {
    Serial.println("### Socket close error");
  }
  return;   

Exit:
  Serial.println("## Socket close");
  if(!LTE.sockClose(0))
  {
    Serial.println("### Socket close error");
  }  
  return;
}

void loop() {
  /* Debug */
  LTE._AtSerial.AT_Bypass();
}