#include "wiolte_cat_1.h"
#include "quectel_ec2x.h"

WioLTE board;
Quectel_EC2x ec2x;

const char apn[10] = "CMNET";
// const char apn[10] = "UNINET";
const char URL[100] = "45.77.14.150";
char http_cmd[100] = "GET /media/uploads/mbed_official/hello.txt HTTP/1.0\n\r\n\r";
int port = 5000;

int ret = 0;


void setup() {  
  _debugSerial.println("-- Begin --------------------");
  
  board.initialize();
  delay(1000);
  board.turnOn();

  while(false == ec2x.isAlive()){
    _debugSerial.println("### Waitting for module to alvie");
    delay(1000);
  }

  ec2x.Init();
  while(!ec2x.waitForNetworkRegister())
  {
    _debugSerial.println("### Failed to register network ###");
    return;
  }
  _debugSerial.println("### Initialize done");

  if(!ec2x.Activate(apn, "", ""))
  {
    _debugSerial.println("### Failed to active opeator");
    return;
  }

  if(!ec2x.getIPAddr())
  {    
    return;
  }
  _debugSerial.print("### IP: ");
  _debugSerial.println(ec2x._str_ip);

  if(!ec2x.getOperator())
  {
    return;
  }
  _debugSerial.print("### Operator: ");
  _debugSerial.println(ec2x._operator);

  // if(ec2x.connect(URL, port, TCP)) 
  // {
  //   ec2x.write(http_cmd); 
  //   while(_moduleSerial.available()){
  //       _debugSerial.write(_moduleSerial.read());
  //   }    
  //   ec2x.close(1);
  // } 
  // else {
  //   _debugSerial.println("Connect Error!");
  // }
  // int signal = 0;;
  // wiolte.getSignalStrength(&signal);
  // _debugSerial.print("Signale: ");
  // _debugSerial.println(signal);
  
}

void loop() {
  /* Debug */
  wiolte._AtSerial.AT_ByPass();
}
