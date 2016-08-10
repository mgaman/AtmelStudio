/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "LedManager.h"
#include "ORG1411.h"
#include "Sirflib.h"
#include "TWIHandler.h"

volatile bool rtcdone;
void rtcallback(uint32_t t);
void rtcallback(uint32_t t)
{
	rtcdone = true;
}
uint32_t clkrate;

int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).
	irq_initialize_vectors();
	ioport_init();
	//pmic_init();
	board_init();
	sysclk_init();
	rtc_init();
	rtc_set_callback(rtcallback);
	clkrate = sysclk_get_cpu_hz();
	cpu_irq_enable();
	LedInit();
#if 0
	while(true)
	{
		On_Off_Pulse();
		if (ioport_get_value(GPS_WAKEUP_PIN) == HIGH)
			LedOn(LED1);
		else
			LedOff(LED1);
	}
#endif
	// Insert application code here, after the board has been initialized.
	LedOff(LED1);
	LedOff(LED2);
	SirfParseInit();
	// at this point TWI lines are defined as input as we start out as slave
	// If defined as output, the GPS will not reset
	if (!ORG1411Reset())
		LedOn(LED2);  // can't go any further
	else
	{
		LedOn(LED1);
		// set  up TWI hardware
		TWIInit();
		// set up TWI as slave
		TWISlave(&ORG1411SlaveHandler, ORG1411_SLAVE_ADDR);
	}
	while (true){}
}
