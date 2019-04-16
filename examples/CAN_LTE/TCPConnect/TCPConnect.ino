#include "quectel_ec2x.h"

Quectel_EC2x ec2x;

// const char apn[10] = "CMNET";
const char apn[10] = "UNINET";
const char URL[100] = "example.com";
char http_cmd[100] = "GET / HTTP/1.0\n\r\n\r";
int port = 80;

int ret = 0;

void setup() {  
  Serial.begin(115200);
  while(!Serial);
  Serial.println("-- Wio LTE --------------------");
  
  ec2x.Init();

  while(false == ec2x.isAlive()){
    Serial.println("### Waitting for module to alvie");
    delay(1000);
  }

  while(!ec2x.waitForNetworkRegister())
  {
    Serial.println("### Failed to register network ###");
    return;
  }
  Serial.println("### Initialize done");

  if(!ec2x.Deactivate())
  {
    Serial.println("### Failed to deactive network");
    return;
  }

  if(!ec2x.Activate(apn, "", ""))
  {
    Serial.println("### Failed to active opeator");
    return;
  }

  if(!ec2x.getIPAddr())
  {    
    Serial.println("### Failed to get ip.");
    return;
  }
  Serial.print("### IP: ");
  Serial.println(ec2x._str_ip);

  if(!ec2x.getOperator())
  {
    Serial.println("### Failed to get operator.");
    return;
  }
  Serial.print("### Operator: ");
  Serial.println(ec2x._operator);

  if(ec2x.sockOpen(URL, port, TCP)) 
  {        
    Stopwatch ws;
    ws.Restart();

    ec2x.sockWrite(0, http_cmd);    
    ec2x.sockReceive(0, NULL, 1500);     
    while(ws.ElapsedMilliseconds() < 10000)
    {
      ec2x._AtSerial.AT_ByPass();
    }
    
    if(!ec2x.sockClose(0))
    {
      Serial.println("Socket close Error!");
      return;
    }
    Serial.println("Socket closed");
    
  } 
  else {
    Serial.println("Connect Error!");
  }
  // int signal = 0;;
  // wiolte.getSignalStrength(&signal);
  // Serial.print("Signale: ");
  // Serial.println(signal);
  
}

void loop() {
  /* Debug */
  ec2x._AtSerial.AT_ByPass();
}
