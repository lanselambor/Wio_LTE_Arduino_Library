
#include "Quectel_LTE.h"

Quectel_LTE LTE;

// const char *APN = "CMNET";
const char *APN = "UNINET";
const char *USERNAME = "";
const char *PASSWD = "";

const char *HOST = "arduino.cc";
int port = 80;
char *param = "GET /asciilogo.txt HTTP/1.1";

char recvData[1024] = {'\0'};
uint16_t recvSize = 0;

void setup() { 
  delay(200); 
  Serial.begin(115200);
  Serial.println("## TCP Client Demo");

  LTE.initialize();

  Serial.println("## Waitting for module to alvie...");
  if(!LTE.isAlive(20000)) 
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

  if(!LTE.Deactivate())
  {
    Serial.println("### Failed to deactive network");
    return;
  }

  Serial.println("## Active network");
  if(!LTE.Activate(APN, USERNAME, PASSWD))
  {
    Serial.println("### Failed to active network");
    return;
  }

  if(!LTE.getIPAddr())
  {
    Serial.println("### ERROR:getIPAddr");
    return;
  }
  Serial.print("## IP:");
  Serial.println(LTE._str_ip);

  Serial.println("## Socket open");
  if(!LTE.sockOpen(HOST, port, TCP)) 
  {
    Serial.println("### Socket open error");
    return;
  }
  
  char data[256] = {'\0'};
  uint32_t dataLen= 0;
 
  sprintf(data, "%s\n", param); dataLen = strlen(data);
  sprintf((char *)(data+dataLen), "Host: %s\n", HOST); dataLen = strlen(data);  
  sprintf((char *)(data+dataLen), "Accept: */*\n"); dataLen = strlen(data);  
  sprintf((char *)(data+dataLen), "User-Agent: QUECTEL_MODULE\n"); dataLen = strlen(data);  
  sprintf((char *)(data+dataLen), "Connection: Keep-Alive\n"); dataLen = strlen(data);  
  sprintf((char *)(data+dataLen), "Content-Type: application/x-www-form-urlencoded\n\n");

  Serial.println("## Socket write\r\n");
  Serial.print("## HTTP Header: ");
  Serial.print(strlen(data));
  Serial.println(" Bytes");
  Serial.println(data);
  if(!LTE.sockWrite(0, data))
  {
    Serial.println("### Socket write error");
    return;
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
}

void loop() {
  /* Debug */
  LTE._AtSerial.AT_Bypass();
}
