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
#include "SpiFlash.h"
#include "EepromHandler.h"


fifo_desc_t pc_com_rx_fifo;
uint8_t		pc_com_rx_buffer[_MAX_RX_FIFO_BUFFER_LENGTH];

fifo_desc_t pc_com_tx_fifo;
uint8_t		pc_com_tx_buffer[_MAX_TX_FIFO_BUFFER_LENGTH];

fifo_desc_t pc_com_tx_debug_fifo;
uint8_t		pc_com_tx_debug_buffer[_MAX_DEBUG_FIFO_BUFFER_LENGTH];



fifo_desc_t pc_d_com_rx_fifo;
uint8_t		pc_d_com_rx_buffer[_MAX_RX_FIFO_BUFFER_LENGTH];

fifo_desc_t pc_d_com_tx_fifo;
uint8_t		pc_d_com_tx_buffer[_MAX_TX_FIFO_BUFFER_LENGTH];

fifo_desc_t pc_d_com_tx_debug_fifo;
uint8_t		pc_d_com_tx_debug_buffer[_MAX_DEBUG_FIFO_BUFFER_LENGTH];



fifo_desc_t modem_com_rx_fifo;
uint8_t		modem_com_rx_buffer[_MAX_RX_FIFO_BUFFER_LENGTH];

fifo_desc_t modem_com_tx_fifo;
uint8_t		modem_com_tx_buffer[_MAX_TX_FIFO_BUFFER_LENGTH];

fifo_desc_t modem_com_tx_debug_fifo;
uint8_t		modem_com_tx_debug_buffer[_MAX_DEBUG_FIFO_BUFFER_LENGTH];

uint8_t j;

extern uint8 buttonReleased;
extern uint8 buttonIntFlag;
extern bool request_for_flash_data;
extern bool request_to_delete_flash;

extern uint8_t info_to_send_to_host;

extern char modemBuffer[100];
extern uint8_t modemBufferIndex;

extern char bluetoothBuffer[100];
extern uint8_t bluetoothBufferIndex;

const char salutare[5] = "HELLO";
const char bt_send[10] = "AT+SEND=7\r";

extern bool record_to_flash; 
extern bool write_one_row_to_flash; 
extern bool request_to_delete_flash;

extern unsigned long gpsRowsRecorded;
extern unsigned long lsmRowsRecorded;

extern unsigned long pagesWrittenToFlash;

extern unsigned long pages_counter_for_offset_save;


