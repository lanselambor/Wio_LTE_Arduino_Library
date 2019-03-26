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

#ifndef __WIO_TRACKER_H__
#define __WIO_TRACKER_H__

#include "UART_Interface.h"
#include "itoa.h"
#include "board_config.h"

/** WioTracker class.
 *  used to realize WioTracker communication
 */ 
 
class WioTracker
{
public:
    /** Create WioTracker instance
     *  @param number default phone number during mobile communication
     */
     
    WioTracker(); 
    
    /** get instance of WioTracker class
     */
    // static WioTracker* getInstance() {
    //     return inst;
    // };
    
    /** initialize WioTracker module including SIM card check & signal strength
     *  @return true if connected, false otherwise
     */

    bool initialize(void);

   
    /** check if WioTracker module is powered on or not
     *  @returns
     *      true on success
     *      false on error
     */
    bool Check_If_Power_On(void);

    
    /** power Up WioTracker module
     *  @param  pin D13 is connectted to a power switch IC, the power is on when pin D13 pull up. 
     *  @returns
     *      
     */
    void Power_On(void);    
    void VCCB_Power_On(void);
    
    /** power reset for SIM800 board 
     *  @param  pin (preconfigurated as OUTPUT)
     *  @returns
     *      
     */ 
    void powerReset(void);
     
    /** Set URC port to uart1
     *  @returns
     *      true on successfully
     *      false on failed
     */
    bool setURCtoUart1(void);

    /** check SIM card status
    *  @returns
    *      true on SIM card Ready
    *      false on error
    */
    bool checkSIMStatus(void);

    /** Wait for network register
    *  @returns
    *      true on success
    *      false on error
    */
    bool waitForNetworkRegister(void);


    /** send text SMS
     *  @param  *number phone number which SMS will be send to
     *  @param  *data   message that will be send to
     *  @returns
     *      true on success
     *      false on error
     */
    bool sendSMS(char* number, char* data);
    
    /** Set all REC UNREAD SMS to REC READ
     *  @returns
     *      true on success
     *      false on error
     */
    bool readAllRecUnreadSMS(void);

    /** Detect REC UNREAD SMS 
     *  @returns
     *      -1 on no REC UNREAD SMS detected
     *      index of REC UNREAD SMS detected
     */
    int16_t detectRecUnreadSMS(void);

    /** read SMS, phone and date if getting a SMS message. It changes SMS status to READ 
     *  @param  messageIndex  SIM position to read
     *  @param  message  buffer used to get SMS message
     *  @param  length  length of message buffer
     *  @param  phone  buffer used to get SMS's sender phone number
     *  @param  datetime  buffer used to get SMS's send datetime
     *  @returns
     *      true on success
     *      false on error
     */
    bool readSMS(int messageIndex, char *message, int length, char *phone, char *datetime); 

    /** read SMS if getting a SMS message
     *  @param  buffer  buffer that get from WioTracker module(when getting a SMS, WioTracker module will return a buffer array)
     *  @param  message buffer used to get SMS message
     *  @param  check   whether to check phone number(we may only want to read SMS from specified phone number)
     *  @returns
     *      true on success
     *      false on error
     */
    bool readSMS(int messageIndex, char *message, int length);

    /** delete SMS message on SIM card
     *  @param  index   the index number which SMS message will be delete
     *          index > 998, delete all message stored on SIM card
     *  @returns
     *      true on success
     *      false on error
     */
    bool deleteSMS(int index);

    /** call someone
     *  @param  number  the phone number which you want to call
     *  @returns
     *      true on success
     *      false on error
     */
    bool callUp(char* number);

    /** auto answer if coming a call
     *  @returns
     */    
    void answer(void);
    
    /** hang up if coming a call
     *  @returns
     *      true on success
     *      false on error
     */    
    bool hangup(void);  
    
    /** get Signal Strength from SIM900 (see AT command: AT+CSQ) as integer
    *  @param
    *  @returns
    *      true on success
    *      false on error
    */
    bool getSignalStrength(int *buffer);

    int recv(char* buf, int len);

    /** Set phone functionarity mode
     * @param
     *      0, least consumption 1, 4
     *      1, standard mode
     *      4, shut down RF send and receive function
     */
    bool set_CFUN(int mode);

    /** Set module system clock 
     * @param
     *  0 Disable slow clock
     *  1 Enable slow clock, and it is controlled by DTR
     */
    bool set_SysClock(int mode);

    /** Set module into sleep mode
     * @param
     * @return 
     *  true on sleep successfully
     *  false on sleep failed
     */
    bool module_sleep(void);

    /** Set module wakeup from sleep mode
     * @param
     * @return 
     *  true on wakeup successfully
     *  false on wakeup failed
     */
    bool module_wakeup(void);

    /**
     * Turn off module power buy AT commnad
     */
    bool AT_PowerDown(void);    
    
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
#elif(1 == MKRZERO_4G)   
    const int DTR_PIN            =  3; // PA11
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
#endif
};
#endif
