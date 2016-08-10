// Test project for MS5611-01BA03 sensor
//
//  DH remove all references to flash

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <asf.h>

#include "Utilities.h"
#include "uart.h"
#include "avr_compiler.h"
#include "twi_master_driver.h"
#include "i2c_lsm.h"
#include "i2c_bar.h"
#include "ReliableUART.h"
#include "DataLogger.h"
#include "telemetry.h"

//#define F_CPU 2000000UL
#define F_CPU 32000000UL

/*! CPU speed 2MHz, BAUDRATE 100kHz and Baudrate Register Settings */
//#define CPU_SPEED  2000000
//#define BAUDRATE	100000

#define CPU_SPEED  32000000
#define BAUDRATE	 400000
#define TWI_BAUDSETTING TWI_BAUD(CPU_SPEED, BAUDRATE)

TWI_Master_t twiMaster;    /*!< TWI master module. */

//volatile uint16_t bar_check_count = 0;
//volatile uint16_t lsm_check_count;


#ifdef VARIO_TEST
int read_count = 0; 
unsigned long lsmRowsRecorded = 0;
bool time_to_read_bar = false;
bool record_to_flash = false;
bool request_to_delete_flash = false;
bool write_one_row_to_flash = false;
bool memoryFull = 0;
bool sendConnectAck = false;
bool SendFlashStats = false;
uint32_t volatile time_stamp;
volatile unsigned long NextPageAvailable;
enum eTrackerMode {TENTERSLEEP, TSLEEPING, TENTERWAKE, TAWAKE};
volatile enum eTrackerMode TrackerMode;
volatile enum sleepmgr_mode sleepmode;
uint8_t button_press_counter = 0; 
uint8_t myID = 55;
bool StartRecordingRequested = false;
bool StopRecordingRequested = false;
bool StartDownloadRequested = false;
bool StopDownloadRequested = false;
#else
extern uint8_t myID;
extern uint32_t volatile time_stamp;
extern bool sendConnectAck;
extern bool record_to_flash;
extern bool StartRecordingRequested;
extern bool StopRecordingRequested;
extern bool StartDownloadRequested;
extern bool StopDownloadRequested;
#endif

#define BLINK_DELAY_MS 10


#define CLKSYS_RTC_ClockSource_Disable() ( CLK.RTCCTRL &= ~CLK_RTCEN_bm )  // disables routing of clock signals to the Real-Time Counter (RTC).
#define CLKSYS_RTC_ClockSource_Enable()  ( CLK.RTCCTRL |= CLK_RTCEN_bm )


//****************************
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
	
	cli();
	sysclk_enable_peripheral_clock(&TWIE);
	TWI_MasterInit(&twiMaster,	&TWIE,	TWI_MASTER_INTLVL_LO_gc, TWI_BAUDSETTING);    /* Initialize TWI master. */
	PMIC.CTRL |= PMIC_LOLVLEN_bm;                                                     /* Enable LO interrupt level. */
	sei();
}


//****************************
static bool SoftwareInit(void)
{
	return (init_bar() & new_lsm_init_check());
}

//*************
int VarioTest()
{
	uint32_t VarioNextGet;
	HardwareInit();
    if (SoftwareInit())
	{
		VarioNextGet = time_stamp + (TIMER_RATE/MS5611BA03_SAMPLE_RATE);
		while(true)
		{		
			if  (sendConnectAck)
			{
				UartWrapSendByte(USART_SERIAL_PC, AckFromTracker);
				UartWrapSendByte(USART_SERIAL_PC, myID);
				LSM9DS0Reset();
				MS561101BA03Reset();
				TelemetryReset();
				sendConnectAck = false;
			}
			if (time_stamp >= VarioNextGet)
			{
   	   			VarioNextGet = time_stamp + (TIMER_RATE/MS5611BA03_SAMPLE_RATE);
				MS561101BA03Handler();
			}
			if (LSM9DS0GReady)
			{
				LSM9DS0Handler();
			}
		}
    }
	else
		while(true);
}
