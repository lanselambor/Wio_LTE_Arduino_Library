/*
 * WioLTE.cpp
 * A library for SeeedStudio Wio LTE Tracker
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

 #include <stdio.h>
 #include <itoa.h>

 #include "WioLTE.h"
 #include "Stopwatch.h"

// WioLTE* WioLTE::inst;

/** 
 * Create WioLTE instance
 *  @param number default phone number during mobile communication
 */
WioLTE::WioLTE() : _AtSerial(&Serial1)
{     
    // for(uint8_t i = 0; i < 64; i++)
    // {
    //     pinMode(i, INPUT);
    // }
    
    powerSupplyLTE(0);
    powerSupplyGNSS(0);
    powerSupplyLED(0);
    powerSupplyGrove(0);
    powerSupplySDCard(0);
    powerSupplyCodec(0);
    powerSupplyAntenna(1);
}   

void WioLTE::powerSupplyLTE(uint8_t on)
{
    pinMode(MODULE_PWR_PIN, OUTPUT);
    digitalWrite(MODULE_PWR_PIN, on > 0 ? HIGH : LOW);
}

void WioLTE::powerSupplyGNSS(uint8_t on)
{
    pinMode(ANT_PWR_PIN, OUTPUT);
    digitalWrite(ANT_PWR_PIN, on > 0 ? HIGH : LOW);
}

void WioLTE::powerSupplyLED(uint8_t on)
{
    pinMode(RGB_LED_PIN, OUTPUT);
    digitalWrite(RGB_LED_PIN, on > 0 ? HIGH : LOW);
}

void WioLTE::powerSupplyGrove(uint8_t on)
{
    pinMode(ENABLE_VCCB_PIN, OUTPUT);
    digitalWrite(ENABLE_VCCB_PIN, on > 0 ? HIGH : LOW);
}

void WioLTE::powerSupplySDCard(uint8_t on)
{
    pinMode(SD_PWR_PIN, OUTPUT);
    digitalWrite(SD_PWR_PIN, on > 0 ? HIGH : LOW);
}

void WioLTE::powerSupplyCodec(uint8_t on)
{
    pinMode(CODEC_I2C_PWR_PIN, OUTPUT);
    digitalWrite(CODEC_I2C_PWR_PIN, on > 0 ? HIGH : LOW);
}

void WioLTE::powerSupplyAntenna(uint8_t on)
{
    pinMode(ANT_PWR_PIN, OUTPUT);
    digitalWrite(ANT_PWR_PIN, on > 0 ? HIGH : LOW);
}

void WioLTE::initialize(void)
{    
    _AtSerial._Serial->begin(115200);           
}

void WioLTE::reset(void)
{
    turnOff();
    turnOn();
}

void WioLTE::turnOff(void)
{
    powerSupplyLTE(0);
    delay(1000);
}

void WioLTE::turnOn(void)
{    
    powerSupplyLTE(1);
    pinMode(WAKEUP_IN_PIN, OUTPUT);
    digitalWrite(WAKEUP_IN_PIN, LOW);
    delay(500);

    pinMode(PWR_KEY_PIN, OUTPUT);
    digitalWrite(PWR_KEY_PIN, LOW);
    digitalWrite(PWR_KEY_PIN, HIGH);
    delay(800);
    digitalWrite(PWR_KEY_PIN, LOW);
}


/** Set URC port to uart1
 *  @returns
 *      true on successfully
 *      false on failed
 */
bool WioLTE::setURCtoUart1(void)
{
    return _AtSerial.WriteCommandAndWaitForResponse("AT+QURCCFG=\"urcport\",\"uart1\"", "OK", CMD, 2);
}



/** send text SMS
 *  @param  *number phone number which SMS will be send to
 *  @param  *data   message that will be send to
 *  @returns
 *      true on success
 *      false on error
 */
