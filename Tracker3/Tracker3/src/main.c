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
#include "HDLC.h"
#include "Telemetry.h"

#define DO_GPS
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
//uint32_t pages_counter_for_offset_save;
uint8_t    flash_page_buf[PAGE_SIZE];
bool request_for_flash_data;
bool record_to_flash;
bool ConnectedToPC = false;
uint8_t myID = 55;
volatile uint16_t flash_page_index;
uint32_t lsmRowsRecorded,MS5611RowsRecorded,GPSRecorded;
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

uint32_t CDCfifoerrors;
void CDC_rx_notify(uint8_t port)
{
	int nb = udi_cdc_get_nb_received_data();
	uint8_t c;
	while (nb--)
	{
		c = udi_cdc_getc();
#ifdef DO_PC
		if (fifo_push_uint8(&PC_rx_fifo,c) != FIFO_OK)
			CDCfifoerrors++;
#else
		usart_putchar(USART_GPS,c);	// just playing with gps
#endif
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
struct sMS5611_Data MS5611Data;
uint32_t MS5611SampleTime;
bool MS5611CalibWritten = false;
extern struct sMS5611_Calibration MS5611CalibData;
#endif
#ifdef DO_ACCEL
extern struct sLSM9DS0_Data LSM9DS0Data;
extern struct sLSM9SD0_Calib LSM9DS0Calib;
bool LSM9DS0CalibWritten = false;
#endif
static uint8_t *cookedBuffer;  // when local to CDCWrapSendByte data not transmitted - why ??
uint8_t messageBody[20]; // make big enough
void CDCWrapSendByte(uint8_t b)
{
	uint8_t rawBuffer[2];
	uint32_t cookedLength;
	int i;
	rawBuffer[0] = b;	
	cookedBuffer = HDLCStuff(rawBuffer,1,&cookedLength);
#if 1
	for (i=0;i<cookedLength;i++)
		udi_cdc_putc(*cookedBuffer++);
#else
	udi_cdc_write_buf(cookedBuffer,cookedLength);
#endif
}
int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).
	bool InitOK = true;
	uint8_t msgLength;
	
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
	HDLCInit();
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
	MS5611SampleTime = time_stamp + 20;
#endif
#ifdef DO_FLASH
	if (W25Q128Init())
		NextPageAvailable = GetNextAvailablePage();
	else
		InitOK = false;
#endif
	while (InitOK)
	{
		uint8_t pcByte;
#ifdef DO_GPS
		ORG1411_Test();
#endif
#ifdef DO_GSM
		WS6318_Test();
#endif
#ifdef DO_BARO
		MS561101BA03Ready = (time_stamp > MS5611SampleTime);
		if ((record_to_flash == true) && (MS561101BA03Ready == true))
		{
			if (MS5611CalibWritten == false)
			{
				TelemetryWriteToFlash(MS561101BA03_CALIB,&MS5611CalibData);
				MS5611CalibWritten = true;
			}
			MS5611Data.D1 = MS561101BA03ReadADC(CMD_ADC_256+CMD_ADC_D1) ;
			MS5611Data.D2 = MS561101BA03ReadADC(CMD_ADC_256+CMD_ADC_D2) ;
			MS561101BA03Ready = false;
			TelemetryWriteToFlash(MS561101BA03,&MS5611Data);
			MS5611RowsRecorded++;
			MS5611SampleTime = time_stamp + 20;	// assume 50Hz
			//MS561101BA03FormatAltimeter(D1,D2,&temp,&press);
		}
#endif
#ifdef DO_ACCEL
		if (record_to_flash == true && LSM6DS0GReady())
		{
			if (!LSM9DS0CalibWritten)
			{
				TelemetryWriteToFlash(LSM9DS0_CALIB,&LSM9DS0Calib);
				LSM9DS0CalibWritten = true;
			}
			if (LSM9DS0readAll())
			{
				TelemetryWriteToFlash(LSM9DS0_ALL,&LSM9DS0Data.header.ID);
				lsmRowsRecorded++;
			}
		}
#endif
#ifdef DO_PC
		if (fifo_pull_uint8(&PC_rx_fifo,&pcByte) == FIFO_OK)
		{
			if (HDLCParse(pcByte))
			{
				msgLength = HDLCUnStuff(messageBody);
				switch (messageBody[0])
				{
					case ConnectToTracker:
						CDCWrapSendByte(AckFromTracker);
						//					LSM9DS0Reset();
						//	MS561101BA03Reset();
						TelemetryReset();
						CDCWrapSendByte(myID);
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
						CDCWrapSendByte(AckGetFlashData);
						SendFlashDataToHost();
						break;
					case StopSendingFlashData:
						request_for_flash_data = false;
						CDCWrapSendByte(AckStopFlashData);
						break;
					case GetFlashOffset:
						sendFlashOffsetAndLastRecordsToHost();
						break;
					case RecordToFlash:
						if (flashIsFull())
						CDCWrapSendByte( MemoryFullAck);
						else
						{
							record_to_flash = true;
							//LedOn(LED2);
							CDCWrapSendByte(RecordToFlashAck);
						}
						break;
					case StopRecordingToFlash:
						record_to_flash = false;
						//LedOff(LED2);
						CDCWrapSendByte(StopRecordingFlashAck);
						break;					
				}
			}
		}
#endif
	}	
}
