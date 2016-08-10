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
#include <string.h>
#include "LedManager.h"

//#include "TrackerTWI.h"
void ORG1411main(void);
char *hw = "Hello World\r\n";
volatile bool rtcdone;
void rtccallback(rtc_callback_t t)
{
	rtcdone = true;
}
volatile bool cdc_up = false;

bool my_callback_cdc_enable(void)
{
	cdc_up = true;
	return true;
}

void my_callback_cdc_disable(void)
{
	cdc_up = false;
}

uint32_t crate;
int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).
	pmic_init();
	irq_initialize_vectors();
	sysclk_init();
	board_init();
	crate = sysclk_get_cpu_hz();
	// rtc source is 1K crystal, divider 1 i.e. interrupt every 1msec
	rtc_init();
	rtc_set_callback(rtccallback);
	cpu_irq_enable();
	rtcdone = false;
	LedInit();
	LedOn(LED1);
	LedToggle(LED1);
	LedOn(LED2);
	LedToggle(LED2);
	// serial port
	udc_start();
	ORG1411main();		// go to my code
	while(true){} 
}