bool WioLTE::sendSMS(char *number, char *data)
{
    //char cmd[32];
    if(!_AtSerial.WriteCommandAndWaitForResponse(F("AT+CMGF=1\r\n"), "OK\r\n", CMD)) { // Set message mode to ASCII
        return false;
    }
    delay(500);
    _AtSerial.FlushSerial();
    _AtSerial.WriteCommand("AT+CMGS=\"");
    _AtSerial.WriteCommand(number);
    if(!_AtSerial.WriteCommandAndWaitForResponse(F("\"\r\n"),">",CMD)) {
        return false;
    }
    delay(1000);
    _AtSerial.WriteCommand(data);
    delay(500);
    _AtSerial.WriteEndMark();
    return _AtSerial.WaitForResponse("OK\r\n", CMD, 10);
}

/** Set all REC UNREAD SMS to REC READ
 *  @returns
 *      true on success
 *      false on error
 */
bool WioLTE::readAllRecUnreadSMS(void)
{
    /*
        AT+CMGL="REC UNREAD"
    */
    
    // Set SMS as text mode
    if(!_AtSerial.WriteCommandAndWaitForResponse(F("AT+CMGF=1\r\n"), "OK", CMD)) { // Set message mode to ASCII
        return false;
    } else {
#if(UART_DEBUG==true)
        DEBUG("Set SMS as text mode!");
#endif
    }

    _AtSerial.WriteCommand("AT+CMGL=\"REC UNREAD");
    if(!_AtSerial.WriteCommandAndWaitForResponse(F("\"\r\n"),"OK",CMD, true)) {
        _AtSerial.FlushSerial();
    } else {
        return false;
    }

    return true;
}

/** Detect REC UNREAD SMS 
 *  @returns
 *      -1 on no REC UNREAD SMS detected
 *      index of REC UNREAD SMS detected
 */
int16_t WioLTE::detectRecUnreadSMS(void)
{
    /*
        AT+CMGL="REC UNREAD"

        +CMGL: 0,"REC UNREAD","xxxxxxx",,"17/07/24,14:45:43+32",161,6
        xxxxxxx
    */

    uint16_t i;
    char *s, *ps, *pe;
    char Buffer[64];
    char str_index[8];

    _AtSerial.CleanBuffer(Buffer, 64);
    _AtSerial.WriteCommand("AT+CMGL=\"REC UNREAD\"\r\n");
    _AtSerial.Read(Buffer, 64, 1000);
    
    DEBUG("SMS Buffer: ");
    DEBUG(Buffer);

    if(NULL != (s = strstr(Buffer, "+CMGL:"))){
        ps = s + 7;
        pe = strstr(s, "\"");
        pe -= 2;

        for(i = 0; ps+i <= pe; i++){
            str_index[i] = *(ps+i);
        }
        str_index[i] = '\0';
    } else {
        return -1;
    }

    // Check if str_index valid
    for(i = 0; i < strlen(str_index); i++)
    {
        if(!('0' <= str_index[i] && str_index[i] <= '9')){
#if(UART_DEBUG)
            ERROR("SMS index invalid\r\n");
#endif
            return -1;
        }
    }

    // TODO not use atoi method here
    return atoi(str_index);
}

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
bool WioLTE::readSMS(int messageIndex, char *message, int length, char *phone, char *datetime)  
{
  /* Response is like:
  AT+CMGR=2
  
  +CMGR: "REC READ","XXXXXXXXXXX","","14/10/09,17:30:17+08"
  SMS text here
  
  So we need (more or lees), 80 chars plus expected message length in buffer. CAUTION FREE MEMORY
  */

    int i = 0;
    char Buffer[80 + length];
    //char cmd[16];
    char num[4];
    char *p,*p2,*s;
    
    _AtSerial.WriteCommandAndWaitForResponse(F("AT+CMGF=1\r\n"),"OK\r\n",CMD);
    delay(1000);
    //sprintf(cmd,"AT+CMGR=%d\r\n",messageIndex);
    //WriteCommand(cmd);
    _AtSerial.WriteCommand("AT+CMGR=");
    itoa(messageIndex, num, 10);
    _AtSerial.WriteCommand(num);
    _AtSerial.WriteCommand("\r\n");
    _AtSerial.CleanBuffer(Buffer,sizeof(Buffer));
    _AtSerial.Read(Buffer,sizeof(Buffer));
      
    if(NULL != ( s = strstr(Buffer,"READ\",\""))){
        // Extract phone number string
        p = strstr(s,",");
        p2 = p + 2; //We are in the first phone number character
        p = strstr((char *)(p2), "\"");
        if (NULL != p) {
            i = 0;
            while (p2 < p) {
                phone[i++] = *(p2++);
            }
            phone[i] = '\0';            
        }
        // Extract date time string
        p = strstr((char *)(p2),",");
        p2 = p + 1; 
        p = strstr((char *)(p2), ","); 
        p2 = p + 2; //We are in the first date time character
        p = strstr((char *)(p2), "\"");
        if (NULL != p) {
            i = 0;
            while (p2 < p) {
                datetime[i++] = *(p2++);
            }
            datetime[i] = '\0';
        }        
        if(NULL != ( s = strstr(s,"\r\n"))){
            i = 0;
            p = s + 2;
            while((*p != '\r')&&(i < length-1)) {
                message[i++] = *(p++);
            }
            message[i] = '\0';
        }
        return true;
    }
    return false;    
}

