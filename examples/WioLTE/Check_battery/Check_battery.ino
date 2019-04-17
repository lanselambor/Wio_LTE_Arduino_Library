
#include "UART_Interface.h"

const int battery_adc_Pin = 16;  // PB0

void setup() {

}

void loop() {
    float a = analogRead(battery_adc_Pin);
    float v = a*3300/2048;        
    Serial.print("The voltage of battery is ");
    Serial.print(v, 2);
    Serial.println(" mV");
    delay(1000);
}