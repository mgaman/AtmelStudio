/*
 * ORG1411.c
 *
 * Created: 09/03/2015 18:43:42
 *  Author: henryd
 */ 

#include <asf.h>
#include "ORG1411.h"
extern volatile bool rtcdone;
static void On_Off()
{
	// see Hornet ORG14xx datasheet 7.2.2
	ioport_set_pin_level(GPS_ON_OFF_PIN,HIGH);
	rtcdone = false;
	rtc_set_alarm_relative(60);
	while (!rtcdone);
	ioport_set_pin_level(GPS_ON_OFF_PIN,LOW);
	rtcdone = false;
	rtc_set_alarm_relative(700);
	while (!rtcdone);
}

void ORG1411Reset()
{
	bool hibernate;
	// bring gps to known state see Hornet ORG14xx datasheet Chapter 9.1
	// on_off pulse alternately switches gps on/off but as we don't know its original state
	// force it into power off then power on
	hibernate = ioport_get_pin_level(GPS_WAKEUP_PIN) == HIGH;
	if (!hibernate)
	while (!hibernate)
	{
		On_Off();
		hibernate = ioport_get_pin_level(GPS_WAKEUP_PIN) == HIGH;
	}
	while (hibernate)
	{
		On_Off();
		hibernate = ioport_get_pin_level(GPS_WAKEUP_PIN) == HIGH;
	}	
}
