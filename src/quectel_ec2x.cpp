#include <stdio.h>

#include "quectel_ec2x.h"

Quectel_EC2x::Quectel_EC2x() : _AtSerial(&Serial1)
{

}

void Quectel_EC2x::Init()
{
  _AtSerial._Serial->begin(115200);
}

/** check if module is powered on or not
 *  @returns
 *      true on success
 *      false on error
 */
boolean Quectel_EC2x::isAlive(uint32_t timeout)
{
    Stopwatch sw;
    sw.Restart();

    while(sw.ElapsedMilliseconds() <= timeout)
    {
        if(_AtSerial.WriteCommandAndWaitForResponse(F("AT\r\n"), "OK", CMD, 500)){
            return RET_OK;
        }
    }    
    
    return false;
}

boolean Quectel_EC2x::waitForNetworkRegister(uint32_t timeout)
{
    Stopwatch sw;
    sw.Restart();

    // Check CS Registration
    while(true)
    {
        if(_AtSerial.WriteCommandAndWaitForResponse("AT+CEREG?\r\n", "+CEREG: 0,1", CMD, 500) || // Home network
        _AtSerial.WriteCommandAndWaitForResponse("AT+CEREG?\r\n", "+CEREG: 0,5", CMD, 500)) // Roaming
        {
            break;
        }
        delay(100);
        if (sw.ElapsedMilliseconds() >= (unsigned long)timeout) return RET_ERR;
    }

    sw.Restart();
    // Check PS Registration
    while(true)
    {
        if(_AtSerial.WriteCommandAndWaitForResponse("AT+CGREG?\r\n", "+CGREG: 0,1", CMD, 500) || // Home network
        _AtSerial.WriteCommandAndWaitForResponse("AT+CGREG?\r\n", "+CGREG: 0,5", CMD, 500)) // Roaming
        {
            break;
        }
        delay(100);
        if (sw.ElapsedMilliseconds() >= (unsigned long)timeout) return RET_ERR;
    }
  
  return RET_OK;
}

boolean Quectel_EC2x::Activate(const char* APN, const char* userName, const char* password, long waitForRegistTimeout)
{
    char sendBuffer[64] = {0};

    sprintf(sendBuffer, "AT+QICSGP=1,1,\"%s\",\"%s\",\"%s\",1\r\n", APN, userName, password);
    if(!_AtSerial.WriteCommandAndWaitForResponse(sendBuffer, "OK", CMD, 5000, UART_DEBUG))
    {
        return RET_ERR;
    }

    if(!waitForNetworkRegister(waitForRegistTimeout)) return RET_ERR;
    
#if(1==UART_DEBUG)
    _AtSerial.WriteCommandAndWaitForResponse("AT+CREG?\r\n", "OK", CMD, 500, UART_DEBUG);
    _AtSerial.WriteCommandAndWaitForResponse("AT+CGREG?\r\n", "OK", CMD, 500, UART_DEBUG);
    _AtSerial.WriteCommandAndWaitForResponse("AT+CEREG?\r\n", "OK", CMD, 500, UART_DEBUG);
#endif // UART_DEBUG
	
  _AtSerial.WriteCommand("AT+QIACT=1\r\n");		
  delay(100);

	// for debug.
#if(1==UART_DEBUG)
	if (!_AtSerial.WriteCommandAndWaitForResponse("AT+QIACT?\r\n", "OK", CMD, 500, UART_DEBUG)) return RET_ERR;
#endif // UART_DEBUG

	return RET_OK;
}

boolean Quectel_EC2x::Deactivate()
{
    if (!_AtSerial.WriteCommandAndWaitForResponse("AT+QIDEACT=1\r\n", "OK", CMD, 500, UART_DEBUG)) return RET_ERR;

	return RET_OK;
}

/**
 * Get IP address
 */
