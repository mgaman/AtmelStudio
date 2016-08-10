/*
 * Tracker.c
 *
 * Created: 5/5/2014 8:22:00 AM
 *  Author: User
 */ 
#define OLD_TYPE
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <asf.h>
#include "Utilities.h"
#include "twi_common.h"
#include "EepromHandler.h"
#include "I2CUtils.h"
#include "conf_board.h"
#include "SpiFlash.h"
#include "I2CMpu.h"
//#include "I2CGps.h"
#include "I2CBmp.h"
#include "UartModem.h"
#include "i2c_lsm.h"
#include "i2c_bar.h"
#include "UartBluetooth.h"
#include "SpiFlash.h"
#include "uart.h"
#include "avr_compiler.h"
#include "twi_master_driver.h"
#include "I2CVario.h"
#include "DataLogger.h"
#include "ReliableUART.h"
#include "Telemetry.h"
#include "ORG1411.h"
#include "sirf.h"

#ifdef OLD_TWI
TWI_Master_t twiMaster;    /*!< TWI master module. */
TWI_t * trcTwi = &TWIE; 
#define TWI_BAUDSETTING TWI_BAUD(CPU_SPEED, BAUDRATE)
#else
#define TWI_SPEED 400000
#define TWI_ORG1411   TWIE
#define TWI_PORT_GPS	PORTE
#define GPS_AS_SLAVE_ADDR	 0x60
#define GPS_AS_MASTER_ADDR	 0x62
char *SIRF115200 = "$PSRF100,0,115200,8,1,0*04\r\n";  // MUST have \r\n
TWI_Slave_t slave;
twi_options_t m_options;
twi_package_t packet;

struct sMID166 *pMID166;
struct sMID136 *pMID136;
struct sSirfHeader *pSirfHeader;
struct sMID41 *pMID41;

static bool mid41active;
extern volatile bool rtcdone;

// fifo to store GPS SIRF data
fifo_desc_t i2c_slave_rx_fifo;
uint8_t		 i2c_slave_rx_buffer[128];

#endif

#define CPU_SPEED  32000000
#define BAUDRATE	 400000
#ifdef TRACKER_TEST
volatile uint16_t bar_check_count = 0;
volatile uint16_t lsm_check_count;
int read_count = 0;
bool time_to_read_bar = false;
bool record_to_flash = false;
//bool request_to_delete_flash = false;
bool write_one_row_to_flash = false;
bool memoryFull = 0;
//bool sendConnectAck = false;
//bool SendFlashStats = false;
//bool StartRecordingRequested = false;
//bool StopRecordingRequested = false;
//bool StartDownloadRequested = false;
//bool StopDownloadRequested = false;
uint32_t volatile time_stamp;
volatile unsigned long NextPageAvailable;
enum eTrackerMode {TENTERSLEEP, TSLEEPING, TENTERWAKE, TAWAKE};
volatile enum eTrackerMode TrackerMode;
volatile enum sleepmgr_mode sleepmode;
uint8_t button_press_counter = 0;
uint8_t myID = 55;
unsigned long gpsRowsRecorded;
unsigned long lsmRowsRecorded;
bool MonitorReady = false;
uint8_t MonitorChar;
#else
//extern bool sendConnectAck;
//extern bool SendFlashStats;
extern uint8_t myID ;
extern unsigned long NextPageAvailable;
extern write_one_row_to_flash;
extern volatile uint32_t time_stamp;
extern bool record_to_flash;
//extern bool request_to_delete_flash;
extern bool memoryFull;
//extern bool StartRecordingRequested;
//extern bool StopRecordingRequested;
//extern bool StartDownloadRequested;
//extern bool StopDownloadRequested;
#endif
		
uint8_t mpu_init_count = 0;
uint8_t info_to_send_to_host = 0;

bool components_check(void);
bool yellow_led_On = false;

bool fix_led_On = false;
bool page_written_to_flash;
char flash_page_buf[PAGE_SIZE];
uint16_t flash_page_index;
uint32_t TelemetrySequence;
unsigned long pages_counter_for_offset_save;
volatile unsigned long pagesWrittenToFlash;

void MonitorHandler(uint8_t c);

//**********************
void yellow_led_on()
{	
	ONBOARD_LED_2_ON;
    yellow_led_On = true;
	
}
void yellow_led_off()
{
	ONBOARD_LED_2_OFF;
	yellow_led_On = false;	
}
void yellow_led_toggle(void)
{
	if (yellow_led_On)
	{
		ONBOARD_LED_2_ON;
		yellow_led_On = false;
	}
	else
	{
    	ONBOARD_LED_2_OFF;
		yellow_led_On = true;
	}
}	
	

//************************
void flash_init_handling(void)
{	
	#ifdef DELETE_OFFSET_AND_FLASH
	    deleteOffsetAndFlash();
	#endif
		
	#ifdef FLASH_READ_WRITE_TEST
	    flashReadWriteTest();
	#endif
		
	#ifdef READ_SAMPLE_DATA_FROM_FLASH
	    readSampleFlashData();
	#endif
		
#ifndef FLASH_ERASE_CHIP
	readOffsetFromEeprom();
#else
	NextPageAvailable = GetCurrentPageFromEEPROM();
#endif
}

