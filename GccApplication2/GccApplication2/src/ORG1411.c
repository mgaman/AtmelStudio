/*
 * OTG1411.c
 *
 * Created: 04-Feb-15 11:26:44 PM
 *  Author: Saba
 */ 
#include <asf.h>
#include "ORG1411.h"
#include "TWIHandler.h"

extern volatile bool rtcdone;
#define MAX_NMEA_SIZE 200
char NMEABuffer[MAX_NMEA_SIZE];
int NMEAindex;
enum eNMEAParse {WAIT_FOR_DOLLAR,WAIT_FOR_CR,WAIT_FOR_LF};
enum eNMEAParse NMEAState;
bool NMEAReady;

#define XMx
void On_Off_Pulse()
{
	// see Hornet ORG14xx datasheet 7.2.2
	#ifdef XM
	ioport_set_pin_level(GPS_ON_OFF_PIN,LOW);
	#else
	ioport_set_pin_level(GPS_ON_OFF_PIN,HIGH);
	#endif
	rtcdone = false;
	rtc_set_alarm_relative(90);
	while (!rtcdone){}
	#ifdef XM
	ioport_set_pin_level(GPS_ON_OFF_PIN,HIGH);
	#else
	ioport_set_pin_level(GPS_ON_OFF_PIN,LOW);
	#endif
	rtcdone = false;
	rtc_set_alarm_relative(1000);
	while (!rtcdone){}
}

bool ORG1411Reset()
{
	int retries = 20;	// allow 20 attempts to reset it
	// if not hibernating, force it to hibernate
	if (ioport_get_pin_level(GPS_WAKEUP_PIN) == HIGH)
	{
		while (ioport_get_pin_level(GPS_WAKEUP_PIN) == HIGH)
		{
			On_Off_Pulse();
			if (retries-- == 0)
			return false;	
		}
	}
	// now get out of hibernate
	while (ioport_get_pin_level(GPS_WAKEUP_PIN) == LOW)
	{
		On_Off_Pulse();
		if (retries-- == 0)
		return false;
	}
	NMEAState = WAIT_FOR_DOLLAR;
	NMEAReady = false;
	return true;
}

void NMEAParser(uint8_t c)
{
	switch (NMEAState)
	{
		case WAIT_FOR_DOLLAR:
			if (c == '$')
			{
				NMEAindex = 0;
				NMEABuffer[NMEAindex++] = c;
				NMEAState = WAIT_FOR_CR;
			}
			break;
		case WAIT_FOR_CR:
			NMEABuffer[NMEAindex++] = c;
			if (c == 0x0d)
				NMEAState = WAIT_FOR_LF;
			break;
		case WAIT_FOR_LF:
			if (c == 0x0a)
			{
				NMEAReady = true;
			}
			NMEAState = WAIT_FOR_DOLLAR;
			break;
	}
}
/*
		Called whenever there is data from the slave
		At this point in time we are in NMEA mode
		Look for a <LF> character signifying end of line, that tells us
		we have a chance to go in and change to master mode
*/
void ORG1411SlaveHandler(void)
{
	int i;
	for (i=0;i<slave.bytesSent;i++)
		NMEAParser(slave.receivedData[i]);
}