boolean Quectel_EC2x::getIPAddr(void)
{
    // AT+QIACT?
    // +QIACT: 1,1,1,"10.72.134.66"

    char *p;
    uint8_t i = 0;
    char rxBuf[128] = {'\0'};
    uint8_t a0, a1, a2, a3;

    // Get IP address
    _AtSerial.WriteCommand("AT+QIACT?\r\n");
    _AtSerial.Read(rxBuf, sizeof(rxBuf));
    rxBuf[127] = '\0';
    DEBUG(rxBuf);

    // sscanf can not use here, why?
    // if(4 == sscanf(p, "+QIACT: %*d,%*d,%*d,\"%d.%d.%d.%d\"", &a0, &a1, &a2, &a3))  
    if(NULL != (p = strstr(rxBuf, "+QIACT:")))
    {       
        p = strtok(rxBuf, ",");  // +QIACT: 1,1,1,"10.72.134.66"
        p = strtok(NULL, ",");  // 1,1,"10.72.134.66"
        p = strtok(NULL, ",");  // 1,"10.72.134.66"
        p = strtok(NULL, ",");  // "10.72.134.66"
        p += 1;
    }
    else
    {
        Log_error("+QIACT failed");
        return RET_ERR;
    }

    memset(_str_ip, '\0',sizeof(_str_ip));
    while((*(p+i) != '\"') && (*(p+i) != '\0'))
    { 
      _str_ip[i] = *(p+i);
      i++;
    }
    _u32ip = str_to_u32ip(_str_ip);

    return RET_OK;
}

/**
 * Get operator name
*/
boolean Quectel_EC2x::getOperator()
{
    // AT+COPS?
    // +COPS: 0,0,"CHN-UNICOM",7

    char *p;
    char rxBuf[64] = {'\0'};

    _AtSerial.WriteCommand("AT+COPS?\r\n");
    _AtSerial.Read(rxBuf, sizeof(rxBuf));
    DEBUG(rxBuf);  

    if(NULL != (p = strstr(rxBuf, "+COPS:")))
    {
        if(1 == sscanf(p, "+COPS: %*d,%*d,\"%[^\"]\",%*d", _operator))
        {
            // debugPrint("Operator: ");
            // debugPrintln(_operator);
        }
    }
    else
    {
        Log_error("+COPS failed");
        return RET_ERR;
    }

    return RET_OK;
}

/** 
 * Parse IP string
 * @return ip in hex
 */
uint32_t Quectel_EC2x::str_to_u32ip(char* str)
{
    uint32_t ip = 0;
    char *p = (char*)str;
    
    for(int i = 0; i < 4; i++) {
        ip |= atoi(p);
        p = strchr(p, '.');
        if (p == NULL) {
            break;
        }
        if(i < 3) ip <<= 8;
        p++;
    }
    return ip;
}


/**
 * Set host server by ip/domain and port, then connect to the server 
 * @param sockid is local socket id
 * @param host is remote server ip or domain
 * @param port is remote server port
 * @return true for success, false for failure
*/
boolean Quectel_EC2x::sockOpen(const char *host, int port, Socket_type connectType )
{
  /**
   * @breif EC2x has 12 socketid can be used, in this function we always use socketid NO.0.
   * @dataAccessMode:
   * 
   * 0.In SOCKET_BUFFER_MODE
   *  - Use "AT+QISEND=0,len" to send data.
   *  - Use "AT+QIRD=0,0" to check buffer size and read out data.
   * 
   * 1.In SOCKET_DIRECT_PUSH_MODE 
   *  - Use "AT+QISEND=0,len" to send data
   *  - Received data will push out directly.
   * 
   * 2.In SOCKET_TRANSPARANT_MODE
   *  - After connect 
   *  - Write data to Module Serial directly.
   *  - Any received data will push out directly.
  */

  if(host == NULL || host[0] == '\0') return false;
  if(port < 0 || 65535 < port) return false;

  const char* typeStr;
  switch(connectType)
  {
    case TCP:
      typeStr = "TCP";
      break;
    case UDP:
      typeStr = "UDP";
      break;
    
    default: 
      return false;
  }

  uint8_t errCount = 0;
  char txBuf[128] = {0};
  char rxBuf[128] = {0};
  char *p;
  
  _AtSerial.WriteCommand("AT+QISTATE?\r\n");
  _AtSerial.Read((char*)rxBuf, sizeof(rxBuf), 2000);  

  //if socketId 0 has been used, then deactive PDP then re-open socket
  if(NULL != (p = strstr(rxBuf, "+QISTATE:"))) return false;  


  if(connectType == TCP) { sprintf(txBuf, "AT+QIOPEN=1,0,\"%s\",\"%s\",%d\r\n", typeStr, host, port); }
  else if(connectType == UDP) { sprintf(txBuf, "AT+QIOPEN=1,0,\"%s\",\"%s\",%d\r\n", typeStr, host, port); }
  else { return false; } 
  
  DEBUG(txBuf);
  while(!_AtSerial.WriteCommandAndWaitForResponse(txBuf, "+QIOPEN: 0", CMD, 10000)) {// connect tcp      
      ERROR("ERROR:QIOPEN");
      if(errCount > 3){
          return false;
      }
      errCount++;
      // Deactive
  }

  return true;
}

