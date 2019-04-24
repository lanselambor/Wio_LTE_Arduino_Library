#pragma once

#include <Arduino.h>

#include "Stopwatch.h"
#include "AtSerial.h"


#define MAX_TXBUF_LEN 1024
#define MAX_RXBUF_LEN 1024

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

typedef enum {
    GGA = 0,
    RMC,
    GSV,
    GSA,
    VTG,
    GNS
} NMEA_type;

class Quectel_LTE
{
    public:
        uint32_t _u32ip = 0;
		char _str_ip[20] = {'\0'};
        bool usedSockId[7] = {false};
        char _apn[32] = {'\0'};
		char _user[32] = {'\0'};
		char _passwd[32] = {'\0'};
        char _operator[32] = {'\0'};
        char _TxBuf[MAX_TXBUF_LEN] = {0};
        char _RxBuf[MAX_RXBUF_LEN] = {0};

        double longitude;
        double latitude;
        char str_longitude[16];
        char str_latitude[16];
        double ref_longitude = 22.584322;
        double ref_latitude = 113.966678;
        char North_or_South[2];
        char West_or_East[2];

        AtSerial _AtSerial;

        Quectel_LTE();        
        void initialize();
/************************** Standard AT command **************************/        
        bool initialAtCommands(void);
        bool checkSIMStatus(void);
/************************** Network startup **************************/            
        bool isAlive(uint32_t timeout = 5000);
        bool waitForNetworkRegister(uint32_t timeout = 120000);
        bool Activate(const char* APN, const char* userName, const char* password, long waitForRegistTimeout = 120000);
        bool Deactivate();
          
        bool getIPAddr(void);
		bool getOperator(void);
        uint32_t str_to_u32ip(char* str);

        bool getSignalStrength(int *buffer);
/************************** Socket TCP UDP **************************/
        bool sockOpen(const char *host, int port, Socket_type connectType);
        bool sockClose(int sockid);
        bool sockWrite(uint8_t sockid, char *data, uint16_t dataSize);
        bool sockWrite(uint8_t sockid, char *data);
        uint16_t sockReceive(uint8_t sockid, char *data, uint16_t dataSize, uint32_t timeoutMs=100);
        bool udpSendTo(uint8_t sockid, char *host, uint16_t port, char oneByte);               
        bool udpSendTo(uint8_t sockid, char *host, uint16_t port, char *data, uint16_t dataSize);

/************************** HTTP(S) **************************/
        bool httpSetUrl(char *url);
        bool httpGet(char *url, uint8_t *data, uint16_t dataSize);
        // bool httpPost(char *url, );
/************************** MQTT **************************/

/************************** GNSS **************************/
        bool open_GNSS(int mode);
        bool close_GNSS(void);
        bool open_GNSS(void);
        void doubleToString(double longitude, double latitude);        
        bool getCoordinate(void);            
        bool dataFlowMode(void);
        bool enable_NMEA_mode();        
        bool disable_NMEA_mode();        
        bool NMEA_read_and_save(const char *type, char *save_buff);        
        bool read_NMEA(NMEA_type data_type, char *data);       
        bool read_NMEA_GSV(char *save_buff);

};