int ferrcount;

void slaveProcess(void)
{
	// copy data to fifo
	if (mid41active)
	{
		//	if (cdc_up)
		//		udi_cdc_putc(slave.receivedData[slave.bytesReceived]);
		if (fifo_push_uint8(&i2c_slave_rx_fifo,(uint32_t)slave.receivedData[slave.bytesReceived]) != FIFO_OK)
			ferrcount++;
	}
	slave.bytesReceived = 0;
}

ISR(TWIE_TWIS_vect) {
	TWI_SlaveInterruptHandler(&slave);
}
	
void TWIMasterMode(TWI_t *twi)
{
	cpu_irq_disable();
	twi_slave_disable(twi);
	twi_master_init(twi, &m_options);
	twi_master_enable(twi);
	cpu_irq_enable();
}

void TWISlaveMode(TWI_t *twi,void (*handler)(void),uint8_t selfAddress)
{
	cpu_irq_disable();
	twi_master_disable(twi);
	TWI_SlaveInitializeDriver(&slave, twi, handler);
	TWI_SlaveInitializeModule(&slave, selfAddress, TWI_SLAVE_INTLVL_MED_gc);
	memset(slave.receivedData,0,TWIS_RECEIVE_BUFFER_SIZE);
	twi_slave_enable(twi);
	cpu_irq_enable();
}

void TWIInit()
{
	m_options.speed = TWI_SPEED;
	m_options.chip = 0;
	m_options.speed_reg = TWI_BAUD(sysclk_get_cpu_hz(), TWI_SPEED);
	TWI_PORT_GPS.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc;
	TWI_PORT_GPS.PIN1CTRL = PORT_OPC_WIREDANDPULL_gc;
	sysclk_enable_peripheral_clock(&TWI_ORG1411);  // same clock for master & slave
}
		
//************************
static void HardwareInit(void)
{
	ioport_set_pin_high(POWER_LATCH);
	
	ONBOARD_LED_2_ON;
	ONBOARD_LED_3_ON;    // Yellow
	//_delay_ms(100);
	ONBOARD_LED_2_OFF;
	ONBOARD_LED_3_OFF;
	/* Initialize PORTE for output and PORTD for inverted input. */
	PORTE.DIRSET    = 0xFF;
	PORTCFG.MPCMASK = 0xFF;
	
	cpu_irq_disable();
#ifdef OLD_TWI
	sysclk_enable_peripheral_clock(&TWIE);
	TWI_MasterInit(&twiMaster,	&TWIE,	TWI_MASTER_INTLVL_LO_gc, TWI_BAUDSETTING);    /* Initialize TWI master. */
#endif
	PMIC.CTRL |= PMIC_LOLVLEN_bm;                                                     /* Enable LO interrupt level. */
	cpu_irq_enable();
}


//************************
static bool SoftwareInit(void)
{
	uint8_t tmp;
	uint8_t high_byte;
	uint8_t low_byte;	
	
	record_to_flash = false; 
	mid41active = false;
		
//	GpioPinClear();
//	GpioPollInit(GPS_WAKEUP_PIN, PIN_UP);
//	GpioPollInit(MODEM_STATUS_PIN, PIN_CHANGED);	
	
    SPI_Init();	
	// GPS is now in default state, I2C Master NMEA
	// set up fifo to receive slave data
	fifo_init(&i2c_slave_rx_fifo,i2c_slave_rx_buffer,sizeof(i2c_slave_rx_buffer));
	bool checkRes = components_check();
	if (checkRes)
		flash_init_handling();
	memset(flash_page_buf,0xff,PAGE_SIZE);
	return checkRes;
}

//************************
bool components_check(void)
{	
	bool inits_delay = true;
#ifdef TRACKER_GPS
	ORG1411Reset();
	ONBOARD_LED_2_ON;
	TWIInit();	// just set up hardware
	// set up Atmel as a slave device
	TWISlaveMode(&TWI_ORG1411,slaveProcess,GPS_AS_MASTER_ADDR);
	// change GPS to SIRF mode
	packet.chip = GPS_AS_SLAVE_ADDR;
	packet.addr_length = 0;
	packet.buffer = SIRF115200;
	packet.length = strlen(SIRF115200);
	packet.no_wait = false;
	TWIMasterMode(&TWI_ORG1411);
	twi_master_write(&TWI_ORG1411,&packet);
	TWISlaveMode(&TWI_ORG1411,slaveProcess,GPS_AS_MASTER_ADDR);
	delay_s(1);
	// turn off all messages (this disables poll nav parameters, poll software version
	pMID166 = MID166(ENABLEDISABLEALL,0,0);
	packet.chip = GPS_AS_SLAVE_ADDR;
	packet.addr_length = 0;
	packet.buffer = pMID166;
	packet.length = sizeof(struct sMID166);
	packet.no_wait = false;
	TWIMasterMode(&TWI_ORG1411);
	twi_master_write(&TWI_ORG1411,&packet);
	TWISlaveMode(&TWI_ORG1411,slaveProcess,GPS_AS_MASTER_ADDR);
	delay_s(1);
	// set 5Mhz mode
	pMID136 = MID136();
	packet.chip = GPS_AS_SLAVE_ADDR;
	packet.addr_length = 0;
	packet.buffer = pMID136;
	packet.length = sizeof(struct sMID136);
	packet.no_wait = false;
	TWIMasterMode(&TWI_ORG1411);
	twi_master_write(&TWI_ORG1411,&packet);
	TWISlaveMode(&TWI_ORG1411,slaveProcess,GPS_AS_MASTER_ADDR);
	delay_s(1);
	// create a poll 41 message
	pMID166 = MID166(POLLONE,41,0);
	// initialize the parser
	SirfParseInit();
	// set polling at 5 hz
	rtcdone = false;
	mid41active = true;
	rtc_set_alarm_relative(200);
#endif

#ifdef TRACKER_SENSORS
#error cc
	bool varStat = init_bar();
	bool lsm_stat = new_lsm_init_check();
	bool w25_stat = spi_flash_check();
	return(lsm_stat && varStat && w25_stat);
#else
	return(spi_flash_check());
#endif
}