//*******************
void board_init(void)
{
	static usart_rs232_options_t USART_PC_SERIAL_OPTIONS =
	{
		.baudrate   = USART_SERIAL_PC_BAUD,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits   = USART_SERIAL_STOP_BIT
	};
	
	
	static usart_rs232_options_t USART_MODEM_SERIAL_OPTIONS =	
	{
		.baudrate   = USART_SERIAL_MODEM_BAUD,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits   = USART_SERIAL_STOP_BIT
	};
	
	
	
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	// initialize pins
	ioport_init();
	
	
  //--------------------------------- TRACKER -------------------------------	
    ioport_configure_pin(TEST_TWI_DATA, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
    ioport_configure_pin(TEST_TWI_CLK, IOPORT_DIR_OUTPUT  | IOPORT_INIT_LOW);		
  
	ioport_configure_pin(GPS_ON_OFF_PIN, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);	
	ioport_configure_pin(ONBOARD_LED_2,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(ONBOARD_LED_3,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	
	ioport_configure_pin(GPS_STATUS_PIN, IOPORT_DIR_INPUT);
	
 	ioport_configure_pin(MODEM_STATUS_PIN, IOPORT_DIR_INPUT);
 // ioport_configure_pin(MODEM_STATUS_PIN, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);	   // FOR PIN TEST  
  
    ioport_configure_pin(MODEM_ON_OFF_PIN, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);	
	
	ioport_configure_pin(SPI_SS,    IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(SPI_MOSI,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
  //ioport_configure_pin(SPI_MISO,  IOPORT_DIR_INPUT);                                // For test
	ioport_configure_pin(SPI_MISO,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);             // For test
	ioport_configure_pin(SPI_CLK,   IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	
	ioport_configure_pin(POWER_LATCH, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(ACTIVATION_BUTTON, IOPORT_DIR_INPUT);
  //-------------------------------------------------------------------------	
		
	ioport_configure_pin(ONBOARD_LED,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);	// low is on
	ioport_configure_pin(HIPOWER_LED,IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);	// high is on

	ioport_configure_pin(HIPOWER_LED_1, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);	
	//ioport_configure_pin(HIPOWER_LED_2, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);	
	
	ioport_configure_pin(HIPOWER_LED_3, IOPORT_DIR_INPUT | IOPORT_PULL_UP);	
	
    ioport_configure_pin(PUSH_BUTTON_2, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);	  
	//ioport_configure_pin(PUSH_BUTTON_3, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);	   
	
	ioport_configure_pin(USART_MODEM_RX_PIN, IOPORT_DIR_INPUT);
	ioport_configure_pin(USART_MODEM_TX_PIN, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	
    ioport_configure_pin(USART_PC_RX_PIN,  IOPORT_DIR_INPUT);                  
  	ioport_configure_pin(USART_PC_TX_PIN,  IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);	
	
	// UNUSED
	ioport_configure_pin(UNUSED_PIN_0,IOPORT_DIR_INPUT);
  //ioport_configure_pin(UNUSED_PIN_1,IOPORT_DIR_INPUT);
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
	
	
    usart_init_rs232(USART_SERIAL_MODEM, &USART_MODEM_SERIAL_OPTIONS);
	usart_set_rx_interrupt_level(USART_SERIAL_MODEM, USART_INT_LVL_HI);
 // usart_set_tx_interrupt_level(USART_SERIAL_MODEM, USART_INT_LVL_HI);
	fifo_init(&modem_com_rx_fifo, modem_com_rx_buffer, sizeof(modem_com_rx_buffer));
	fifo_init(&modem_com_tx_fifo, modem_com_tx_buffer, sizeof(modem_com_tx_buffer));
	fifo_init(&modem_com_tx_debug_fifo, modem_com_tx_debug_buffer, sizeof(modem_com_tx_debug_buffer));	
	
	
	//#ifdef TRACKER_TEST
	  usart_init_rs232(USART_SERIAL_PC, &USART_PC_SERIAL_OPTIONS);
	  
	  sysclk_enable_peripheral_clock(USART_SERIAL_PC);
	  
   	  usart_set_rx_interrupt_level(USART_SERIAL_PC, USART_INT_LVL_HI);
  //  usart_set_tx_interrupt_level(USART_SERIAL_PC,USART_INT_LVL_HI);
	  fifo_init(&pc_com_rx_fifo, pc_com_rx_buffer, sizeof(pc_com_rx_buffer));
	  fifo_init(&pc_com_tx_fifo, pc_com_tx_buffer,sizeof(pc_com_tx_buffer));
	  fifo_init(&pc_com_tx_debug_fifo, pc_com_tx_debug_buffer, sizeof(pc_com_tx_debug_buffer));
	//#endif
	
 // create a 1ms timer, only RF handler uses it	
	tc_enable(TIMER_1MS);
	tc_set_overflow_interrupt_callback(TIMER_1MS, tc1ms_ovf_interrupt_callback);
	tc_set_resolution(TIMER_1MS, TIMER_1MS_RESOLUTION);
	tc_write_period(TIMER_1MS, TIMER_1MS_PERIOD);
	tc_set_overflow_interrupt_level(TIMER_1MS, TC_INT_LVL_LO);	
	
	
 // RTC set at 1 or 64 ticks per second
	rtc_init();
	rtc_set_callback(rtc_ovf_interrupt_callback);	
	rtc_set_alarm_relative(0);	
	//rtc_set_alarm_relative(3);
	
	
	#ifdef TRACKER_TEST
	// setup interrupt mask for PORTA_6 = MODEM_STATUS_PIN
	   PORTA.INT0MASK |= 1<<2;
	   PORTA.INTCTRL |= PORT_INT0LVL0_bm << PORT_INT0LVL_gp;
	   ioport_set_pin_sense_mode(MODEM_STATUS_PIN, IOPORT_SENSE_BOTHEDGES);
	#endif
}


//------------------------------ PC --------------------------------
ISR(USARTD1_RXC_vect)
{
	unsigned char received_byte;
	received_byte = usart_getchar(USART_SERIAL_PC);
	fifo_push_uint8(&pc_d_com_rx_fifo,received_byte);
	
	
	//#ifdef TR_TEST
	//usart_putchar(USART_SERIAL_MODEM, received_byte);
	
	if (received_byte == 254)
	{
		usart_putchar(USART_SERIAL_PC, 253);
	}
	else
	if (received_byte == 0xE0)
	{
		usart_putchar(USART_SERIAL_PC, 0xE1);
		request_for_flash_data = true;
	}
	else
	if (received_byte == 0xE2)
	{
		usart_putchar(USART_SERIAL_PC, 0xE3);
		request_for_flash_data = false;
	}
	else
	if (received_byte == 0xE6)
	{
		usart_putchar(USART_SERIAL_PC, 0xE7);   // send MS5611 data
		info_to_send_to_host = 0;
	}
	else
	if (received_byte == 0xE8)
	{
		usart_putchar(USART_SERIAL_PC, 0xE9);   // send LSM data
		info_to_send_to_host = 1;
	}
	else
	if (received_byte == 0xEA)
	{
		usart_putchar(USART_SERIAL_PC, 0xEB);   // send GPS data
		info_to_send_to_host = 2;
	}
	else
	if (received_byte == 0xEC)
	{
		usart_putchar(USART_SERIAL_PC, 0xED);
		request_to_delete_flash = true;
	}
	else
	if (received_byte == 0xEE)
	{
 		if (memoryFull)
		{
			usart_putchar(USART_SERIAL_PC, 0x80);
		}
		else
		{
			usart_putchar(USART_SERIAL_PC, 0xEF);
			//pagesWrittenToFlash = 0;
			//pages_counter_for_offset_save = 0;
			
			//gpsRowsRecorded = 0;
			//lsmRowsRecorded = 0;
			//varRowsRecorded = 0;
			
			//cleanRecordsInEeprom();
			//record_to_flash = true;
		}
	}
	else
	if (received_byte == 0xF0)
	{
		usart_putchar(USART_SERIAL_PC, 0xF1);
		record_to_flash = false;
		
		saveOffsetToEeprom();
		//saveOffsetToFlash();
		
		//cleanRecordsInEeprom();
		//saveRecordsToEeprom(gpsRowsRecorded, GPS_REC_ADDR);
		//saveRecordsToEeprom(lsmRowsRecorded, LSM_REC_ADDR);
		//saveRecordsToEeprom(varRowsRecorded, VAR_REC_ADDR);
	}
	else
	if (received_byte == 0xF2)
	{
		usart_putchar(USART_SERIAL_PC, 0xF3);
		write_one_row_to_flash = true;
	}
	else
	if (received_byte == 0xF7)
	{
		usart_putchar(USART_SERIAL_PC, 0xF8);         // send all sensors data
		info_to_send_to_host = 3;
	}
	else
	if (received_byte == 0x7E)
	{
		sendFlashOffsetAndLastRecordsToHost();
	}
	//#endif
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
//------------------------------------------------------------------------------


//------------------------------  Modem/BLE ------------------------------------
ISR(USARTD0_RXC_vect)
{
	unsigned char received_byte;
	received_byte = usart_getchar(USART_SERIAL_MODEM);
	fifo_push_uint8(&modem_com_rx_fifo, received_byte);
	
	if (received_byte != 0)
	{
		usart_putchar(USART_SERIAL_PC, received_byte);
		modemBuffer[modemBufferIndex++] = received_byte;
		
		//if (received_byte == 'C')
		//{
		//if (modemBuffer[modemBufferIndex-2] == '+')
		//{
		////while(true)
		////{
		//MODEM_ON_OFF_PIN_TO_HIGH;
		//
		//usart_putchar(USART_SERIAL_MODEM, 'H');
		//usart_putchar(USART_SERIAL_MODEM, 'I');
		//usart_putchar(USART_SERIAL_MODEM, ' ');
		//usart_putchar(USART_SERIAL_MODEM, 'D');
		//usart_putchar(USART_SERIAL_MODEM, 'U');
		//usart_putchar(USART_SERIAL_MODEM, 'D');
		//usart_putchar(USART_SERIAL_MODEM, 'E');
		//usart_putchar(USART_SERIAL_MODEM, ' ');
		//
		//MODEM_ON_OFF_PIN_TO_LOW;
		//
		////
		//////modemBufferIndex = 0;
		//////for (int j = 0; j < sizeof(bt_send); j++)
		//////usart_putchar(USART_SERIAL_MODEM, bt_send[j]);
		////
		////}
		//}
		//}
		
		if ((modemBufferIndex == 100) || (received_byte == 0x0A))
		modemBufferIndex = 0;
		
		if (received_byte == '>')    // for sending the SMS
		{
			modemBufferIndex = 0;
			for (int j = 0; j < sizeof(salutare); j++)
			usart_putchar(USART_SERIAL_MODEM, salutare[j]);
			
			usart_putchar(USART_SERIAL_MODEM, 0x1A);
		}
	}
}


ISR(USARTD0_TXC_vect)
{
	uint8_t transmit_byte = 0xff;
	if (fifo_is_empty(&modem_com_tx_fifo) == false)
	{
		fifo_pull_uint8(&modem_com_tx_fifo, &transmit_byte);
		usart_putchar(USART_SERIAL_MODEM, transmit_byte);
	}
	else
	if (fifo_is_empty(&modem_com_tx_debug_fifo) == false)
	{
		fifo_pull_uint8(&modem_com_tx_debug_fifo, &transmit_byte);
		usart_putchar(USART_SERIAL_MODEM, transmit_byte);
	}
}
//------------------------------------------------------------------


ISR(PORTA_INT0_vect)
{
	 if (ioport_pin_is_high(MODEM_STATUS_PIN))
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