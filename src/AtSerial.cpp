#include "AtSerial.h"

AtSerial::AtSerial(HardwareSerial* serial) : _Serial(serial)
{

}

bool AtSerial::WaitForAvailable(Stopwatch* sw, uint32_t timeout) const
{
	while(!_Serial->available())
	{
		if(timeout > 0 && sw != NULL && sw->ElapsedMilliseconds() >= timeout)
		{
			return false;
		}
	}
	return true;
}

void AtSerial::FlushSerial()
{
	while(_Serial->available() > 0){
			_Serial->read();
	}
}

uint16_t AtSerial::ReadLine(char *buffer, int count, unsigned int timeout)
{
	uint16_t i = 0;

	Stopwatch sw;
	sw.Restart();

	while(1) {
			while (_Serial->available()) {
					char c = _Serial->read();
					// prevChar = millis();
					buffer[i++] = c;
					if( (i >= count) || ('\n' == c) || ('\0' == c) ) break;
			}
			if(i >= count)break;
			if (sw.ElapsedMilliseconds() >= timeout) {
					break;
			}
			//If interchar Timeout => return FALSE. So we can return sooner from this function. Not DO it if we dont recieve at least one char (prevChar <> 0)
			// if (((unsigned long) (millis() - prevChar) > chartimeout) && (prevChar != 0)) {
			// 		break;
			// }
	}
	return (uint16_t)(i - 1);
}

uint16_t AtSerial::ReadUntil(char *buffer, int count, char *pattern, unsigned int timeout)
{
	uint16_t i = 0;
	uint8_t sum = 0;
	uint8_t len = strlen(pattern);
	
	Stopwatch sw;
	sw.Restart();
	
	while(1) {
			if(_Serial->available()) {
					char c = _Serial->read();
					// prevChar = millis();
					buffer[i++] = c;
					if(i >= count)break;
					sum = (c==pattern[sum]) ? sum+1 : 0;
					if(sum == len)break;
			}
			if(i >= count)break;
			if (sw.ElapsedMilliseconds() >= timeout) {
					break;
			}
			//If interchar Timeout => return FALSE. So we can return sooner from this function. Not DO it if we dont recieve at least one char (prevChar <> 0)
			// if (((unsigned long) (millis() - prevChar) > chartimeout) && (prevChar != 0)) {
			// 		break;
			// }
	}
	return (uint16_t)(i - 1);
}

uint16_t AtSerial::Read(char* buffer,uint8_t count, uint32_t timeout, uint16_t inchar_timeout)
{
	uint16_t i = 0;
	
	Stopwatch sw;
	sw.Restart();

	while(1) {
			if(_Serial->available()) {
					char c = _Serial->read();
					// prevChar = millis();
					buffer[i++] = c;
					if(i >= count)break;
			}
			if(i >= count)break;
			if (sw.ElapsedMilliseconds() >= timeout) {
					break;
			}

			//If inchar Timeout => return FALSE. So we can return sooner from this function.
			if (sw_inchar.ElapsedMilliseconds() >= inchar_timeout) {
					return false;
			}
	}
	buffer[count - 1] = '\0';

	return (uint16_t)(i - 1);
}
void  AtSerial::CleanBuffer(char* buffer, uint16_t count)
{
	for(int i=0; i < count; i++) {
			buffer[i] = '\0';
	}
}

void AtSerial::WriteCommand(char data)
{
	_Serial->write(data);
}	

void  AtSerial::WriteCommand(const char* cmd)
{
	for(uint16_t i=0; i<strlen(cmd); i++)
	{
			WriteCommand(cmd[i]);
	}	
}

void AtSerial::WriteCommand(char *data, uint16_t dataSize)
{
	for(uint16_t i = 0; i < dataSize; i++)
	{
		_Serial->write(data[i]);
	}	
}	

void  AtSerial::WriteCommand(const __FlashStringHelper* cmd)
{
	int i = 0;
  const char *ptr = (const char *) cmd;
  while (pgm_read_byte(ptr + i) != 0x00) 
	{
    WriteCommand(pgm_read_byte(ptr + i++));
  }
}

void AtSerial::WriteEndMark(void)
{
	WriteCommand((char)26);
}

bool AtSerial::WaitForResponse(const char* resp, DataType type, uint16_t timeout, uint16_t inchar_timeout, bool debug)
{
	int len = strlen(resp);
	int sum = 0;
	
	Stopwatch sw;
	Stopwatch sw_inchar;

	sw.Restart();
	sw_inchar.Restart();

	while(1) {
			if(_Serial->available()) {
					sw_inchar.Restart();
					char c = _Serial->read();
					
					if(debug){
							DEBUG_BYTE(c);
					}

					sum = (c==resp[sum]) ? sum+1 : 0;
					if(sum == len)break;
			}
			if (sw.ElapsedMilliseconds() >= timeout) {
					return false;
			}
			//If inchar Timeout => return FALSE. So we can return sooner from this function.
			if (sw_inchar.ElapsedMilliseconds() >= inchar_timeout) {
					return false;
			}
	}
	DEBUG();
	//If is a CMD, we will finish to read buffer.
	if(type == CMD) FlushSerial();

	return true;
}

// bool AtSerial::WriteCommandAndWaitForResponse(uint8_t* cmd, const char *resp, DataType type, unsigned int timeout, bool debug)
// {
// 	WriteCommand(cmd);
//   return WaitForResponse(resp, type, timeout, debug);
// }

bool AtSerial::WriteCommandAndWaitForResponse(const char* cmd, const char *resp, DataType type, unsigned int timeout, bool debug)
{
	WriteCommand(cmd);
  return WaitForResponse(resp, type, timeout, DEFAULT_INCHAR_TIMEOUT, debug);
}

bool AtSerial::WriteCommandAndWaitForResponse(const __FlashStringHelper* cmd, const char *resp, DataType type, unsigned int timeout, bool debug)
{
	WriteCommand(cmd);
  return WaitForResponse(resp, type, timeout, DEFAULT_INCHAR_TIMEOUT, debug);
}

void AtSerial::AT_Bypass(void)
{
	if(_Serial->available() > 0)
	{
		_debugSerial.write(_Serial->read());
	}
	if(_debugSerial.available() > 0)
	{
		_Serial->write(_debugSerial.read());
	}
}
