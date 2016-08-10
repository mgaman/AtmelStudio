/*
 * tester.c
 *
 * Created: 12/19/2013 8:27:34 AM
 *  Author: pc
 */ 
#include <asf.h>
#include "Applications.h"
#include "RFHandler.h"
#include "Utilities.h"

uint16_t sequence;
extern uint16_t msgCounter;

volatile enum sleepmgr_mode sleepmode;
int state;
void RTC_init(void);
void RTC_init()
{
	// Enable RTC with ULP as clock source.
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_RTC);
	CLK.RTCCTRL = CLK_RTCSRC_ULP_gc | CLK_RTCEN_bm;

	// Configure RTC for wakeup at 1.5 second period (at 256x prescaling).
	RTC.PER = RTC_TICKS_SEC;
	RTC.INTCTRL = RTC_OVFINTLVL_LO_gc;
	
	// Wait until RTC is ready before continuing.
	do { } while (RTC.STATUS & RTC_SYNCBUSY_bm);

	// Enable low level interrupts for wakeups to occur.
	//PMIC.CTRL = PMIC_LOLVLEN_bm;
}

static void HardwareInit(void)
{
	GpioPollInit(PUSH_BUTTON_2,PIN_DOWN);
	RTC_init();
}

static void SoftwareInit(void)
{
	msgCounter = 0;
	sequence = 0;
	sleepmode = SLEEPMGR_PSAVE;
	ONBOARD_LED_OFF;
	HIPOWER_LED_OFF;
	state = 0;
}

void RTC_Reset(uint8_t Period)
{
	RTC.PER = Period;
	RTC.CNT = 0;
	RTC.CTRL = CONFIG_RTC_PRESCALER; //RTC_PRESCALER_DIV256_gc
	do { } while (RTC.STATUS & RTC_SYNCBUSY_bm);
}

void RTC_Stop(void)
{
	// Stop the RTC
	RTC.CTRL = RTC_PRESCALER_OFF_gc;
	
}

void sleep_init(enum sleepmgr_mode mode)
{
	
	// Initialize the sleep manager, lock initial mode.
	sleepmgr_init();
	sleepmgr_lock_mode(mode);
}

void sleep_wakeupOnRTC(uint8_t period)
{
	RTC_Reset(period);
	sleepmgr_init();
	sleepmgr_lock_mode(SLEEPMGR_PSAVE);
	sleepmgr_enter_sleep();
	RTC_Stop();
}

void tester()
{
	//int j;
	
	HardwareInit();
	SoftwareInit();
	
	//while (true)
	//{
        //#if 1
		    //sleep_wakeupOnRTC(RTC_TICKS_SEC*2);
        //#else
			//sleepmgr_init();
			//sleepmgr_lock_mode(SLEEPMGR_PSAVE);
			//sleepmgr_enter_sleep();
        //#endif
	//}
	
	while (true)
	{
	   RFSendPacket(0, 1, 1, 0xffff,NULL,0);
	}
}