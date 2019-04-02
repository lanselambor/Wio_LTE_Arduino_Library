#include "LTEClient.h"

LTEClient client;

void setup()
{
  client.Init();

  Serial.begin(115200);
  while(!Serial);

  if(!client.Deactivate()) 
  {
    Log_error("Deactive error...");
    return;
  }


  if(!client.Activate("CMNET","",""))
  {
    Log_error("Active error...");
    return;
  }
  
  client.getOperator();
  
  // if(!client.getIPAddr());
  // {
  //   Log_error("GetIPAddr error...");
  //   return;    
  // }

  Log_info("APN: " + String(client._apn));
  Log_info("Local IP: " + String(client._str_ip));
  Log_info("Operator: " + String(client._operator));

  Log_info("Start AT commands loop");
}

void loop()
{
  AT_bypass();
}