/** read SMS if getting a SMS message
 *  @param  buffer  buffer that get from WioLTE module(when getting a SMS, WioLTE module will return a buffer array)
 *  @param  message buffer used to get SMS message
 *  @param  check   whether to check phone number(we may only want to read SMS from specified phone number)
 *  @returns
 *      true on success
 *      false on error
 */
bool WioLTE::readSMS(int messageIndex, char *message,int length)
{
    int i = 0;
    char Buffer[100];
    //char cmd[16];
    char num[4];
    char *p,*s;
    
    _AtSerial.WriteCommandAndWaitForResponse(F("AT+CMGF=1\r\n"),"OK\r\n",CMD);
    delay(1000);
    _AtSerial.WriteCommand("AT+CMGR=");
    itoa(messageIndex, num, 10);
    _AtSerial.WriteCommand(num);
    // WriteCommand("\r\n");
    _AtSerial.WriteCommand("\r\n");
//  sprintf(cmd,"AT+CMGR=%d\r\n",messageIndex);
//    WriteCommand(cmd);
    _AtSerial.CleanBuffer(Buffer,sizeof(Buffer));
    _AtSerial.Read(Buffer,sizeof(Buffer),DEFAULT_TIMEOUT);
    if(NULL != ( s = strstr(Buffer,"+CMGR:"))){
        if(NULL != ( s = strstr(s,"\r\n"))){
            p = s + 2;
            while((*p != '\r')&&(i < length-1)) {
                message[i++] = *(p++);
            }
            message[i] = '\0';
            return true;
        }
    }
    return false;   
}

/** delete SMS message on SIM card
 *  @param  index   the index number which SMS message will be delete
 *          index > 998, delete all message stored on SIM card
 *  @returns
 *      true on success
 *      false on error
 */
bool WioLTE::deleteSMS(int index)
{
    //char cmd[16];
    char num[4];
    //sprintf(cmd,"AT+CMGD=%d\r\n",index);
    _AtSerial.WriteCommand("AT+CMGD=");
    if(index > 998){
        _AtSerial.WriteCommand("1,4");
    }
    else{
        itoa(index, num, 10);
        _AtSerial.WriteCommand(num);
    }
    // We have to wait OK response
    //return WriteCommandAndWaitForResponse(cmd,"OK\r\n",CMD);
    return _AtSerial.WriteCommandAndWaitForResponse(F("\r"),"OK\r\n",CMD); 
}

/** call someone
 *  @param  number  the phone number which you want to call
 *  @returns
 *      true on success
 *      false on error
 */
