#ifndef _PCMSGHANDLER_H_
#define _PCMSGHANDLER_H_

#include "pcdatahandler.h"

typedef struct
{
	uint8_t major;
	uint8_t minor;
	uint8_t revision;
}version_t;

#define VER_MAJOR				1
#define VER_MINOR               1
#define VER_REV                 0

typedef enum {
  MSG_SHAKEHANDE = 1,			// shake hande with PC
  MSG_VERSION = 2,				// return firmware version
  MSG_RESET = 3,                // reset device
  MSG_SETTINGS = 4,             // reset device
  MSG_RF_SETTINGS = 5,  
  MSG_YUVALIT_ACK = 6,
  MSG_YUVALIT_STATE = 7,
  EEPROM_GET = 8,         // get from eeprom address, number of bytes
  EEPROM_PUT = 9,         // put to eeprom address, data (deduce length)
  EEPROM_DATA = 10        // data from eeprom address, data (deduce length)
} PC_MSG_ID;


void PCMessagesHandler_Init(void);
void PCRecievedMessagesHandler(uint8_t* pMessage);

bool PCSendMessage(uint8_t* Buffer,uint8_t Len);

void UART_StartTransmition(void);

#ifdef UARTHANDLER_MAIN
PC_PACKET_HEADER pcmessage;
#else
extern PC_PACKET_HEADER pcmessage;
#endif
#endif
