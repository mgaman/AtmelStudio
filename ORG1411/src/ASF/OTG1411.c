/*
 * OTG1411.c
 *
 * Created: 30-Jan-15 9:20:52 AM
 *  Author: Saba
 */ 


//#include <avr/io.h>
#include <asf.h>
#include "LedManager.h"
void On_Off_Pulse(void);
void OTG1411Reset(void);
void OTG1411main(void);

void On_Off_Pulse()
{
	// see Hornet ORG14xx datasheet 7.2.2
	ioport_set_pin_level(GPS_ON_OFF_PIN,HIGH);
	delay_ms(100);
	ioport_set_pin_level(GPS_ON_OFF_PIN,LOW);
	delay_ms(1100);
}

void OTG1411Reset()
{
	bool wakeup_high;
	// bring gps to known state see Hornet ORG14xx datasheet Chapter 9.1
	// on_off pulse alternately switches gps on/off but as we don't know its original state
	// force it into power off then power on
	On_Off_Pulse();
	wakeup_high = ioport_get_pin_level(GPS_WAKEUP_PIN);
	while (!wakeup_high)
	{
		On_Off_Pulse();
		wakeup_high = ioport_get_pin_level(GPS_WAKEUP_PIN);
	}	
}
void OTG1411main()
{
	OTG1411Reset();
	// if we get here turn on led
	LedOn(LED1);
	// GPS is now in default state, I2C Master NMEA
	// We need to force it into I2C slave
}