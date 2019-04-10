/*
 * wiwio_trackerolte.h
 * A library for SeeedStudio Wio Tracker LTE
 *  
 * Copyright (c) 2017 Seeed Technology Co., Ltd.
 * Website    : www.seeed.cc
 * Author     : lawliet zou, lambor
 * Create Time: April 2017
 * Change Log :
 *
 * The MIT License (MIT)
  *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

// #include "UART_Interface.h"
#include "board_config.h"
#include "AtSerial.h"

/** WioTracker class.
 *  used to realize WioTracker communication
 */ 
 
class WioLTE
{
    public: 
        #if (1 == WIO_TRACKER_LTE_V11)
        const int DTR_PIN            =  1; // PA1
        const int BAT_C_PIN          = 16; // PB0
        const int RGB_LED_PIN        = 17; // PB1
        const int MODULE_PWR_PIN     = 18; // PB2
        const int ENABLE_VCCB_PIN    = 26; // PB10    
        const int ANT_PWR_PIN        = 28; // PB12
        const int STATUS_PIN         = 31; // PB15
        const int WAKEUP_IN_PIN      = 32; // PC0
        const int AP_READY_PIN       = 33; // PC1
        const int WAKEUP_DISABLE_PIN = 34; // PC2
        const int RESET_MODULE_PIN   = 35; // PC3
        const int PWR_KEY_PIN        = 36; // PC4 
        #elif (1 == WIO_TRACKER_LTE_V12)
        const int DTR_PIN            =  1; // PA1
        const int RGB_LED_PWR_PIN    =  8; // PA8
        const int SD_PWR_PIN         = 15; // PA15
        const int BAT_C_PIN          = 16; // PB0
        const int RGB_LED_PIN        = 17; // PB1
        const int MODULE_PWR_PIN     = 21; // PB5
        const int ENABLE_VCCB_PIN    = 26; // PB10    
        const int ANT_PWR_PIN        = 28; // PB12
        const int STATUS_PIN         = 31; // PB15
        const int WAKEUP_IN_PIN      = 32; // PC0
        const int AP_READY_PIN       = 33; // PC1
        const int WAKEUP_DISABLE_PIN = 34; // PC2
        const int RESET_MODULE_PIN   = 35; // PC3
        const int PWR_KEY_PIN        = 36; // PC4
        const int CODEC_I2C_PWR_PIN  = 37; // PC5

        AtSerial _AtSerial;

        WioLTE(); 
        void powerSupplyLTE(uint8_t on);
        void powerSupplyGNSS(uint8_t on);
        void powerSupplyLED(uint8_t on);
        void powerSupplyGrove(uint8_t on);
        void powerSupplySDCard(uint8_t on);
        void powerSupplyCodec(uint8_t on);
        void powerSupplyAntenna(uint8_t on);
              
        void initialize(void);  
        void turnOn(void);
        bool initialAtCommands(void); 
        bool isAlive(uint32_t timeout = 3000);   
        bool setURCtoUart1(void);
        bool checkSIMStatus(void);  
        bool waitForNetworkRegister(void);    
        bool sendSMS(char* number, char* data);    
        bool readAllRecUnreadSMS(void);    
        int16_t detectRecUnreadSMS(void);    
        bool readSMS(int messageIndex, char *message, int length, char *phone, char *datetime);     
        bool readSMS(int messageIndex, char *message, int length);    
        bool deleteSMS(int index);    
        bool callUp(char* number);       
        void answer(void);        
        bool hangup(void);  
        bool getSignalStrength(int *buffer);
        int recv(char* buf, int len);    
        bool set_CFUN(int mode);    
        bool set_SysClock(int mode);    
        bool AT_PowerDown(void);    
        #endif
};
