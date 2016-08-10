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
#include "DataLogger.h"

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
#if 0
extern bool request_for_flash_data;
extern bool request_to_delete_flash;
extern bool sendConnectAck;
extern bool LSMCalibRcvd;

extern uint8_t info_to_send_to_host;

extern char modemBuffer[];
extern uint8_t modemBufferIndex;

extern char bluetoothBuffer[];
extern uint8_t bluetoothBufferIndex;

const char salutare[5] = "HELLO";
const char bt_send[10] = "AT+SEND=7\r";

extern bool record_to_flash; 
extern bool write_one_row_to_flash; 
extern bool request_to_delete_flash;
extern bool VarioCalibRcvd;
extern bool SendFlashStats;
extern bool StartRecordingRequested,StopRecordingRequested;
extern bool StartDownloadRequested,StopDownloadRequested;
extern unsigned long gpsRowsRecorded;
extern unsigned long lsmRowsRecorded;
extern unsigned long pagesWrittenToFlash;

extern unsigned long pages_counter_for_offset_save;
#else
extern bool MonitorReady;
extern uint8_t MonitorChar;
#endif

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
  //  ioport_configure_pin(TEST_TWI_DATA, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
 //   ioport_configure_pin(TEST_TWI_CLK, IOPORT_DIR_OUTPUT  | IOPORT_INIT_LOW);		
  
	ioport_configure_pin(GPS_ON_OFF_PIN, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);	
	ioport_configure_pin(GPS_WAKEUP_PIN, IOPORT_DIR_INPUT);

	ioport_configure_pin(ONBOARD_LED_2,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(ONBOARD_LED_3,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	
	// I2C aka TWI input until needed
	ioport_configure_pin(TEST_TWI_DATA, IOPORT_DIR_INPUT);
	ioport_configure_pin(TEST_TWI_CLK, IOPORT_DIR_INPUT);

	ioport_configure_pin(POWER_LATCH, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);

 	ioport_configure_pin(MODEM_STATUS_PIN, IOPORT_DIR_INPUT);
 // ioport_configure_pin(MODEM_STATUS_PIN, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);	   // FOR PIN TEST  
  
    ioport_configure_pin(MODEM_ON_OFF_PIN, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);	
	
	ioport_configure_pin(SPI_SS,    IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(SPI_MOSI,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
  //ioport_configure_pin(SPI_MISO,  IOPORT_DIR_INPUT);                                // For test
	ioport_configure_pin(SPI_MISO,  IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);             // For test
	ioport_configure_pin(SPI_CLK,   IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	
	ioport_configure_pin(ACTIVATION_BUTTON, IOPORT_DIR_INPUT);
  //-------------------------------------------------------------------------	
		
//	ioport_configure_pin(ONBOARD_LED,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);	// low is on
//	ioport_configure_pin(HIPOWER_LED,IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);	// high is on

//	ioport_configure_pin(HIPOWER_LED_1, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);	
	//ioport_configure_pin(HIPOWER_LED_2, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);	
	
//	ioport_configure_pin(HIPOWER_LED_3, IOPORT_DIR_INPUT | IOPORT_PULL_UP);	
	
    ioport_configure_pin(PUSH_BUTTON_2, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);	  
	//ioport_configure_pin(PUSH_BUTTON_3, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);	   
	
	ioport_configure_pin(USART_MODEM_RX_PIN, IOPORT_DIR_INPUT);
	ioport_configure_pin(USART_MODEM_TX_PIN, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	
    ioport_configure_pin(USART_PC_RX_PIN,  IOPORT_DIR_INPUT);                  
  	ioport_configure_pin(USART_PC_TX_PIN,  IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);	
	
	// UNUSED
//	ioport_configure_pin(UNUSED_PIN_0,IOPORT_DIR_INPUT);
  //ioport_configure_pin(UNUSED_PIN_1,IOPORT_DIR_INPUT);
	//ioport_configure_pin(UNUSED_PIN_2,IOPORT_DIR_INPUT);
//	ioport_configure_pin(UNUSED_PIN_3,IOPORT_DIR_INPUT);
	//ioport_configure_pin(UNUSED_PIN_4,IOPORT_DIR_INPUT);
//	ioport_configure_pin(UNUSED_PIN_5,IOPORT_DIR_INPUT);
	//ioport_configure_pin(UNUSED_PIN_6,IOPORT_DIR_INPUT);
//	ioport_configure_pin(UNUSED_PIN_7,IOPORT_DIR_INPUT);
//	ioport_configure_pin(UNUSED_PIN_8,IOPORT_DIR_INPUT);
//	ioport_configure_pin(UNUSED_PIN_9,IOPORT_DIR_INPUT);
//	ioport_configure_pin(UNUSED_PIN_10,IOPORT_DIR_INPUT);
//	ioport_configure_pin(UNUSED_PIN_11,IOPORT_DIR_INPUT);	
	
	
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
	
	
 // RTC set at 1, 64 or 1024 ticks per second
	rtc_init();
	rtc_set_callback(rtc_ovf_interrupt_callback);	
	rtc_set_alarm_relative(0);	
	
	
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
#if 1
	MonitorChar = received_byte;
	MonitorReady = true;
#else	
	// this ISR cannot send any data as it may interfere with data being send from sensors
	// simply set a flag and let the main loop send data
	switch(received_byte)
	{
		case ConnectToTracker:
			sendConnectAck = true; // cant reply now as we might be in the middle of another message
			break;
		case GetFlashData:
		//	UartWrapSendByte(USART_SERIAL_PC, AckGetFlashData);
		//	request_for_flash_data = true;
			StartDownloadRequested = true;
			break;
		case StopSendingFlashData:
		//	UartWrapSendByte(USART_SERIAL_PC, AckStopFlashData);
			request_for_flash_data = false;
			StopDownloadRequested = true;
			break;
		case VarRadioButton:
		//	UartWrapSendByte(USART_SERIAL_PC, VarButtonAck);   // send MS5611 data
			info_to_send_to_host = 0;
			break;
		case LSMRadioButton:
		//	UartWrapSendByte(USART_SERIAL_PC, LSMButtonAck);   // send LSM data
			info_to_send_to_host = 1;
			break;
		case GPSRadioButton:
		//	UartWrapSendByte(USART_SERIAL_PC, GPSButtonAck);   // send GPS data
			info_to_send_to_host = 2;
			break;
		case CleanFlash:
		//	UartWrapSendByte(USART_SERIAL_PC, CleanFlashAck); // send this AFTER chip erase
			request_to_delete_flash = true;
			break;
		case RecordToFlash:
			if (memoryFull)
				; //UartWrapSendByte(USART_SERIAL_PC, MemoryFullAck);
			else
			{
			//	UartWrapSendByte(USART_SERIAL_PC, RecordToFlashAck);
				StartRecordingRequested = true;
				record_to_flash = true;
			}
			break;
		case StopRecordingToFlash:
//			UartWrapSendByte(USART_SERIAL_PC, StopRecordingFlashAck);
			record_to_flash = false;
			StopRecordingRequested = true;
			break;
		case  WriteOneRowToFlash:
		//	UartWrapSendByte(USART_SERIAL_PC, WriteOneRowAck);
			write_one_row_to_flash = true;
			break;
		case  AllSensorRadioButton:
		//	UartWrapSendByte(USART_SERIAL_PC, AllSensorsAck);         // send all sensors data
			info_to_send_to_host = 3;
			break;
		case GetFlashOffset:
		//	sendFlashOffsetAndLastRecordsToHost();
			SendFlashStats  = true;
			break;
		case VarioCalibReceived:
			VarioCalibRcvd = true;
			break;
		case LSM96D0CalibReceived:
			LSMCalibRcvd = true;
			break;
		default:
			break;
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
//------------------------------------------------------------------------------


//------------------------------  Modem/BLE ------------------------------------
ISR(USARTD0_RXC_vect)
{
	unsigned char received_byte;
	received_byte = usart_getchar(USART_SERIAL_MODEM);
	fifo_push_uint8(&modem_com_rx_fifo, received_byte);
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