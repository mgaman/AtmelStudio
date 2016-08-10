/*
 * Parser.h
 *
 * Created: 9/29/2013 10:05:09 AM
 *  Author: pc
 */ 


#ifndef PARSER_H_
#define PARSER_H_
// Parsing state machine
enum eParseState { WAIT_SYNC1,WAIT_SYNC2,GET_PL,READ_PACKET,READ_CS};
#define SYNC_CHAR 0xAA

bool UartParseDataAvailable(void);
uint8_t UartParseGet(void);
void UartParseInit(void);
bool UartParse(uint8_t c);
#endif /* PARSER_H_ */