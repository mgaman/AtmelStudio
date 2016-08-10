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
#include "sirf.h"
#include "FlashHandler.h"
#include "ORG1411.h"

volatile bool rtcdone;
extern volatile uint32_t NextPageAvailable;
extern volatile uint32_t time_stamp;
extern bool record_to_flash;

//
//  fifo for commands from PC via CDC
//
fifo_desc_t PC_rx_fifo;
uint8_t		PC_rx_buffer[_MAX_RX_FIFO_BUFFER_LENGTH];

void rtccallback(uint32_t t)
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

bool flashcheck;
int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).
	pmic_init();
	irq_initialize_vectors();
	sysclk_init();
	board_init();
	// rtc source is 1K crystal, divider 1 i.e. interrupt every 1msec
	rtc_init();
	rtc_set_callback(rtccallback);
	LedInit();
	LedOn(LED1);
	delay_s(1);
	LedToggle(LED1);
	LedOn(LED2);
	delay_s(1);
	LedToggle(LED2);
	SirfParseInit();
	// set up fifo to receive PC data
	fifo_init(&PC_rx_fifo,PC_rx_buffer,sizeof(_MAX_RX_FIFO_BUFFER_LENGTH));
	cpu_irq_enable();
	rtcdone = false;
	// initialise flash device on SPI
#if 0
	FlashInit();
	flashcheck = spi_flash_check();
	if (flashcheck)
		NextPageAvailable = GetCurrentPageFromEEPROM();
	ORG1411Reset();
	// if we don't get here, we're stuck
	LedOn(LED1);
	// put ORG1411 into binary mode, ready for polling
#endif
	ORG1411Setup(); 
	// set up cd input to go to a fifo buffer
	udc_start();
//	tracker();
	while(true){}
}

void my_callback_rx_notify(uint8_t port)
{
	fifo_push_uint8(&PC_rx_fifo,udi_cdc_getc());
}

void tc1ms_ovf_interrupt_callback(void)
{
	if  (time_stamp == UINT32_MAX)
		time_stamp = 0;
	else
		time_stamp++;
}

ISR(PORTA_INT0_vect)
{
	if (ioport_pin_is_low(ACTIVATION_BUTTON))
	{
		// toggle record_to_flash
		if (record_to_flash)
		{
			record_to_flash = false;
			LedOff(LED2);
		}
		else
		{
			record_to_flash = true;
			LedOn(LED2);
		}
	}
}
