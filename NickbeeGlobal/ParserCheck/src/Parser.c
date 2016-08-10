/*
 * Parser.c
 *
 * Created: 9/29/2013 10:03:27 AM
 *  Author: pc
 */ 
#include <asf.h>
#include <stdbool.h> // for bool
#include <string.h>
//#define PARSER_CHECK	// comment out when debugged
#define PARSER_MAIN
#include "Parser.h"

#define PARSE_BUFFER_SIZE 64
volatile static uint8_t ParseBuffer[PARSE_BUFFER_SIZE];
volatile static int ParseBufferFill;
volatile static int ParseBufferEmpty;
bool MessageReady;
enum eParseState ParseState = WAIT_SYNC1;
#ifdef PARSER_CHECK
uint8_t rawmessage[] = {0xaa,0xaa,6,1,2,3,4,0,0,10};
//uint8_t rawmessage[] = {0xaa,0xaa,6,1,2,3,4,0,0,20};
#endif

void ParseInit(void)
{
	ParseBufferFill = 0;
	ParseBufferEmpty = 0;
	ParseState = WAIT_SYNC1;
#ifdef PARSER_CHECK
	memcpy(ParseBuffer,rawmessage,sizeof(rawmessage));
	ParseBufferFill = sizeof(rawmessage);
#endif
}

bool ParseDataAvailable(void)
{
	return (ParseBufferFill != ParseBufferEmpty);
}

uint8_t ParseGet(void)
{
	uint8_t c = ParseBuffer[ParseBufferEmpty++];
	ParseBufferEmpty %= PARSE_BUFFER_SIZE;
	return c;
}

uint8_t packetLengthUnread;
uint8_t cs;
bool Parse(uint8_t c)
{
	bool rc = false;
	switch(ParseState)
	{
		case WAIT_SYNC1:
			ParseState = (c == 0xaa) ? WAIT_SYNC2 : WAIT_SYNC1;
			cs = 0;
			break;
		case WAIT_SYNC2:
			ParseState = (c == 0xaa) ? GET_PL : WAIT_SYNC1;
			break;
		case GET_PL:
			packetLengthUnread = c;
			ParseState = READ_PACKET;
			break;
		case READ_PACKET:
			cs += c;
			ParseState = (--packetLengthUnread == 0) ? READ_CS : READ_PACKET;
			break;
		case READ_CS:
			ParseState = WAIT_SYNC1;
			return (c == cs);
			break;
	}
	return false;
}
