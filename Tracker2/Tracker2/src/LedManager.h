/*
 * LedManager.h
 *
 * Created: 01-Feb-15 12:30:19 AM
 *  Author: Saba
 */ 


#ifndef LEDMANAGER_H_
#define LEDMANAGER_H_
enum eLed {LED1,LED2,NUM_LEDS};
void LedInit(void);
void LedOn(enum eLed l);
void LedOff(enum eLed l);
void LedToggle(enum eLed l);
#endif /* LEDMANAGER_H_ */