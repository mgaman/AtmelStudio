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
#include "LedManager.h"

static usart_rs232_options_t USART_GSM_OPTIONS =
{
	.baudrate   = WS6318_BAUDRATE,
	.charlength = USART_CHSIZE_8BIT_gc,
	.paritytype = USART_PMODE_DISABLED_gc,
	.stopbits   = 1
};
	
	
static usart_rs232_options_t USART_GPS_OPTIONS =
{
	.baudrate   = ORG1411_POWERON_BAUDRATE,
	.charlength = USART_CHSIZE_8BIT_gc,
	.paritytype = USART_PMODE_DISABLED_gc,
	.stopbits   = 1
};

static twi_master_options_t TWI_OPTIONS = {
	.chip = 0,
	.speed = TWI_SPEED,
	//.speed_reg = TWI_BAUD(sysclk_get_cpu_hz(), TWI_SPEED)
};

extern fifo_desc_t GSM_rx_fifo;
extern fifo_desc_t GPS_rx_fifo;
extern status_code_t master_status;
extern bool record_to_flash;

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
	// gsm power up stuff
	ioport_configure_pin(GSM_ON_OFF_PIN, IOPORT_DIR_OUTPUT);
	ioport_configure_pin(GSM_RESET_PIN, IOPORT_DIR_OUTPUT);
	ioport_configure_pin(GSM_WAKEUP_PIN, IOPORT_DIR_INPUT);
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
	// SPI pins
	ioport_configure_port_pin(&PORTC, PIN4_bm, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);   //slave select
	ioport_configure_port_pin(&PORTC, PIN5_bm, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);   //mosi
	ioport_configure_port_pin(&PORTC, PIN6_bm, IOPORT_DIR_INPUT);                       //miso
	ioport_configure_port_pin(&PORTC, PIN7_bm, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);   //clock
	//
	// set up a 1millsec timer
	//
	tc_enable(TIMER_1MS);
	tc_set_overflow_interrupt_callback(TIMER_1MS, tc1ms_ovf_interrupt_callback);
	tc_set_resolution(TIMER_1MS, TIMER_1MS_RESOLUTION);
	tc_write_period(TIMER_1MS, TIMER_1MS_PERIOD);
	tc_set_overflow_interrupt_level(TIMER_1MS, TC_INT_LVL_LO);
	//
	// configure usarts
	//
    ioport_configure_pin(USART_GSM_RX_PIN,  IOPORT_DIR_INPUT);
    ioport_configure_pin(USART_GSM_TX_PIN,  IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
    usart_init_rs232(USART_GSM, &USART_GSM_OPTIONS);
    usart_set_rx_interrupt_level(USART_GSM, USART_INT_LVL_HI);
	sysclk_enable_peripheral_clock(USART_GSM);

    ioport_configure_pin(USART_GPS_RX_PIN,  IOPORT_DIR_INPUT);
    ioport_configure_pin(USART_GPS_TX_PIN,  IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
    usart_init_rs232(USART_GPS, &USART_GPS_OPTIONS);
    usart_set_rx_interrupt_level(USART_GPS, USART_INT_LVL_HI);
	sysclk_enable_peripheral_clock(USART_GPS);
	//
	// setup TWI master
	//
	// I2C aka TWI input until needed
	ioport_configure_pin(TWI_DATA, IOPORT_DIR_INPUT);
	ioport_configure_pin(TWI_CLK, IOPORT_DIR_INPUT);
	// Use the internal pullups for SDA and SCL
	TWI_MASTER_PORT.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc;
	TWI_MASTER_PORT.PIN1CTRL = PORT_OPC_WIREDANDPULL_gc;
	sysclk_enable_peripheral_clock(&TWI_MASTER);
	TWI_OPTIONS.speed_reg = TWI_BAUD(sysclk_get_cpu_hz(), TWI_SPEED);
	master_status = twi_master_init(&TWI_MASTER, &TWI_OPTIONS);
	if (master_status == STATUS_OK)
		twi_master_enable(&TWI_MASTER);
}

//
//  D0 GSM push data into GSM_rx_fifo
//
ISR(USARTD0_RXC_vect)
{
	fifo_push_uint8(&GSM_rx_fifo,usart_getchar(USART_GSM));
}

//
//  E0 GSM push data into GPS_rx_fifo
//
ISR(USARTE0_RXC_vect)
{
	fifo_push_uint8(&GPS_rx_fifo,usart_getchar(USART_GPS));
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