bool WioLTE::callUp(char *number)
{
    //char cmd[24];
    if(!_AtSerial.WriteCommandAndWaitForResponse(F("AT+COLP=1\r\n"),"OK\r\n",CMD)) {
        return false;
    }
    delay(1000);
    //HACERR quitar SPRINTF para ahorar memoria ???
    //sprintf(cmd,"ATD%s;\r\n", number);
    //WriteCommand(cmd);
    _AtSerial.WriteCommand("ATD");
    _AtSerial.WriteCommand(number);
    _AtSerial.WriteCommand(";\r\n");
    return true;
}

/** auto answer if coming a call
 *  @returns
 */ 
void WioLTE::answer(void)
{
    _AtSerial.WriteCommand("ATA\r\n");  //TO CHECK: ATA doesnt return "OK" ????
}

/** hang up if coming a call
 *  @returns
 *      true on success
 *      false on error
 */
bool WioLTE::hangup(void)
{
    return _AtSerial.WriteCommandAndWaitForResponse(F("ATH\r\n"),"OK\r\n",CMD);
}

/** get Signal Strength from SIM900 (see AT command: AT+CSQ) as integer
*  @param
*  @returns
*      true on success
*      false on error
*/
bool WioLTE::getSignalStrength(int *buffer)
{
    //AT+CSQ                        --> 6 + CR = 10
    //+CSQ: <rssi>,<ber>            --> CRLF + 5 + CRLF = 9                     
    //OK                            --> CRLF + 2 + CRLF =  6

    byte i = 0;
    char Buffer[26];
    char *p, *s;
    char buffers[4];
    _AtSerial.FlushSerial();
    _AtSerial.WriteCommand("AT+CSQ\r");
    _AtSerial.CleanBuffer(Buffer, 26);
    _AtSerial.Read(Buffer, 26, DEFAULT_TIMEOUT);
    if (NULL != (s = strstr(Buffer, "+CSQ:"))) {
        s = strstr((char *)(s), " ");
        s = s + 1;  //We are in the first phone number character 
        p = strstr((char *)(s), ","); //p is last character """
        if (NULL != s) {
            i = 0;
            while (s < p) {
                buffers[i++] = *(s++);
            }
            buffers[i] = '\0';
        }
        *buffer = atoi(buffers);
        return true;
    }
    return false;
}

int WioLTE::recv(char* buf, int len)
{
    _AtSerial.CleanBuffer(buf,len);
    _AtSerial.Read(buf,len);   //Ya he llamado a la funcion con la longitud del buffer - 1 y luego le estoy añadiendo el 0
    return strlen(buf);
}


/*************************** Module consumption control **************************/

/** Set phone functionarity mode
 * @param
 *      0, least consumption 1, 4
 *      1, standard mode
 *      4, shut down RF send and receive function
 */
bool WioLTE::set_CFUN(int mode)
{
  char buf_w[20];
  _AtSerial.CleanBuffer(buf_w, 20);
  sprintf(buf_w, "AT+CFUN=%d", mode);
  _AtSerial.WriteCommand(buf_w);
  return _AtSerial.WriteCommandAndWaitForResponse("\n\r", "OK", CMD, 2000, UART_DEBUG);
}

/** Set module system clock 
 * @param
 *  0 Disable slow clock
 *  1 Enable slow clock, and it is controlled by DTR
 */
bool WioLTE::set_SysClock(int mode)
{
  char buf_w[20];
  _AtSerial.CleanBuffer(buf_w, 20);
  sprintf(buf_w, "AT+QSCLK=%d", mode);
  _AtSerial.WriteCommand(buf_w);
  return _AtSerial.WriteCommandAndWaitForResponse("\r\n", "OK", CMD, 2000);
}

/**
 * Turn off module power buy AT commnad
 */
bool WioLTE::AT_PowerDown(void)
{
  return _AtSerial.WriteCommandAndWaitForResponse("AT+QPOWD=1\n\r", "NORMAL POWER DOWN", CMD, 2000, UART_DEBUG);
}

/*************************** End of Module consumption control **************************/
