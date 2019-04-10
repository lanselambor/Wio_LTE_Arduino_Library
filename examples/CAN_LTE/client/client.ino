#include "EC2xClient.h"

EC2xClient ec2x;

void setup()
{
  ec2x.Init();
  // Wiolte.powerReset();

  Serial.begin(115200);
  while(!Serial);

  if(!ec2x.Deactivate()) 
  {
    Log_error("Deactive error...");
    return;
  }


  if(!ec2x.Activate("CMNET","",""))
  {
    Log_error("Active error...");
    return;
  }
  
  ec2x.getOperator();
  
  // if(!ec2x.getIPAddr());
  // {
  //   Log_error("GetIPAddr error...");
  //   return;    
  // }

  Log_info("APN: " + String(ec2x._apn));
  Log_info("Local IP: " + String(ec2x._str_ip));
  Log_info("Operator: " + String(ec2x._operator));

  Log_info("Start AT commands loop");
}

void loop()
{
  AT_bypass();
}