//****************
void TrackerTest(void)
{ 
	uint32_t VarioNextGet;	
	uint8_t c;
	HardwareInit();	
	if (SoftwareInit())
	{
		VarioNextGet = time_stamp + (TIMER_RATE/MS5611BA03_SAMPLE_RATE);
		mid41active = true;
		
		while (true)
		{
			if (MonitorReady)
			{
				MonitorReady = false;
				MonitorHandler(MonitorChar);
			}
			cpu_irq_disable();
			bool gotac = fifo_pull_uint8(&i2c_slave_rx_fifo,&c) == FIFO_OK;
			cpu_irq_enable();
			if (gotac)
			{
				if (SirfBlockReady(c))
				{
					ONBOARD_LED_2_TOGGLE;
					pSirfHeader = SirfData();
					switch (pSirfHeader->MID)
					{
						case 41:
							pMID41 = (struct sMID41 *)pSirfHeader;
							if (pMID41->NavValid == 0)
							{
								//ONBOARD_LED_2_OFF;
								pMID41->ClimbRate = 1;
							}
							break;
						default:
							break;
					}
				}
			}
			if (rtcdone)
			{
				ONBOARD_LED_3_TOGGLE;
				packet.chip = GPS_AS_SLAVE_ADDR;
				packet.addr_length = 0;
				packet.buffer = pMID166;
				packet.length = sizeof(struct sMID166);
				packet.no_wait = false;
				TWIMasterMode(&TWI_ORG1411);
				twi_master_write(&TWI_ORG1411,&packet);
				TWISlaveMode(&TWI_ORG1411,slaveProcess,GPS_AS_MASTER_ADDR);
				rtcdone = false;
				rtc_set_alarm_relative(200);
			}
#ifdef TRACKER_SENSORS
			if (time_stamp >= VarioNextGet)
			{
				VarioNextGet = time_stamp + (TIMER_RATE/MS5611BA03_SAMPLE_RATE);
				MS561101BA03Handler();
			}
			if (LSM9DS0GReady)
			{
				LSM9DS0Handler();
			}
#endif
		}
	}
	else
	{
		ONBOARD_LED_2_OFF;
		while (true){}
	}
}

void MonitorHandler(uint8_t c)
{
	switch(c)
	{
		case ConnectToTracker:
			UartWrapSendByte(USART_SERIAL_PC, AckFromTracker);
			LSM9DS0Reset();
			MS561101BA03Reset();
			TelemetryReset();
			UartWrapSendByte(USART_SERIAL_PC, myID);
			break;
		case GetFlashData:
			request_for_flash_data = true;
			UartWrapSendByte(USART_SERIAL_PC, AckGetFlashData);
			SendFlashDataToHost();
			break;
		case StopSendingFlashData:
			request_for_flash_data = false;
			UartWrapSendByte(USART_SERIAL_PC, AckStopFlashData);
			break;
		case VarRadioButton:
		case LSMRadioButton:
		case GPSRadioButton:
		case AllSensorRadioButton:
			break;
		case CleanFlash:
			eraseAllFlash();
			LSM9DS0Reset();
			MS561101BA03Reset();
			TelemetryReset();
			break;
		case RecordToFlash:
			if (memoryFull)
				UartWrapSendByte(USART_SERIAL_PC, MemoryFullAck);
			else
			{
				record_to_flash = true;
				yellow_led_on();
				UartWrapSendByte(USART_SERIAL_PC, RecordToFlashAck);
			}
			break;
		case StopRecordingToFlash:
			record_to_flash = false;
			yellow_led_off();
			UartWrapSendByte(USART_SERIAL_PC, StopRecordingFlashAck);
			break;
		case  WriteOneRowToFlash:
			break;
		case GetFlashOffset:
			sendFlashOffsetAndLastRecordsToHost();
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
	
}
