/*
 * internet.h
 *
 * Copyright (c) 2017 Seeed Technology Co., Ltd.
 * Website    : www.seeed.cc
 * Author     : lambor
 * Create Time: July 2017
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

#include "stdio.h"
#include "wio_tracker.h"
#include "UART_Interface.h"


enum Protocol {
    CLOSED = 0,
    TCP    = 1,
    UDP    = 2
};

enum SocketAccessMode {
    SOCKET_BUFFER_MODE = 0,
    SOCKET_DIRECT_PUSH_MODE,
    SOCKET_TRANSPARANT_MODE
};

class Ethernet : public WioTracker
{
public:
    uint32_t _ip;
    char ip_string[20];
    char last_URL[128];

    /** Create Ethernet instance
     *  @param number default phone number during mobile communication
     */
    // Ethernet();

    /** Check network registration status
     *  @returns
     *      0 on success
     *      -1 on error
     */
    bool init(void);

    /** Join APN
     * @param apn name
     * @param apn user name
     * @param apn passwd
     * @return
     *        true Jion APN successfully
     *        false failed to join APN
     */
    bool join(const char *apn, const char *username = "", const char *passwd = "");

    /** Get IP address
     *  @return
     *      true on successfully
     *      flase on failed
     */
    bool getIP(void);

    /** parse IP string
     *  @return
     *      ip in hex
     */
    uint32_t str_to_ip(const char* str);

    /** Recover Ip address
     *  @return
     *       IP string
     */
    char* recoverIP();

    /** check network is OK or not
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int networkCheck(void);

    /** build TCP connect
     *  @param  ip  ip address which will connect to
     *  @param  port    TCP server' port number
     *  @returns     
     *      -1 on error
     *      0 on success
     */
    bool connect(const char* ip, int port, int connectType = TCP, uint8_t dataAccessMode = SOCKET_DIRECT_PUSH_MODE);

    /** Write data to socket server
     *  @param  data    data that will be send to socket server
     *  @returns
     *      0 on success
     *      -1 on error
     */
    bool write(char *data);

    /** Read data from 
     * 
     * 
     * 
    */
    bool read();

    /** Get socket status
     *  @returns
     *      -1 on error
     *      0 "Initial" connection has not been established
     *      1 "Openning" client is connecting ro server is trying to listen
     *      2 "Connected" client/incoming connection has been establish
     *      3 "Listening" server is listening 
     *      4 "Closing" connection is closing
     */
    int getSocketStatus();

    /** Check if socket connected
     *  @returns
     *          true on connected
     *          false on disconnect
     */
    bool is_connected(void);

    /** close TCP connection
     *  @returns
     *      0 on success
     *      -1 on error
     */
     bool httpGET(char *url);
     
     /** close TCP connection
     *  @returns
     *      0 on success
     *      -1 on error
     */
    bool httpPUT(char *url);

     /** close TCP connection
     *  @returns
     *      0 on success
     *      -1 on error
     */


    /** close connection
     *  @param connectid, socket id to close 
     *  @returns
     *      0 on success
     *      -1 on error
     */
    bool close(uint8_t connectid = 0);



};

/* End of file */


