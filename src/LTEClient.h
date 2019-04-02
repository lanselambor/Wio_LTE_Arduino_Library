#pragma once

#include <Arduino.h>
#include "UART_Interface.h"


#define STR_AT	"AT"
#define STR_OK	"OK"
#define STR_ERR	"ERROR"

#define CTRL_Z '\x1A'
#define IP_FORMAT "%d.%d.%d.%d"

#define U32IP_TO_TUPLE(x) (uint8_t)(((x) >> 24) & 0xFF), \
                       (uint8_t)(((x) >> 16) & 0xFF), \
                       (uint8_t)(((x) >> 8) & 0xFF), \
                       (uint8_t)(((x) >> 0) & 0xFF)

#define TUPLE_TO_U32IP(a1, a2, a3, a4) ((((uint32_t)a1) << 24) | \
                                     (((uint32_t)a2) << 16) | \
                                      (((uint32_t)a3) << 8) | \
                                      (((uint32_t)a4) << 0))

enum Socket_type {
    CLOSED = 0,
    TCP    = 6,
    UDP    = 17
};

enum CheckState {
    RET_OK = true,
    RET_ERR = false,
};

class LTEClient
{
    public:
        uint32_t _u32ip = 0;
		char _str_ip[20] = {'\0'};
        bool usedSockId[7] = {false};
        char _apn[32] = {'\0'};
		char _user[32] = {'\0'};
		char _passwd[32] = {'\0'};
        char _operator[32] = {'\0'};

        
        LTEClient();
        void Init();
/************************** Network startup **************************/    
        

        bool waitForNetworkRegister(uint32_t timeout = 120000);
        bool Activate(const char* APN, const char* userName, const char* password, long waitForRegistTimeout = 120000);
        bool Deactivate();

        bool getIPAddr(void);
		bool getOperator(void);
        uint32_t str_to_u32(const char* str);

/************************** Socket TCP UDP **************************/
        int createSocket(Socket_type sock_type, uint16_t port = 0);
        bool sockConnect(uint8_t sockid, char *host, uint16_t port);
        bool sockClose(int sockid);
        int getSocketError(void);        
        bool socketWrite(uint8_t sockid, char oneByte);
        bool socketWrite(uint8_t sockid, char *data, uint16_t dataSize);
        void socketWrite(uint8_t *data, uint16_t dataSize);
        void socketWrite(uint8_t data);
        bool udpSendTo(uint8_t sockid, char *host, uint16_t port, char oneByte);               
        bool udpSendTo(uint8_t sockid, char *host, uint16_t port, char *data, uint16_t dataSize);

/************************** HTTP(S) **************************/

/************************** MQTT **************************/
};