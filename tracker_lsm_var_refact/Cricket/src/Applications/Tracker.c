/*
 * Tracker.c
 *
 * Created: 5/5/2014 8:22:00 AM
 *  Author: User
 */ 

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
#include "I2CGps.h"
#include "I2CBmp.h"
#include "UartModem.h"
#include "I2CLSM.h"
#include "I2CMS5611.h"
#include "UartBluetooth.h"
#include "SpiFlash.h"
#include "uart.h"
#include "avr_compiler.h"
#include "twi_master_driver.h"
#include "I2CVario.h"


TWI_Master_t twiMaster;    /*!< TWI master module. */

TWI_t * trcTwi = &TWIE; 
		
char fromGps[100];
char measuredUtcTime[10];
char measuredLatitude[9];
char measuredLongitude[10];
char measuredAltitude[8];
char measuredSatelites[2];

char lsm_ax[6];
char lsm_ay[6];
char lsm_az[6];

char lsm_gx[5];
char lsm_gy[5];
char lsm_gz[5];

char lsm_mx[5];
char lsm_my[5];
char lsm_mz[5];

char measuredLatitude[9];
char measuredLongitude[10];
char measuredAltitude[8];
char measuredSatelites[2]; 


uint8_t mpu_init_count = 0;

char modemBuffer[100];
char bluetoothBuffer[100];

uint8_t modemBufferIndex;
uint8_t bluetoothBufferIndex;

double baselinePress  = 0;
double measuredBmpAlt = 0;

uint8_t info_to_send_to_host = 0;

uint8_t components_check();

bool yellow_led_On = false;

extern bool record_to_flash; 
extern write_one_row_to_flash; 
extern bool request_to_delete_flash;

extern volatile uint32_t time_stamp;

uint32_t global_raw_temp = 0;
uint32_t global_raw_pres = 0;

uint16_t save_to_eprom_maxval = 0;

extern bool memoryFull;

bool fix_led_On = false;

char flash_page_buf[256];
extern uint16_t flash_page_index;

extern unsigned long flashOffset;
extern unsigned long pages_counter_for_offset_save;

//unsigned long gpsRowsRecorded;
//unsigned long lsmRowsRecorded;
//unsigned long varRowsRecorded;

extern unsigned long pagesWrittenToFlash;

//unsigned long D1;                        // ADC value of the pressure conversion
//unsigned long D2;                        // ADC value of the temperature conversion
//unsigned int C[8];                       // calibration coefficients
//double P;                                // compensated pressure value
//double T;                                // compensated temperature value
//double dT;                               // difference between actual and measured temperature
//double OFF;                              // offset at actual temperature
//double SENS;                             // sensitivity at actual temperature
//int i;
//unsigned char n_crc;                     // crc value of the prom

char message[48];

uint8_t current_sensor_id = 99;

#define GPS_COUNT 1000
#define BAR_COUNT 300


	
	
//**********************
void yellow_led_toggle()
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
void flash_init_handling()
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
		
	readOffsetFromEeprom();
}
	
	
		
//************************
static void HardwareInit()
{
	int len;
	const int BUF_SIZE = 10;
	char buf[BUF_SIZE];
	
	ONBOARD_LED_2_ON;
	ONBOARD_LED_3_ON;    // Yellow
	delay_s(2);
	ONBOARD_LED_2_OFF;
	ONBOARD_LED_3_OFF;	
}


//************************
static void SoftwareInit()
{
	uint8_t tmp;
	uint8_t high_byte;
	uint8_t low_byte;	
	
	record_to_flash = false; 
		
	GpioPinClear();
	GpioPollInit(GPS_STATUS_PIN, PIN_UP);
	GpioPollInit(MODEM_STATUS_PIN, PIN_CHANGED);	
	
    SPI_Init();	
	I2C_Init(trcTwi);
	
    uint8_t checkRes = components_check();
	if (checkRes != 0)
	{
		while(true)
		{
			ONBOARD_LED_3_ON;
		}
	}	
	
    flash_init_handling();
}



//************************
uint8_t components_check()
{	
	bool inits_delay = true;
	uint8_t retval = 0;	
	
	uint8_t lsm_stat = lsm_init_check();
	if (lsm_stat != 0)
	{
		return(lsm_stat);
	}		
	
	//uint8_t ms5611_stat = ms5611_init_check();
	//if (lsm_stat != 0)
	//{
		//return(ms5611_stat);
	//}
	
	uint8_t w25_stat = spi_flash_check();
	if (w25_stat != 0)
	{
		return(w25_stat);
	}
	
	uint8_t gpsactive = gps_activation();
	if (gpsactive != 0)
	{
		return(gpsactive);
	}
	
	//uint8_t modemact = modem_activation();
	//if (modemact != 0)
	//{
		//return(modemact);
	//}
	
    //modem_init_test();
	
	//uint8_t bmpini = bmp_init();	
	
	return(0);
}


////***********************
//void check_for_gps_data()
//{
	//cpu_irq_disable();                                // not atomic operation => we have to disable interrupts when working with shared data
	//if (gps_check_count == 0)
	//{
		//get_gps_data();
		//gps_check_count = GPS_COUNT;
	//}
	//cpu_irq_enable();
//}


////**************************
//void check_for_ms5611_data()
//{
	//cpu_irq_disable();
	//if (ms5611_check_count == 0)
	//{
		//get_ms5611_data();
		//ms5611_check_count = BAR_COUNT;
	//}
	//cpu_irq_enable();
//}


//****************
void TrackerTest()
{ 
	char str[11]; 
	
	HardwareInit();	
	SoftwareInit();	
	
	clean_flash_buff();	
	
  //gprs_first_test();
  //bluetooth_first_test();
	
	info_to_send_to_host = 2;
	
    while (true)
	{		
	        if (request_for_flash_data)
			   SendFlashDataToHost();		   
		   
			if (request_to_delete_flash)
			{
			   request_to_delete_flash = false;
			   deleteOffsetAndFlash();
			   readOffsetFromEeprom();	
			}				

			//if (info_to_send_to_host == 0)
			//{
				//save_to_eprom_maxval = 50;
				//get_ms5611_data();
				//delay_ms(100);
			//}
			//else
			if (info_to_send_to_host == 1)
			{
				save_to_eprom_maxval = 5000;
				get_lsm_data();
			}
			else		
     		if (info_to_send_to_host == 2)
    		{
        		save_to_eprom_maxval = 50;			
				get_gps_data();
			    _delay_ms(1000); 
	   		}
			//else
   			//if (info_to_send_to_host == 3)
			//{
				//save_to_eprom_maxval = 5000;		  
				//get_lsm_data();
				////check_for_gps_data();
			 //// check_for_ms5611_data();
			//}			
		
		yellow_led_toggle();
	}
}


