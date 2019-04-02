/*
 * gnss.h
 * A library for SeeedStudio GPS Tracker BT 
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

#include "wio_tracker.h"
#include "UART_Interface.h"
#include "stdio.h"

typedef enum NMEA_TYPE{
    GGA = 0,
    RMC,
    GSV,
    GSA,
    VTG,
    GNS
} NMEA_type;


class GNSS : public WioTracker
{
public: 
    double longitude;
    double latitude;
    char str_longitude[16];
    char str_latitude[16];
    double ref_longitude = 22.584322;
    double ref_latitude = 113.966678;
    char North_or_South[2];
    char West_or_East[2];
    

    bool open_GNSS(int mode);
    bool close_GNSS(void);

    /** 
     *  open GNSS
     */
    bool open_GNSS(void);
    
    /**
     * Convert double coordinate data to string
     */
    void doubleToString(double longitude, double latitude);


    /** Get coordinate infomation
     *
     */
    bool getCoordinate(void);

    /**
     * Aquire GPS sentence
     */    
    bool dataFlowMode(void);
    
    /**
     * Set outpu sentences in NMEA mode
    */
    bool enable_NMEA_mode();
    
    /**
     * Disable NMEA mode
    */
    bool disable_NMEA_mode();

    /**
     *  Request NMEA data and save the responce sentence
    */
    bool NMEA_read_and_save(const char *type, char *save_buff);

    /**
     * Read NMEA data
    */
    bool read_NMEA(NMEA_type data_type, char *recv_buff);

    /**
     * Read NMEA GSV sentence
     * GSV sentence gonna be 6 lines, that's too much content to save as other NMEA data.
     * save_buff should be 512 Bytes size at least. 
    */
    bool read_NMEA_GSV(char *save_buff);
};

