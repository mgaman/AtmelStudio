/*
 * LedManager.c
 *
 * Created: 01-Feb-15 12:30:37 AM
 *  Author: Saba
 */ 

#include <asf.h>
#include "LedManager.h"

bool ledstate[NUM_LEDS];

void LedInit(void)
{
	LedOff(LED1);
	LedOff(LED2);
}

void LedOff(enum eLed l)
{
	switch(l)
	{
		case LED1:
			ioport_set_pin_level(ONBOARD_LED_2,LOW);
			ledstate[l] = false;
			break;
		case LED2:
			ioport_set_pin_level(ONBOARD_LED_3,LOW);
			ledstate[l] = false;
			break;
		default:
			break; // should never get here
	}
};
void LedOn(enum eLed l)
{
	switch(l)
	{
		case LED1:
			ioport_set_pin_level(ONBOARD_LED_2,HIGH);
			ledstate[l] = true;
			break;
		case LED2:
			ioport_set_pin_level(ONBOARD_LED_3,HIGH);
			ledstate[l] = true;
			break;
		default:
			break; // should never get here
	}
};

void LedToggle(enum eLed l)
{
	if (ledstate[l])
		LedOff(l);
	else
		LedOn(l);
}
