/**
 * \file
 *
 * \brief User board initialization template
 *
 */
#include <asf.h>
#include <board.h>
#include <conf_board.h>
#include "Utilities.h"
#include "spi_interface.h"
#include "RFHandler.h"

fifo_desc_t pc_com_rx_fifo;
uint8_t		pc_com_rx_buffer[_MAX_RX_FIFO_BUFFER_LENGTH];

fifo_desc_t pc_com_tx_fifo;
uint8_t		pc_com_tx_buffer[_MAX_TX_FIFO_BUFFER_LENGTH];

fifo_desc_t pc_com_tx_debug_fifo;
uint8_t		pc_com_tx_debug_buffer[_MAX_DEBUG_FIFO_BUFFER_LENGTH];

uint8_t j;

extern uint8 buttonReleased;
extern uint8 buttonIntFlag;

void board_init(void)
{
	// USART options.
	static usart_rs232_options_t USART_SERIAL_OPTIONS = 
	{
		.baudrate = USART_SERIAL_PC_BAUD,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	// initialize pins
	ioport_init();
	
	// output
	
	
  //--------------------------------- TRACKER -------------------------------	
	ioport_configure_pin(GPS_ON_OFF_PIN, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);	
	ioport_configure_pin(ONBOARD_LED_2,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(ONBOARD_LED_3,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	
	ioport_configure_pin(GPS_STATUS_PIN, IOPORT_DIR_INPUT);
	
	ioport_configure_pin(SPI_SS,    IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(SPI_MOSI,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(SPI_MISO,  IOPORT_DIR_INPUT);
	ioport_configure_pin(SPI_CLK,   IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
  //-------------------------------------------------------------------------	
		
	ioport_configure_pin(ONBOARD_LED,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);	// low is on
	ioport_configure_pin(HIPOWER_LED,IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);	// high is on

	ioport_configure_pin(HIPOWER_LED_1, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);	
	//ioport_configure_pin(HIPOWER_LED_2, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	
	
	ioport_configure_pin(HIPOWER_LED_3, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	
	ioport_configure_pin(SPI_MOSI_PIN	, IOPORT_DIR_OUTPUT);
	ioport_configure_pin(SPI_SCK_PIN	, IOPORT_DIR_OUTPUT);
	ioport_configure_pin(SI4332_NSEL_CHANNEL0	, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);	// deselect channel 0
	ioport_configure_pin(SI4332_NSEL_CHANNEL1	, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);	// deselect channel 1
	ioport_configure_pin(USART_TX_PIN	, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	
	// input
	//ioport_configure_pin(PUSH_BUTTON_0, IOPORT_DIR_INPUT | IOPORT_PULL_UP);	 // high is off	
	//ioport_configure_pin(PUSH_BUTTON_1, IOPORT_DIR_INPUT | IOPORT_PULL_UP);  // high is off
	
		
	ioport_configure_pin(PUSH_BUTTON_2, IOPORT_DIR_INPUT | IOPORT_PULL_UP);	// high is off
	ioport_configure_pin(PUSH_BUTTON_3, IOPORT_DIR_INPUT | IOPORT_PULL_UP);	// high is off
//	ioport_configure_pin(SI4332_NIRQ_CHANNEL0	, IOPORT_DIR_INPUT/* | IOPORT_PULL_UP*/);
	ioport_configure_pin(SI4332_NIRQ_CHANNEL1	, IOPORT_DIR_INPUT/* | IOPORT_PULL_UP*/);
	ioport_configure_pin(SPI_MISO_PIN	, IOPORT_DIR_INPUT );
	ioport_configure_pin(USART_RX_PIN	, IOPORT_DIR_INPUT);
	
	// UNUSED
	ioport_configure_pin(UNUSED_PIN_0,IOPORT_DIR_INPUT);
	ioport_configure_pin(UNUSED_PIN_1,IOPORT_DIR_INPUT);
	ioport_configure_pin(UNUSED_PIN_2,IOPORT_DIR_INPUT);
	ioport_configure_pin(UNUSED_PIN_3,IOPORT_DIR_INPUT);
	ioport_configure_pin(UNUSED_PIN_4,IOPORT_DIR_INPUT);
	ioport_configure_pin(UNUSED_PIN_5,IOPORT_DIR_INPUT);
	ioport_configure_pin(UNUSED_PIN_6,IOPORT_DIR_INPUT);
//	ioport_configure_pin(UNUSED_PIN_7,IOPORT_DIR_INPUT);
//	ioport_configure_pin(UNUSED_PIN_8,IOPORT_DIR_INPUT);
//	ioport_configure_pin(UNUSED_PIN_9,IOPORT_DIR_INPUT);
//	ioport_configure_pin(UNUSED_PIN_10,IOPORT_DIR_INPUT);
	ioport_configure_pin(UNUSED_PIN_11,IOPORT_DIR_INPUT);
	
#ifdef YUVALIT_MASTER
	ioport_configure_pin(SPARE_GPIO_0,IOPORT_DIR_OUTPUT | IOPORT_PULL_DOWN);
	ioport_configure_pin(SPARE_GPIO_1,IOPORT_DIR_OUTPUT | IOPORT_PULL_DOWN);
#else
	//ioport_configure_pin(SPARE_GPIO_0,IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	//ioport_configure_pin(SPARE_GPIO_1,IOPORT_DIR_INPUT | IOPORT_PULL_UP);
#endif


//#ifdef GAHLILIT_SLAVE
   //// ioport_configure_pin(GAHLILIT_LEARN_BUTTON, IOPORT_DIR_INPUT | IOPORT_PULL_UP);	// high is off
   //ioport_configure_pin(GAHLILIT_LEARN_BUTTON, IOPORT_DIR_INPUT);
	//
	//ioport_configure_pin(GAHLILIT_IR_LED_1, IOPORT_DIR_OUTPUT | IOPORT_PULL_DOWN);	
	//ioport_configure_pin(GAHLILIT_IR_LED_2, IOPORT_DIR_OUTPUT | IOPORT_PULL_DOWN);	
//#endif

	// SPI
	//spi_init();
	
	#ifndef GAHLILIT_SLAVE
	// usart & fifo
	// Initialize usart driver in RS232 mode with interrupts
	
	   usart_init_rs232(USART_SERIAL_PC, &USART_SERIAL_OPTIONS);
   	   usart_set_rx_interrupt_level(USART_SERIAL_PC,USART_INT_LVL_HI);
	   usart_set_tx_interrupt_level(USART_SERIAL_PC,USART_INT_LVL_HI);
	   fifo_init(&pc_com_rx_fifo,pc_com_rx_buffer,sizeof(pc_com_rx_buffer));
	   fifo_init(&pc_com_tx_fifo,pc_com_tx_buffer,sizeof(pc_com_tx_buffer));
	   fifo_init(&pc_com_tx_debug_fifo,pc_com_tx_debug_buffer,sizeof(pc_com_tx_debug_buffer));
	#endif
	
	// create a 1ms timer, only RF handler uses it
	
	tc_enable(TIMER_1MS);
	tc_set_overflow_interrupt_callback(TIMER_1MS,tc1ms_ovf_interrupt_callback);
	tc_set_resolution(TIMER_1MS, TIMER_1MS_RESOLUTION);
	tc_write_period(TIMER_1MS, TIMER_1MS_PERIOD);
	tc_set_overflow_interrupt_level(TIMER_1MS, TC_INT_LVL_LO);
	
	// RTC set at 1 or 64 ticks per second
	rtc_init();
	rtc_set_callback(rtc_ovf_interrupt_callback);	// this way we can retain all of the old event code
	rtc_set_alarm_relative(0);
	
	#ifdef GAHLILIT_SLAVE
		// ioport_configure_pin(GAHLILIT_LEARN_BUTTON, IOPORT_DIR_INPUT | IOPORT_PULL_UP);	// high is off
		ioport_configure_pin(GAHLILIT_LEARN_BUTTON, IOPORT_DIR_INPUT);
	
		ioport_configure_pin(GAHLILIT_IR_LED_1, IOPORT_DIR_OUTPUT | IOPORT_PULL_DOWN);
		ioport_configure_pin(GAHLILIT_IR_LED_2, IOPORT_DIR_OUTPUT | IOPORT_PULL_DOWN);
	#endif
	

	#ifdef GAHLILIT_SLAVE
 	 // setup interrupt mask for PORTC_1 = GAHLILIT_LEARN_BUTTON
		PORTC.INT0MASK |= 1<<2;
		PORTC.INTCTRL |= PORT_INT0LVL0_bm << PORT_INT0LVL_gp;		
	  //ioport_set_pin_sense_mode(GAHLILIT_LEARN_BUTTON, IOPORT_SENSE_FALLING);
		ioport_set_pin_sense_mode(GAHLILIT_LEARN_BUTTON, IOPORT_SENSE_BOTHEDGES);
	#endif
}


ISR(USARTD1_RXC_vect)
{
	unsigned char received_byte;
	received_byte = usart_getchar(USART_SERIAL_PC);
	fifo_push_uint8(&pc_com_rx_fifo,received_byte);
	
	#ifdef GAHLILIT_MASTER
		if (received_byte == 254)
		{
			usart_putchar(USART_SERIAL_PC, 253);	
		}
	#endif
}


ISR(USARTD1_TXC_vect)
{
	uint8_t transmit_byte = 0xff;
	if (fifo_is_empty(&pc_com_tx_fifo) == false)
	{
		fifo_pull_uint8(&pc_com_tx_fifo,&transmit_byte);
		usart_putchar(USART_SERIAL_PC, transmit_byte);
	}
	else
	if(fifo_is_empty(&pc_com_tx_debug_fifo) == false)
	{
		fifo_pull_uint8(&pc_com_tx_debug_fifo,&transmit_byte);
		usart_putchar(USART_SERIAL_PC, transmit_byte);
	}
}


ISR(PORTC_INT0_vect)
{
	 if (ioport_pin_is_low(GAHLILIT_LEARN_BUTTON))
	 {
		 buttonIntFlag = true;
		 buttonReleased = false;		 
	 }
	 else
	 {
		 buttonIntFlag = true;
		 buttonReleased = true;
	 }
}