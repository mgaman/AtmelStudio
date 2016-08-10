/*
 * CFile1.c
 *
 * Created: 1/9/2014 8:44:50 AM
 *  Author: User
 */ 

#include <asf.h>
#include "Utilities.h"
#include "RFHandler.h"
#include "pcdatahandler.h"


static void HardwareInit(void)
{
	ONBOARD_LED_ON;	// not seen outside of the box
	HIPOWER_LED_OFF;	// master battery
}


static void SoftwareInit(void)
{
	GpioPollInit(AC_BUZZER, PIN_DOWN);
	sleepmgr_lock_mode(SLEEPMGR_ACTIVE);
}


void GpioIntTester()
{
	//int j;
	
	HardwareInit();
	SoftwareInit();
	
	sleepmgr_init();
	sleepmgr_enter_sleep();
	
	while (true)
	{
		
	}
}