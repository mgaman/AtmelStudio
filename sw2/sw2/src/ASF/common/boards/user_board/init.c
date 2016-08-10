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
	// leds
	ioport_configure_pin(ONBOARD_LED_1,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(ONBOARD_LED_2,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	// button, set up an interrupt for this button
	ioport_configure_pin(ACTIVATION_BUTTON, IOPORT_DIR_INPUT);
	// setup interrupt mask for PORTA_3 = start/stop recording
	PORTA.INT0MASK |= 1<<ACTIVATION_PIN;
	PORTA.INTCTRL |= PORT_INT0LVL0_bm << PORT_INT0LVL_gp;
	ioport_set_pin_sense_mode(ACTIVATION_BUTTON, IOPORT_SENSE_BOTHEDGES);
	// power latch high
	ioport_configure_pin(POWER_LATCH, IOPORT_DIR_OUTPUT  | IOPORT_INIT_HIGH);
}