/**
 * Close connected socket
 * 
 * @param sockid socket id will be closed
 * @return true for success, false for failure.
*/

boolean Quectel_EC2x::sockClose(int sockid)
{
  // Only socketId 0 were used, so we need to close socketId 0
  if(!_AtSerial.WriteCommandAndWaitForResponse("AT+QICLOSE=0\r\n", "OK", CMD, 10000))
  {
    return false;
  }
  return true;
}

/**
 * Write data to opened socket
 * 
 * @param sockid socket id created before 
 * @param data data array will be sent to remote
 * @param dataSize
 * @return true for success, false for failure.
 * 
*/
boolean Quectel_EC2x::sockWrite(uint8_t sockid, char *data, uint16_t dataSize)
{
  if(sockid < 0 || sockid > 11) return false;
  if(data == NULL || data[0] == '\0' || dataSize <= 0) return false;

  char txBuf[32] = {'\0'};

  sprintf(txBuf, "AT+QISEND=0,%d\r\n", dataSize);
  if(!_AtSerial.WriteCommandAndWaitForResponse(txBuf, ">", CMD, 500, UART_DEBUG)) return false;
  _AtSerial.WriteCommand(data);
  if(!_AtSerial.WaitForResponse("SEND OK\r\n", CMD, 5000)) return false;
   
  return true;
}

boolean Quectel_EC2x::sockWrite(uint8_t sockid, char *data)
{
  sockWrite(sockid, data, strlen(data));
}


boolean Quectel_EC2x::sockReceive(uint8_t sockid, char *data, uint16_t dataSize)
{
  char txBuf[32] = {'\0'};

  sprintf(txBuf, "AT+QIRD=%d,%d\r\n", sockid, dataSize);
  _AtSerial.WriteCommand(txBuf);
  
}

/**
 * Send udp message once
 * 
 * @param sockid - socket id created before.
 * @param host - remote server ip address to send message to.
 * @param port - remote server port. 
 * @param oneByte - one byte to send.
 * 
 * @return true for success, false for failure.
*/
boolean Quectel_EC2x::udpSendTo(uint8_t sockid, char *host, uint16_t port, char oneByte)
{

}

/**
 * Send udp message once.
 * 
 * @param sockid - socket id created before.
 * @param host - remote server ip address to send message to.
 * @param port - remote server port. 
 * @param data - Content to send.
 * @param dataSize length of data to be sent
 * 
 * @return true for success, false for failure.
*/
boolean Quectel_EC2x::udpSendTo(uint8_t sockid, char *host, uint16_t port, char *data, uint16_t dataSize)
{

}



boolean Quectel_EC2x::close_GNSS()
{
  int errCounts = 0;

  while(!_AtSerial.WriteCommandAndWaitForResponse("AT+QGNSSC?\n\r", "+QGNSSC: 0", CMD, 2000, UART_DEBUG)){
      errCounts ++;
      if(errCounts > 100){
        return false;
      }
      _AtSerial.WriteCommandAndWaitForResponse("AT+QGNSSC=0\n\r", "OK", CMD, 2000, UART_DEBUG);
      delay(1000);
  }

  return true;
}

