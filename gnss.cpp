/*
 * gnss.cpp
 * A library for SeeedStudio GPS Tracker LTE GNSS
 *
 * Copyright (c) 2017 Seeed Technology Co., Ltd.
 * Website    : www.seeed.cc
 * Author     : lambor
 * Create Time: June 2017
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

#include "gnss.h"


bool GNSS::close_GNSS()
{
  int errCounts = 0;

  //Open GNSS funtion
  while(!check_with_cmd("AT+QGNSSC?\n\r", "+QGNSSC: 0", CMD, 2, 2000, UART_DEBUG)){
      errCounts ++;
      if(errCounts > 100){
        return false;
      }
      check_with_cmd("AT+QGNSSC=0\n\r", "OK", CMD, 2, 2000, UART_DEBUG);
      delay(1000);
  }

  return true;
}

bool GNSS::dataFlowMode(void)
{
    // Make sure that "#define UART_DEBUG" is uncomment.
    send_cmd("AT+QGPSLOC?\r\n");
    return wait_for_resp("OK", CMD, 2, 2000, true);
}

bool GNSS::open_GNSS(void)
{
  int errCounts = 0;

  //Open GNSS funtion
  while(!check_with_cmd("AT+QGPS?\r\n", "+QGPS: 1", CMD, 2, 2000, UART_DEBUG)){
      errCounts ++;
      if(errCounts > 5){
        return false;
      }
      check_with_cmd("AT+QGPS=1\r\n", "OK", CMD, 2, 2000, UART_DEBUG);
      delay(1000);
  }

  return true;
}

bool GNSS::getCoordinate(void)
{
  int tmp = 0;
  char *p = NULL;
  uint8_t str_len = 0;
  char buffer[128];

  clean_buffer(buffer, 128);
  send_cmd("AT+QGPSLOC?\r\n");
  read_buffer(buffer, 128, 2);
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

void GNSS::doubleToString(double longitude, double latitude)
{
  int u8_lon = (int)longitude;
  int u8_lat = (int)latitude;
  uint32_t u32_lon = (longitude - u8_lon)*1000000;
  uint32_t u32_lat = (latitude - u8_lat)*1000000;

  sprintf(str_longitude, "%d.%lu", u8_lon, u32_lon);
  sprintf(str_latitude, "%d.%lu", u8_lat, u32_lat);
}

bool GNSS::enable_NMEA_mode()
{
  if(!check_with_cmd("AT+QGPSCFG=\“nmeasrc\",1\r\n", "OK", CMD, DEFAULT_TIMEOUT, 2000)){
        return false;
  }
  return true;
}

bool GNSS::disable_NMEA_mode()
{
  if(!check_with_cmd("AT+QGPSCFG=\“nmeasrc\",0\r\n", "OK", CMD, DEFAULT_TIMEOUT, 2000)){
        return false;
  }
  return true;
}

bool GNSS::NMEA_read_and_save(const char *type, char *save_buff)
{
  char recv_buff[192];
  char send_buff[32];
  char *p = NULL;
  uint16_t i = 0;

  clean_buffer(recv_buff,192);
  clean_buffer(send_buff,32);  
  sprintf(send_buff, "AT+QGPSGNMEA=\"%s\"\r\n", type);                                                                                                                                                                                          
  send_cmd(send_buff);  // Send command
  read_string_until(recv_buff, 192, "OK", 1);  // Save response data
  // Serial.print("##DEBUG read_string_until: ");
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

bool GNSS::read_NMEA(NMEA_type type, char *save_buff)
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
bool GNSS::read_NMEA_GSV(char *save_buff)
{
  char recv_buff[512];
  char *p;
  uint16_t i = 0;

  clean_buffer(recv_buff,192);                                                                                                                                                                                                                  
  send_cmd("AT+QGPSGNMEA=\"GSV\"\r\n");  // Send command
  read_string_until(recv_buff, 512, "OK", 1);  // Save response data  
  // Serial.print("##DEBUG read_string_until: ");
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
