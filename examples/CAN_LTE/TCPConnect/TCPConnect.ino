#include "quectel_ec2x.h"

Quectel_EC2x ec2x;

// const char apn[10] = "CMNET";
const char apn[10] = "UNINET";
const char URL[100] = "45.77.14.150";
char http_cmd[100] = "GET /media/uploads/mbed_official/hello.txt HTTP/1.0\n\r\n\r";
int port = 5000;

int ret = 0;

void setup() {  
  Serial.begin(115200);
  while(!Serial);
  Serial.println("-- Begin --------------------");
  
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

  if(!ec2x.Activate(apn, "", ""))
  {
    Serial.println("### Failed to active opeator");
    return;
  }

  // return;


  if(!ec2x.getIPAddr())
  {    
    return;
  }
  // Serial.print("### IP: ");
  // Serial.println(ec2x._str_ip);

  delay(1000);

  if(!ec2x.getOperator())
  {
    return;
  }
  Serial.print("### Operator: ");
  Serial.println(ec2x._operator);

  // if(ec2x.connect(URL, port, TCP)) 
  // {
  //   ec2x.write(http_cmd); 
  //   while(_moduleSerial.available()){
  //       Serial.write(_moduleSerial.read());
  //   }    
  //   ec2x.close(1);
  // } 
  // else {
  //   Serial.println("Connect Error!");
  // }
  // int signal = 0;;
  // wiolte.getSignalStrength(&signal);
  // Serial.print("Signale: ");
  // Serial.println(signal);
  
}

void loop() {
  /* Debug */
  ec2x._AtSerial.AT_ByPass();
}