/**
 * Aquire GPS sentence
 */
boolean Quectel_EC2x::dataFlowMode(void)
{
    // Make sure that "#define UART_DEBUG" is uncomment.
    _AtSerial.WriteCommand("AT+QGPSLOC?\r\n");
    return _AtSerial.WaitForResponse("OK", CMD, 2000, true);
}

boolean Quectel_EC2x::open_GNSS(void)
{
  int errCounts = 0;

  while(!_AtSerial.WriteCommandAndWaitForResponse("AT+QGPS?\r\n", "+QGPS: 1", CMD, 2000, UART_DEBUG)){
      errCounts ++;
      if(errCounts > 5){
        return false;
      }
      _AtSerial.WriteCommandAndWaitForResponse("AT+QGPS=1\r\n", "OK", CMD, 2000, UART_DEBUG);
      delay(1000);
  }

  return true;
}

/** 
 * Get coordinate infomation
 */
boolean Quectel_EC2x::getCoordinate(void)
{
  int tmp = 0;
  char *p = NULL;
  uint8_t str_len = 0;
  char buffer[128];

  _AtSerial.CleanBuffer(buffer, 128);
  _AtSerial.WriteCommand("AT+QGPSLOC?\r\n");
  _AtSerial.Read(buffer, 128, 2);
  // Serial.println(buffer);
  if(NULL != (p = strstr(buffer, "+CME ERROR")))
  {
    return false;
  }

  // +QGPSLOC: 084757.700,2235.0272N,11357.9730E,1.6,40.0,3,171.43,0.0,0.0,290617,10    
  else if(NULL != (p = strstr(buffer, "+QGPSLOC:")))
  {
    p += 10;      
    p = strtok(buffer, ","); // time
    p = strtok(NULL, ",");  // latitude
    sprintf(str_latitude, "%s", p);
    latitude = strtod(p, NULL);
    tmp = (int)(latitude / 100);
    latitude = (double)(tmp + (latitude - tmp*100)/60.0);

    // Get North and South status
    str_len = strlen(p);
    if ((*(p+str_len-1) != 'N') && (*(p+str_len-1) != 'S')){
      North_or_South[0] = '0';
      North_or_South[1] = '\0';
    } else {
      North_or_South[0] = *(p+str_len-1);
      North_or_South[1] = '\0';
    }

    p = strtok(NULL, ",");  // longitude
    sprintf(str_longitude, "%s", p);
    longitude = strtod(p, NULL);

    // Get West and East status
    str_len = strlen(p);
    if ((*(p+str_len-1) != 'W') && (*(p+str_len-1) != 'E')){
      West_or_East[0] = '0';        
      West_or_East[1] = '\0';
    } else {
      West_or_East[0] = *(p+str_len-1);
      West_or_East[1] = '\0';
    }

    tmp = (int)(longitude / 100);
    longitude = (double)(tmp + (longitude - tmp*100)/60.0);

    // if(North_or_South[0] == 'S'){
    //     // latitude = 0.0 - latitude;
    // } else if(North_or_South[0] = 'N'){
    //     latitude = 0.0 - latitude;
    // }

    // if(West_or_East[0] == 'W'){
    //     // longitude = 0.0 - longitude;
    // } else if(West_or_East[0] = 'E'){
    //     longitude = 0.0 - longitude;
    // }

    doubleToString(longitude, latitude);
  }
  else{
    return false;
  }
  return true;
}

/**
 * Convert double coordinate data to string
 */
void Quectel_EC2x::doubleToString(double longitude, double latitude)
{
  int u8_lon = (int)longitude;
  int u8_lat = (int)latitude;
  uint32_t u32_lon = (longitude - u8_lon)*1000000;
  uint32_t u32_lat = (latitude - u8_lat)*1000000;

  sprintf(str_longitude, "%d.%lu", u8_lon, u32_lon);
  sprintf(str_latitude, "%d.%lu", u8_lat, u32_lat);
}

