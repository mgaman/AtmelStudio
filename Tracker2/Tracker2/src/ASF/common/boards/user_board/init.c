/**
 * \file
 *
 * \brief User board initialization template
 *
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	ioport_init();
	// gps power up stuff
	ioport_configure_pin(GPS_ON_OFF_PIN, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(GPS_WAKEUP_PIN, IOPORT_DIR_INPUT);
	// leds
	ioport_configure_pin(ONBOARD_LED_3,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(ONBOARD_LED_2,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	// button, set up an interrupt for this button
	ioport_configure_pin(ACTIVATION_BUTTON, IOPORT_DIR_INPUT);
	// setup interrupt mask for PORTA_3 = start/stop recording
	PORTA.INT0MASK |= 1<<ACTIVATION_PIN;
	PORTA.INTCTRL |= PORT_INT0LVL0_bm << PORT_INT0LVL_gp;
	ioport_set_pin_sense_mode(ACTIVATION_BUTTON, IOPORT_SENSE_BOTHEDGES);
	// I2C aka TWI input until needed
	ioport_configure_pin(TWI_DATA, IOPORT_DIR_INPUT);
	ioport_configure_pin(TWI_CLK, IOPORT_DIR_INPUT);
	// power latch high
	ioport_configure_pin(POWER_LATCH, IOPORT_DIR_OUTPUT  | IOPORT_INIT_HIGH);	
	// SPI pins
	ioport_configure_port_pin(&PORTC, PIN4_bm, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);   //slave select
	ioport_configure_port_pin(&PORTC, PIN5_bm, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);   //mosi
	ioport_configure_port_pin(&PORTC, PIN6_bm, IOPORT_DIR_INPUT);                       //miso
	ioport_configure_port_pin(&PORTC, PIN7_bm, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);   //clock
	// I2C aka TWI input until needed
	ioport_configure_pin(TWI_DATA, IOPORT_DIR_INPUT);
	ioport_configure_pin(TWI_CLK, IOPORT_DIR_INPUT);
	//
	// set up a 1millsec timer
	//
	tc_enable(TIMER_1MS);
	tc_set_overflow_interrupt_callback(TIMER_1MS, tc1ms_ovf_interrupt_callback);
	tc_set_resolution(TIMER_1MS, TIMER_1MS_RESOLUTION);
	tc_write_period(TIMER_1MS, TIMER_1MS_PERIOD);
	tc_set_overflow_interrupt_level(TIMER_1MS, TC_INT_LVL_LO);
}
