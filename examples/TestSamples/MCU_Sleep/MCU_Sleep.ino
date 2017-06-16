#include "Arduino.h"
#include "module_common.h"
#include "Arduino_Interface.h"
#include "pwr.h"

#define SET_BIT(REG, BIT)  ((REG) |= (BIT))
#define SCB_SCR_SLEEPDEEP_Pos               2                                             /*!< SCB SCR: SLEEPDEEP Position */
#define SCB_SCR_SLEEPDEEP_Msk              (1UL << SCB_SCR_SLEEPDEEP_Pos)                 /*!< SCB SCR: SLEEPDEEP Mask */



// Function declare
void mcu_standby();


WioTracker wio = WioTracker();


void setup() {
  SerialUSB.begin(115200);
  // delay(5000);
  SerialUSB.println("Begin...");

  // wio.Power_On();
  // while(false == wio.Check_If_Power_On()){
  //   SerialUSB.println("Waitting for module to alvie...");
  //   delay(1000);
  // }  
  // SerialUSB.println("Power On O.K!");
  pwr_init();
  
}

void loop() {
  // /* Debug */
  // if(SerialUSB.available()){
  //   MODULE_PORT.write(SerialUSB.read());
  // }
  // if(MODULE_PORT.available()){     
  //   SerialUSB.write(MODULE_PORT.read()); 
  // }
  SerialUSB.println("MCU Standby Test!");
  delay(1000);
}



