/**
 * Set outpu sentences in NMEA mode
*/
boolean Quectel_EC2x::enable_NMEA_mode()
{
  if(!_AtSerial.WriteCommandAndWaitForResponse("AT+QGPSCFG=\"nmeasrc\",1\r\n", "OK", CMD, 2000)){
        return false;
  }
  return true;
}

/**
 * Disable NMEA mode
*/
boolean Quectel_EC2x::disable_NMEA_mode()
{
  if(!_AtSerial.WriteCommandAndWaitForResponse("AT+QGPSCFG=\"nmeasrc\",0\r\n", "OK", CMD, 2000)){
        return false;
  }
  return true;
}

/**
 *  Request NMEA data and save the responce sentence
*/
boolean Quectel_EC2x::NMEA_read_and_save(const char *type, char *save_buff)
{
  char recv_buff[192];
  char send_buff[32];
  char *p = NULL;
  uint16_t i = 0;

  _AtSerial.CleanBuffer(recv_buff,192);
  _AtSerial.CleanBuffer(send_buff,32);  
  sprintf(send_buff, "AT+QGPSGNMEA=\"%s\"\r\n", type);                                                                                                                                                                                          
  _AtSerial.WriteCommand(send_buff);  // Send command
  _AtSerial.ReadUntil(recv_buff, 192, "OK", 1);  // Save response data
  // Serial.print("##DEBUG _AtSerial.ReadUntil: ");
  // Serial.println(recv_buff);
  if(NULL == (p = strstr(recv_buff, "+QGPSGNMEA:")))
  {
    return false;
  }
  p += 12;  
  while((*(p) != '\n') && (*(p) != '\0')) {  // If receive "+QGPSGNMEA:", than keep saving the NMEA sentence 
    save_buff[i++] = *(p++);
  }
  save_buff[i] = '\0';
  // Serial.print("##DEBUG save_buff: ");
  // Serial.println(save_buff);
  return true;

} 

/**
 * Read NMEA data
*/
boolean Quectel_EC2x::read_NMEA(NMEA_type type, char *save_buff)
{
  switch(type){
    case GGA:                
      NMEA_read_and_save("GGA", save_buff);
      break;
    case RMC:
      NMEA_read_and_save("RMC", save_buff);
      break;
    case GSV:
      // NMEA_read_and_save("GSV", save_buff); // Delete GSV aquirement, too much content to be saved, 
      break;
    case GSA:
      NMEA_read_and_save("GSA", save_buff);
      break;
    case VTG:
      NMEA_read_and_save("VTG", save_buff);
      break;
    case GNS:
      NMEA_read_and_save("GNS", save_buff);  // GNS sentence didn't show anything.
      break;    

    default:
      break;
  }

  return true;
}

/**
 * Read NMEA GSV sentence
 * GSV sentence gonna be 6 lines, that's too much content to save as other NMEA data.
 * save_buff should be 512 Bytes size at least. 
*/
boolean Quectel_EC2x::read_NMEA_GSV(char *save_buff)
{
  char recv_buff[512];
  char *p;
  uint16_t i = 0;

  _AtSerial.CleanBuffer(recv_buff,192);                                                                                                                                                                                                                  
  _AtSerial.WriteCommand("AT+QGPSGNMEA=\"GSV\"\r\n");  // Send command
  _AtSerial.ReadUntil(recv_buff, 512, "OK", 1);  // Save response data  
  // Serial.print("##DEBUG _AtSerial.ReadUntil: ");
  // Serial.println(recv_buff);

  if(NULL == (p = strstr(recv_buff, "+QGPSGNMEA:")))
  {
    return false;
  }

  // while(NULL != (p = strstr((recv_buff+i), "+QGPSGNMEA:")))
  while(NULL != (p = strstr(p, "+QGPSGNMEA:")))
  {
    p += 12;    
    while((*(p) != '\n') && (*(p) != '\0')) {  // If receive "+QGPSGNMEA:", than keep saving the NMEA sentence 
      save_buff[i++] = *(p++);
    }
  }
  
  // Serial.print("##DEBUG save_buff: ");
  // Serial.println(save_buff);
  return true;

} 
