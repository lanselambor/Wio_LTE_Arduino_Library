
#include "Quectel_LTE.h"

Quectel_LTE LTE;
WioLTE Wio;

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
  SerialUSB.begin(115200);
  SerialUSB.println("## TCP Client Demo");

  Wio.initialize();
  Wio.reset();

  SerialUSB.println("## Waitting for module to alvie...");
  if(!LTE.isAlive(20000)) 
  {
    SerialUSB.println("## Module not alive");
    return;
  }

  SerialUSB.println("## Wait for network ready");
  while(!LTE.waitForNetworkRegister())
  {
    SerialUSB.println("## Network not ready");
    return;
  }

  if(!LTE.Deactivate())
  {
    SerialUSB.println("### Failed to deactive network");
    return;
  }

  SerialUSB.println("## Active network");
  if(!LTE.Activate(APN, USERNAME, PASSWD))
  {
    SerialUSB.println("### Failed to active network");
    return;
  }

  if(!LTE.getIPAddr())
  {
    SerialUSB.println("### ERROR:getIPAddr");
    return;
  }
  SerialUSB.print("## IP:");
  SerialUSB.println(LTE._str_ip);

  SerialUSB.println("## Socket open");
  if(!LTE.sockOpen(HOST, port, TCP)) 
  {
    SerialUSB.println("### Socket open error");
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

  SerialUSB.println("## Socket write\r\n");
  SerialUSB.print("## HTTP Header: ");
  SerialUSB.print(strlen(data));
  SerialUSB.println(" Bytes");
  SerialUSB.println(data);
  if(!LTE.sockWrite(0, data))
  {
    SerialUSB.println("### Socket write error");
    return;
  }
  delay(1000);

  SerialUSB.println("## Socket receive");   
  SerialUSB.println("********** request page ************");
  while(true)
  {
    LTE._AtSerialUSB.CleanBuffer(recvData, 1024);
    recvSize = LTE.sockReceive(0, recvData, 128); 
    char *p = (char *)recvData;
    while((*p++) != '\0') SerialUSB.write(*p);    
    if(128 > recvSize)
    {      
      break;
    } 
  }  
  SerialUSB.println("\r\n**************** END ***************");

  SerialUSB.println("## Socket close");
  if(!LTE.sockClose(0))
  {
    SerialUSB.println("### Socket close error");
  }
}

void loop() {
  /* Debug */
  LTE._AtSerialUSB.AT_Bypass();
}