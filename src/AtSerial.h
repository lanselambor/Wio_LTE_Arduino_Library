#pragma once

#include <Arduino.h>

#include "Stopwatch.h"
#include "board_config.h"

#define DEFAULT_TIMEOUT           1000   //miliseconds

#if defined (ARDUINO_ARCH_SAMD)
	#define _debugSerial Serial
#elif defined (ARDUINO_ARCH_AVR)
	#define _debugSerial Serial
#elif defined(ARDUINO_ARCH_SEEED_STM32F4)
	#define _debugSerial SerialUSB
#else
	#error "Architecture not matched"
#endif

#define Log(x)     (_debugSerial.print(x))
#define Logln(x)     (_debugSerial.println(x))
#define Log_info(x)     (_debugSerial.print("[INFO] "), _debugSerial.println(x))
#define Log_error(x)     (_debugSerial.print("[ERROR] "), _debugSerial.println(x))
#define Log_prolog_in(x)       (_debugSerial.print("<<"), _debugSerial.println(x))
#define Log_prolog_out(x)      (_debugSerial.print(">>"), _debugSerial.println(x))

#if(1==UART_DEBUG)
#define ERROR(x)            _debugSerial.println(x)
#define DEBUG(x)            _debugSerial.println(x)
#define DEBUG_BYTE(x)       _debugSerial.write(x)
#else
#define ERROR(x)
#define DEBUG(x)
#define DEBUG_BYTE(x)
#endif

#if (1==UART_DEBUG)
#define debugPrint(x)     _debugSerial.print(x)
#define debugPrintln(x)   _debugSerial.println(x)
#define debugPrintIn(x)     _debugSerial.print("<< "),_debugSerial.printl(x)
#define debugPrintOut(x)     _debugSerial.print(">> "),_debugSerial.println(x)
#else
#define debugPrint(x)
#define debugPrintln(x)
#define debugPrintOut(x)
#endif

enum DataType {
    CMD     = 0,
    DATA    = 1,
};

class AtSerial
{
	public:
		HardwareSerial* _Serial;

		AtSerial(HardwareSerial* serial);
		bool WaitForAvailable(Stopwatch* sw, uint32_t timeout) const;
		void FlushSerial();
		uint16_t ReadLine(char *buffer, int count, unsigned int timeout = DEFAULT_TIMEOUT);
		uint16_t ReadUntil(char *buffer, int count, char *pattern, unsigned int timeout = DEFAULT_TIMEOUT);
		uint16_t Read(char* buffer,uint8_t count, uint32_t timeout = DEFAULT_TIMEOUT);
		void CleanBuffer(char* buffer, uint16_t count);
		void WriteCommand(char data);
		void WriteCommand(const char* cmd);
		void WriteCommand(const __FlashStringHelper* cmd);
		void WriteEndMark(void);
		bool WaitForResponse(const char* resp, DataType type, unsigned int timeout = DEFAULT_TIMEOUT, bool debug=false);
		bool WriteCommandAndWaitForResponse(const char* cmd, const char *resp, DataType type, unsigned int timeout = DEFAULT_TIMEOUT*5, bool debug=false);
		bool WriteCommandAndWaitForResponse(const __FlashStringHelper* cmd, const char *resp, DataType type, unsigned int timeout = DEFAULT_TIMEOUT, bool debug=false);
		void AT_Bypass(void);
};
