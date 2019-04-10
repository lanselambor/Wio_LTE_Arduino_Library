#pragma once

#include <Arduino.h>

#include "Stopwatch.h"
#include "AtSerial.h"

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

class Quectel_EC2x
{
    public:
        uint32_t _u32ip = 0;
		char _str_ip[20] = {'\0'};
        boolean usedSockId[7] = {false};
        char _apn[32] = {'\0'};
		char _user[32] = {'\0'};
		char _passwd[32] = {'\0'};
        char _operator[32] = {'\0'};

        double longitude;
        double latitude;
        char str_longitude[16];
        char str_latitude[16];
        double ref_longitude = 22.584322;
        double ref_latitude = 113.966678;
        char North_or_South[2];
        char West_or_East[2];

        AtSerial _AtSerial;

        Quectel_EC2x();        
        void Init();
        
/************************** Network startup **************************/            
        boolean isAlive(uint32_t timeout = 5000);
        boolean waitForNetworkRegister(uint32_t timeout = 120000);
        boolean Activate(const char* APN, const char* userName, const char* password, long waitForRegistTimeout = 120000);
        boolean Deactivate();

        boolean getIPAddr(void);
		boolean getOperator(void);
        uint32_t str_to_u32(const char* str);
                
        boolean getSignalStrength(int *buffer);

/************************** Socket TCP UDP **************************/
        // boolean sockOpen(const char *host, Socket_type port, Socket_type connectType);
        boolean sockOpen(const char *host, int port, Socket_type connectType);
        boolean sockClose(int sockid);
        boolean socketWrite(uint8_t sockid, char *data, uint16_t dataSize);
        boolean socketWrite(uint8_t sockid, char *data);
        boolean socketReceive(uint8_t sockid, char *data, uint16_t dataSize );
        boolean udpSendTo(uint8_t sockid, char *host, uint16_t port, char oneByte);               
        boolean udpSendTo(uint8_t sockid, char *host, uint16_t port, char *data, uint16_t dataSize);

/************************** HTTP(S) **************************/

/************************** MQTT **************************/

/************************** GNSS **************************/
        boolean open_GNSS(int mode);
        boolean close_GNSS(void);
        boolean open_GNSS(void);
        void doubleToString(double longitude, double latitude);        
        boolean getCoordinate(void);            
        boolean dataFlowMode(void);
        boolean enable_NMEA_mode();        
        boolean disable_NMEA_mode();        
        boolean NMEA_read_and_save(const char *type, char *save_buff);        
        boolean read_NMEA(NMEA_type data_type, char *recv_buff);       
        boolean read_NMEA_GSV(char *save_buff);

};