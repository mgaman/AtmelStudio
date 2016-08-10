/*
 * Parser.c
 *
 * Created: 9/29/2013 10:03:27 AM
 *  Author: pc
 */ 
#include <asf.h>
#include <board.h>
#include <conf_board.h>
#include <stdbool.h> // for bool
#define PARSER_MAIN
#include "Parser.h"

#define PARSE_BUFFER_SIZE 64
volatile static uint8_t UartParseBuffer[PARSE_BUFFER_SIZE];
volatile  unsigned UartParseBufferFill;
volatile  unsigned UartParseBufferEmpty;
//bool MessageReady;
enum eParseState UartParseState = WAIT_SYNC1;
// place complete parsed message here
struct sMessage UartParsedMessage;
volatile uint8_t *pFillUartParsedMessage;
void UartParseInit(void)
{
	UartParseBufferFill = 0;
	UartParseBufferEmpty = 0;
	UartParseState = WAIT_SYNC1;
	UartParsedMessage.sync1 = SYNC_CHAR;
	UartParsedMessage.sync2 = SYNC_CHAR;
}

bool UartParseDataAvailable(void)
{
	return (UartParseBufferFill != UartParseBufferEmpty);
}

uint8_t UartParseGet(void)
{
	uint8_t c = UartParseBuffer[UartParseBufferEmpty++];
	UartParseBufferEmpty %= PARSE_BUFFER_SIZE;
	return c;
}
volatile uint8_t cs;
static uint8_t packetLengthUnread;
bool UartParse(uint8_t c)
{
	bool rc = false;
	switch(UartParseState)
	{
		case WAIT_SYNC1:
			UartParseState = (c == 0xaa) ? WAIT_SYNC2 : WAIT_SYNC1;
			cs = 0;
			break;
		case WAIT_SYNC2:
			UartParseState = (c == 0xaa) ? GET_PL : WAIT_SYNC1;
			break;
		case GET_PL:
			packetLengthUnread = c;
			cs = c;
			UartParsedMessage.packetLength = c;
			UartParseState = READ_PACKET;
			pFillUartParsedMessage = (uint8_t *)&UartParsedMessage.SourceID;
			break;
		case READ_PACKET:
			cs += c;
			*pFillUartParsedMessage++ = c;
			UartParseState = (--packetLengthUnread == 0) ? READ_CS : READ_PACKET;
			break;
		case READ_CS:
//			*pFillUartParsedMessage = c;
			UartParseState = WAIT_SYNC1;
			rc = (c == cs);
			break;
	}
	return rc;
}

// uart handler, add latest character to circular buffer
ISR(USARTD1_RXC_vect)
{
	UartParseBuffer[UartParseBufferFill++] = usart_getchar(MMI_UART);
	UartParseBufferFill %= PARSE_BUFFER_SIZE;
}
