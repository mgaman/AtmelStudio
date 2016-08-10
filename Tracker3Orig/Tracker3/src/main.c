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
#include "ORG1411.h"
#include "WS6318.h"
#include "LedManager.h"
#include "MS5611.h"
#include "LSM9DS0.h"
#include "W25Q128FV.h"
#include "Tracker.h"
#include "ReliableUART.h"
#include "Telemetry.h"

//#define DO_GPS
//#define DO_GSM
//#define DO_ACCEL
//#define DO_BARO
#define DO_FLASH
#define DO_PC
//
//  fifo for commands from PC via CDC, GSM to Atmel, GPS to Atmel
//
fifo_desc_t PC_rx_fifo;
uint8_t		PC_rx_buffer[_MAX_RX_FIFO_BUFFER_LENGTH];
fifo_desc_t GSM_rx_fifo;
uint8_t		GSM_rx_buffer[_MAX_RX_FIFO_BUFFER_LENGTH];
fifo_desc_t GPS_rx_fifo;
uint8_t		GPS_rx_buffer[_MAX_RX_FIFO_BUFFER_LENGTH];

/*
  Global data
*/
volatile bool rtcdone;
volatile uint32_t time_stamp;
void myWaitMs(unsigned int);
// service routines for USB CDC
volatile bool cdc_up = false;
volatile unsigned long NextPageAvailable;
volatile uint32_t pagesWrittenToFlash;
uint32_t pages_counter_for_offset_save;
uint8_t    flash_page_buf[PAGE_SIZE];
bool request_for_flash_data;
bool record_to_flash;
bool ConnectedToPC = false;
uint8_t myID = 55;
volatile uint16_t flash_page_index;
uint32_t lsmRowsRecorded,varRowsRecorded,GPSRecorded;
bool memoryFull = false;

void rtccallback(uint32_t t)
{
	rtcdone = true;
}

bool CDC_enable(void)
{
	cdc_up = true;
	return true;
}

void CDC_disable(void)
{
	cdc_up = false;
}

void CDC_rx_notify(uint8_t port)
{
	int nb = udi_cdc_get_nb_received_data();
	uint8_t c;
	while (nb--)
	{
		c = udi_cdc_getc();
		fifo_push_uint8(&PC_rx_fifo,c);
	}
}

void tc1ms_ovf_interrupt_callback(void)
{
	if  (time_stamp == UINT32_MAX)
	time_stamp = 0;
	else
	time_stamp++;
}

#ifdef DO_BARO
double temp,press;
uint32_t D1,D2;
#endif

int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).
	bool InitOK = true;
	pmic_init();
	irq_initialize_vectors();
	sysclk_init();

	// set up various fifos before board_init
	
	fifo_init(&PC_rx_fifo,PC_rx_buffer,_MAX_RX_FIFO_BUFFER_LENGTH);
	fifo_init(&GSM_rx_fifo,GSM_rx_buffer,_MAX_RX_FIFO_BUFFER_LENGTH);
	fifo_init(&GPS_rx_fifo,GPS_rx_buffer,_MAX_RX_FIFO_BUFFER_LENGTH);
	
	// rtc source is 1K crystal, divider 1 i.e. interrupt every 1msec
	rtc_init();
	rtc_set_callback(rtccallback);
	rtcdone = false;

	board_init();
	// set  up CDC channel. All input stuff goes to an circular buffer which is emptied by the interested party
	// Insert application code here, after the board has been initialized.
	LedInit();
	cpu_irq_enable();
	udc_start();
	// INITs using rtc delays must be after interrupts enabled
#ifdef DO_GPS
	InitOK &= ORG1410_Init();
#endif
#ifdef DO_GSM
	InitOK &= WS6318Init();
#endif
#ifdef DO_ACCEL
	InitOK &= LSM9DS0GInit();
	InitOK &= LSM9DS0XMInit();
	InitOK &= LSM9DS0Config();
#endif
#ifdef DO_BARO
	InitOK &= MS561101BA03Reset();
#endif
#ifdef DO_FLASH
	InitOK &= W25Q128Init();
#endif
	while (InitOK)
	{
		uint8_t command;
#ifdef DO_GPS
		ORG1411_Test();
#endif
#ifdef DO_GSM
		WS6318_Test();
#endif
#ifdef DO_BARO
		D1 = MS561101BA03ReadADC(CMD_ADC_256+CMD_ADC_D1) ;
		D2 = MS561101BA03ReadADC(CMD_ADC_256+CMD_ADC_D2) ;
		MS561101BA03FormatAltimeter(D1,D2,&temp,&press);
#endif
#ifdef DO_ACCEL
		if (LSM6DS0GReady())
			LSM9DS0readAll();
#endif
#ifdef DO_PC
		if (fifo_pull_uint8(&PC_rx_fifo,&command) == FIFO_OK)
			switch (command)
			{
				case ConnectToTracker:
					SerialWrapSendByte(AckFromTracker);
					//					LSM9DS0Reset();
					//					MS561101BA03Reset();
					TelemetryReset();
					SerialWrapSendByte(myID);
					ConnectedToPC = true;
					break;
				case CleanFlash:
					W25Q128EraseAllFlash();
					//				LSM9DS0Reset();
					//			MS561101BA03Reset();
					TelemetryReset();
					break;				
				case GetFlashData:
					request_for_flash_data = true;
					SerialWrapSendByte(AckGetFlashData);
					SendFlashDataToHost();
					break;
				case StopSendingFlashData:
					request_for_flash_data = false;
					SerialWrapSendByte(AckStopFlashData);
					break;
				case GetFlashOffset:
					sendFlashOffsetAndLastRecordsToHost();
					break;
				case RecordToFlash:
					if (flashIsFull())
						SerialWrapSendByte( MemoryFullAck);
					else
					{
						record_to_flash = true;
						LedOn(LED2);
						SerialWrapSendByte(RecordToFlashAck);
					}
					break;
				case StopRecordingToFlash:
					record_to_flash = false;
					LedOff(LED2);
					SerialWrapSendByte(StopRecordingFlashAck);
					break;
			}

#endif
	}	
}
