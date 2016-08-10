/*
 * Application.h
 *
 * Created: 9/29/2013 10:13:48 AM
 *  Author: pc
 */ 


#ifndef APPLICATION_H_
#define APPLICATION_H_

// API from main to App
void appInit(void);
void gpioEvent(ioport_pin_t gpioNumber,bool value);
void msgRx(enum eChannel,struct sMessage *msg);
void appTimerExpired(uint32_t);
void msgSent(enum eChannel);
// API from app to Main
void appTimerStart(unsigned msec);
void appTimerStop(void);
void msgTx(enum eChannel channel,uint8_t *,int);
// utilities
uint8_t CheckSum(uint8_t *buff,int length);
int shortMessage(uint8_t dest,enum eMessageID msg,enum eOpCode opcode);
int longMessage(uint8_t dest,enum eMessageID msg,enum eOpCode opcode,unsigned datalength,uint8_t *data);
#endif /* APPLICATION_H_ */