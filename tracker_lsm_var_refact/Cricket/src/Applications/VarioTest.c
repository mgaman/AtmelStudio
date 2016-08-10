// Test project for MS5611-01BA03 sensor

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
#include "SpiFlash.h"


//#define F_CPU 2000000UL
#define F_CPU 32000000UL


#define LSM_COUNT 10
#define BAR_COUNT 100


/*! CPU speed 2MHz, BAUDRATE 100kHz and Baudrate Register Settings */
//#define CPU_SPEED  2000000
//#define BAUDRATE	100000

#define CPU_SPEED  32000000
#define BAUDRATE	 400000
#define TWI_BAUDSETTING TWI_BAUD(CPU_SPEED, BAUDRATE)

int ax=0; int ay=0; int az=0; int te=0; int gx=0; int gy=0; int gz=0; int mx=0; int my=0; int mz=0;	

TWI_Master_t twiMaster;    /*!< TWI master module. */

volatile uint16_t bar_check_count = 0;
volatile uint16_t lsm_check_count = 0;

unsigned long gpsRowsRecorded = 0;
unsigned long lsmRowsRecorded = 0;
unsigned long varRowsRecorded = 0;

bool write_one_row_to_flash = false;

bool request_to_delete_flash = false;
bool record_to_flash = false;

bool memoryFull = false;

bool page_written_to_flash = false;
//bool bar_written_to_flash = false;

char lsm_ax[6];
char lsm_ay[6];
char lsm_az[6];

char lsm_gx[6];
char lsm_gy[6];
char lsm_gz[6];

char lsm_mx[6];
char lsm_my[6];
char lsm_mz[6];

int read_count = 0; 

volatile char     flash_page_buf[256];
volatile uint16_t flash_page_index = 0;

volatile unsigned long flashOffset = 0;
volatile unsigned long pages_counter_for_offset_save = 0;
volatile unsigned long pagesWrittenToFlash = 0;

volatile uint32_t time_stamp = 0;
volatile uint32_t glob_timestamp = 0;

bool time_to_read_lsm = false;
bool time_to_read_bar = false;

unsigned int C[8];            // calibration coefficients

enum eTrackerMode {TENTERSLEEP, TSLEEPING, TENTERWAKE, TAWAKE};
volatile enum eTrackerMode TrackerMode;

volatile enum sleepmgr_mode sleepmode;

uint8_t button_press_counter = 0; 


#define BLINK_DELAY_MS 10


#define CLKSYS_RTC_ClockSource_Disable() ( CLK.RTCCTRL &= ~CLK_RTCEN_bm )  // disables routing of clock signals to the Real-Time Counter (RTC).
#define CLKSYS_RTC_ClockSource_Enable()  ( CLK.RTCCTRL |= CLK_RTCEN_bm )


//**************************
void v_flash_init_handling()
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
		
  //readOffsetFromEeprom();
	readOffsetFromFlash();
}


//****************************
static void HardwareInit(void)
{
	int len;
	const int BUF_SIZE = 10;
	char buf[BUF_SIZE];
	
	ioport_set_pin_high(POWER_LATCH);
	
	ONBOARD_LED_2_ON;
	ONBOARD_LED_3_ON;    // Yellow
	//_delay_ms(100);
	ONBOARD_LED_2_OFF;
	ONBOARD_LED_3_OFF;	
}


