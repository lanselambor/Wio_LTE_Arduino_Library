#include <stdio.h>

#include "LTEClient.h"
#include "Stopwatch.h"

LTEClient::LTEClient()
{

}

void LTEClient::Init()
{
    _moduleSerial.begin(115200);
}

bool LTEClient::waitForNetworkRegister(uint32_t timeout)
{
    Stopwatch sw;
    sw.Restart();

    // Check CS Registration
    while(true)
    {
        if(check_with_cmd("AT+CEREG?\r\n", "+CEREG: 0,1", CMD, 2, 500) || // Home network
        check_with_cmd("AT+CEREG?\r\n", "+CEREG: 0,5", CMD, 2, 500)) // Roaming
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
        if(check_with_cmd("AT+CGREG?\r\n", "+CGREG: 0,1", CMD, 2, 2000) || // Home network
        check_with_cmd("AT+CGREG?\r\n", "+CGREG: 0,5", CMD, 2, 2000)) // Roaming
        {
            break;
        }
        delay(100);
        if (sw.ElapsedMilliseconds() >= (unsigned long)timeout) return RET_ERR;
    }
  
  return RET_OK;
}

bool LTEClient::Activate(const char* APN, const char* userName, const char* password, long waitForRegistTimeout)
{
    char sendBuffer[64] = {0};
    Stopwatch sw;

    sprintf(sendBuffer, "AT+QICSGP=1,1,\"%s\",\"%s\",\"%s\",1\r\n", APN, userName, password);
    if(!check_with_cmd(sendBuffer, "OK", CMD, DEFAULT_TIMEOUT, 500, UART_DEBUG))
    {
        return RET_ERR;
    }

    sw.Restart();

    if(!waitForNetworkRegister(120000)) return RET_ERR;
#ifdef UART_DEBUG
    char dbg[100];
    sprintf(dbg, "Elapsed time is %lu[msec.].", sw.ElapsedMilliseconds());
    debugPrintln(dbg);

    check_with_cmd("AT+CREG?\r\n", "OK", CMD, DEFAULT_TIMEOUT, 500, UART_DEBUG);
    check_with_cmd("AT+CGREG?\r\n", "OK", CMD, DEFAULT_TIMEOUT, 500, UART_DEBUG);
    check_with_cmd("AT+CEREG?\r\n", "OK", CMD, DEFAULT_TIMEOUT, 500, UART_DEBUG);
#endif // UART_DEBUG

	sw.Restart();
	
    send_cmd("AT+QIACT=1\r\n");		

	// for debug.
#ifdef UART_DEBUG
	if (!check_with_cmd("AT+QIACT?\r\n", "OK", CMD, DEFAULT_TIMEOUT, 500, UART_DEBUG)) return RET_ERR;
#endif // UART_DEBUG

	return RET_OK;
}

bool LTEClient::Deactivate()
{
    if (!check_with_cmd("AT+QIDEACT=1\r\n", "OK", CMD, DEFAULT_TIMEOUT, 4000, UART_DEBUG)) return RET_ERR;

	return RET_OK;
}

/**
 * Get IP address
 */
bool LTEClient::getIPAddr(void)
{
    // AT+CGPADDR=1
    // +CGPADDR: 1,"10.33.124.114"

    char *p;
    char rxBuf[64] = {'\0'};
    uint8_t a0, a1, a2, a3;

    // Get IP address
    send_cmd("AT+CGPADDR=1\r\n");
    read_buffer(rxBuf, sizeof(rxBuf), 1);

    if(NULL != (p = strstr(rxBuf, "+CGPADDR:")))
    {
        if(4 == sscanf(p, "+CGPADDR: %*d,\"%d.%d.%d.%d\"", &a0, &a1, &a2, &a3))
        {      
            _u32ip = TUPLE_TO_U32IP(a0, a1, a2, a3);
            sprintf(_str_ip, IP_FORMAT, a0, a1, a2, a3);
        }
    }
    else
    {
        Log_error("+CGPADDR failed");
        return RET_ERR;
    }

    return RET_OK;
}

/**
 * Get operator name
*/
bool LTEClient::getOperator()
{
    // AT+COPS?
    // +COPS: 0,0,"CHN-UNICOM",7

    char *p, *s;
    char rxBuf[64] = {'\0'};

    send_cmd("AT+COPS?\r\n");
    read_buffer(rxBuf, sizeof rxBuf);  

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
uint32_t LTEClient::str_to_u32(const char* str)
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
 * Create socket and return socket id
 * 
 * @param sock_type use TCP or UDP to create the socket type
 * @param port local port for socket
 * @return socket id
*/
int LTEClient::createSocket(Socket_type sock_type, uint16_t port)
{

}

/**
 * Set host server by ip/domain and port, then connect to the server 
 * @param sockid is local socket id
 * @param host is remote server ip or domain
 * @param port is remote server port
 * @return true for success, false for failure
*/
bool LTEClient::sockConnect(uint8_t sockid, char *host, uint16_t port)
{

}

/**
 * Close connected socket
 * 
 * @param sockid socket id will be closed
 * @return true for success, false for failure.
*/

bool LTEClient::sockClose(int sockid)
{

}

/**
 * Get last socket connect error
 * 
 * @return socket error code
*/
int LTEClient::getSocketError(void)
{

}

/**
 * Write one byte data to opened socket
 * 
 * @param sockid socket id created before 
 * @param oneByte one byte will be sent to remote
 * @return true for success, false for failure.
 * 
*/
bool LTEClient::socketWrite(uint8_t sockid, char oneByte)
{

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
bool LTEClient::socketWrite(uint8_t sockid, char *data, uint16_t dataSize)
{

}

/**
 * Write one byte data to opened socket
 * 
 * @param data data array will be sent to remote
 * @param dataSize
 * @return - none
 * 
*/
void LTEClient::socketWrite(uint8_t *data, uint16_t dataSize)
{

}

/**
 * Write one byte to opened socket
 * 
 * @param data one byte will be sent to remote
 * @return - none
 * 
*/
void LTEClient::socketWrite(uint8_t data)
{

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
bool LTEClient::udpSendTo(uint8_t sockid, char *host, uint16_t port, char oneByte)
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
bool LTEClient::udpSendTo(uint8_t sockid, char *host, uint16_t port, char *data, uint16_t dataSize)
{

}