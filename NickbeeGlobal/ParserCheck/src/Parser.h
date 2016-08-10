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

bool ParseDataAvailable(void);
uint8_t ParseGet(void);
void ParseInit(void);
bool Parse(uint8_t c);
#endif /* PARSER_H_ */