//****************************
static void SoftwareInit(void)
{
	uint8_t tmp;
	uint8_t high_byte;
	uint8_t low_byte;	
	
	GpioPinClear();
	GpioPollInit(GPS_STATUS_PIN, PIN_UP);
	GpioPollInit(MODEM_STATUS_PIN, PIN_CHANGED);		
	GpioPollInit(ACTIVATION_BUTTON, PIN_CHANGED);	
       
	//uart_init();

	/* Initialize PORTE for output and PORTD for inverted input. */
	PORTE.DIRSET    = 0xFF;
	PORTCFG.MPCMASK = 0xFF;
	
	cli();
	sysclk_enable_peripheral_clock(&TWIE);	
	TWI_MasterInit(&twiMaster,	&TWIE,	TWI_MASTER_INTLVL_LO_gc, TWI_BAUDSETTING);    /* Initialize TWI master. */
	PMIC.CTRL |= PMIC_LOLVLEN_bm;                                                     /* Enable LO interrupt level. */
	sei();		

	uint8_t resp =  new_lsm_init_check();	
	//if (resp != 0)
	//{
		//while(1)
		//{
			// blink both leds
		//}
	//}
	
	init_bar();
	
	SPI_Init();
	uint8_t w25_stat = spi_flash_check();

	if (w25_stat != 0)
	{
		return(w25_stat);
	}	
    v_flash_init_handling();
	
	TrackerMode = TENTERSLEEP;
	sleepmode = SLEEPMGR_PSAVE;
}



//*************
int VarioTest()
{
	HardwareInit();
    SoftwareInit();
	
	time_to_read_lsm = false;
	time_to_read_bar = false;

    lsm_check_count = LSM_COUNT;
	bar_check_count = BAR_COUNT;	
	
	read_count = 0;
	
	record_to_flash = false;   
	
	bool tracker_active = false;	
	
	button_press_counter = 0; 
	
    while(1)
    {		
	   if (button_press_counter == 0)
	   {
		   sleepmgr_init();
		   sleepmgr_lock_mode(sleepmode);
		   sleepmgr_enter_sleep();
	   }	   
   
	   if (GpioPoll(ACTIVATION_BUTTON))
	   {
		   if (gpio_pin_is_low(ACTIVATION_BUTTON))
		   {
		   }
		   else
		   {
			   button_press_counter++;
			   
			   switch (button_press_counter)
			   {
				   case 1:
				      tracker_active  = true;
				      record_to_flash = false;	
  
					  CLKSYS_RTC_ClockSource_Disable();
				   break;
			   
			       case 2:
			          tracker_active  = true;
			          record_to_flash = true;
					  ONBOARD_LED_3_ON;
			       break;
				   
				   case 3:
				   	  tracker_active  = false;
				   	  record_to_flash = false;
					  button_press_counter = 0;
					  ONBOARD_LED_3_OFF;
					  
					  CLKSYS_RTC_ClockSource_Enable();					  

                      lsm_check_count = LSM_COUNT;
                      bar_check_count = BAR_COUNT;						  
				   break;
			   }
		   }
	   }
	   
	   if (tracker_active)
	   {
		   TrackerMode = TENTERWAKE;
	   }
	   else
	   {
		   TrackerMode = TENTERSLEEP;
	   }
	   
	   switch (TrackerMode)
	   {
		   case TENTERSLEEP:
		      TrackerMode = TSLEEPING;
		      sleepmode = SLEEPMGR_PSAVE;
		   break;
		   
		   case TENTERWAKE:
		      TrackerMode = TAWAKE;
		      sleepmode = SLEEPMGR_IDLE;
		   break;
		   
		   case TSLEEPING:
		   case TAWAKE:
		   break;
	   }

	   if (true /*TrackerMode == TAWAKE*/)
	   {
		  	if (request_for_flash_data)
		      SendFlashDataToHost();
		  	        
		  	if (request_to_delete_flash)
		  	{
			  	request_to_delete_flash = false;
			  	deleteOffsetAndFlash();
				  
			  //readOffsetFromEeprom();
				readOffsetFromFlash();
		  	}			  
		  
		   cpu_irq_disable();
		   glob_timestamp = time_stamp;
		   
		   if (bar_check_count == 0)
		   {
			   time_to_read_bar = true;
			   bar_check_count  = BAR_COUNT;
		   }
		   
		   if (lsm_check_count == 0)
		   {
			   time_to_read_lsm = true;
			   lsm_check_count  = LSM_COUNT;
		   }
		   cpu_irq_enable();
		   
		   
		   if (time_to_read_lsm)
		   {
			   time_to_read_lsm = false;
			   
			   new_get_lsm_data();
			   
			   if (time_to_read_bar)
			   {
				   time_to_read_bar = false;
				   read_bar_data();
			   }
		   }
	   }
    }
}


