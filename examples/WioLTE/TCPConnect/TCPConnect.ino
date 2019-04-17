#include "WioLTE.h"
#include "Quectel_LTE.h"

WioLTE Wio;
Quectel_LTE LTE;

// const char apn[10] = "CMNET";
const char apn[10] = "UNINET";
const char URL[100] = "45.77.14.150";
char http_cmd[100] = "GET /media/uploads/mbed_official/hello.txt HTTP/1.0\n\r\n\r";
int port = 5000;

char data[128] = {'\0'};
uint16_t inSize = 0;

void setup() {  
  delay(200);
  SerialUSB.println("## TCP Client Demo");
  
  Wio.initialize();  
  Wio.reset();  

  SerialUSB.println("## Waitting for module to alvie..."); 
  if(!LTE.isAlive(10000)) 
  {
    SerialUSB.println("### Module not alive");
    return;
  }

  SerialUSB.println("## Wait for network ready");
  while(!LTE.waitForNetworkRegister())
  {
    SerialUSB.println("### Network not ready");
    return;
  }

  SerialUSB.println("## Active network");
  if(!LTE.Activate(apn, "", ""))
  {
    SerialUSB.println("### Failed to active network");
    return;
  }

  if(!LTE.getIPAddr())
  {    
    return;
  }
  SerialUSB.print("### IP: ");
  SerialUSB.println(LTE._str_ip);

  if(!LTE.getOperator())
  {
    return;
  }
  SerialUSB.print("### Operator: ");
  SerialUSB.println(LTE._operator);

  
  SerialUSB.println("## Socket open");
  if(!LTE.sockOpen(URL, port, TCP)) 
  {
    SerialUSB.println("### Socket open error");
    goto sock_err;
  }

  SerialUSB.println("## Socket write");
  if(!LTE.sockWrite(0, http_cmd))
  {
    SerialUSB.println("### Socket write error");
    goto sock_err;
  }
  delay(1000);

  SerialUSB.println("## Socket receive");   
  while(true)
  {
    Wio._AtSerial.CleanBuffer(data, 128);
    inSize = LTE.sockReceive(0, data, 128, 5000);
    SerialUSB.println(data);    
    if(128 > inSize)
    {
      goto sock_err;
    } 
  } 

sock_err:
  SerialUSB.println("## Socket close");
  if(!LTE.sockClose(0))
  {
    SerialUSB.println("### Socket close error");
  }  
}

void loop() {
  /* Debug */
  Wio._AtSerial.AT_Bypass();
}
