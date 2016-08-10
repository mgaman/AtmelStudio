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
	// initialize pins
//	ioport_init();
	// gps power up stuff
	ioport_configure_pin(GPS_ON_OFF_PIN, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(GPS_WAKEUP_PIN, IOPORT_DIR_INPUT);
	// leds
	ioport_configure_pin(ONBOARD_LED_3,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(ONBOARD_LED_2,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);	
	// button
	ioport_configure_pin(ACTIVATION_BUTTON, IOPORT_DIR_INPUT);
	// power latch high
	ioport_configure_pin(POWER_LATCH, IOPORT_DIR_OUTPUT  | IOPORT_INIT_HIGH);
	// I2C aka TWI
	ioport_configure_pin(TWI_DATA, IOPORT_DIR_INPUT);  // starting out as slave
    ioport_configure_pin(TWI_CLK, IOPORT_DIR_INPUT);
#if 0
	// I2C aka TWI
	ioport_configure_pin(TWI_DATA, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(TWI_CLK, IOPORT_DIR_OUTPUT  | IOPORT_INIT_LOW);
	// all unused pins input
	ioport_configure_pin( UNUSED2,IOPORT_DIR_INPUT);
	//ioport_configure_pin( UNUSED4,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED7,IOPORT_DIR_INPUT);

	ioport_configure_pin( UNUSED8,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED9,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED10,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED11,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED12,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED13,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED14,IOPORT_DIR_INPUT);

	ioport_configure_pin( UNUSED15,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED16,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED31,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED32,IOPORT_DIR_INPUT);

	ioport_configure_pin( UNUSED18,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED19,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED20,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED21,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED22,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED23,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED24,IOPORT_DIR_INPUT);

	ioport_configure_pin( UNUSED25,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED26,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED27,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED28,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED29,IOPORT_DIR_INPUT);
	ioport_configure_pin( UNUSED30,IOPORT_DIR_INPUT);
#endif
}
