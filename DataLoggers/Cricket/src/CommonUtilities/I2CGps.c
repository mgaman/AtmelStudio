/*
 * I2CGps.c
 *
 * Created: 7/1/2014 2:45:58 PM
 *  Author: User
 */ 

#include <string.h>
#include <math.h>
#include <asf.h>

#include "Utilities.h"
#include "I2CUtils.h"
#include "I2CGps.h"
#include "SpiFlash.h"
#include "ReliableUART.h"
#include "Telemetry.h"

static int uartmsglength;

struct sNMEA_Data NMEA_data;

extern TWI_t * trcTwi;
extern uint8_t info_to_send_to_host;

extern bool record_to_flash;

extern bool fix_led_On;

volatile extern char     flash_page_buf[];
volatile extern uint16_t flash_page_index;

volatile extern unsigned long NextPageAvailable;
volatile extern unsigned long gpsRowsRecorded;

extern uint8_t current_sensor_id;


#define GPS_PACK_DATA_ID 0x23


 uint8_t gps_config[65] = {0xA0, 0xA2, 0x00, 0x39, 0xB2, 0x02, 0x00, 0xF9, 0xC5, 0x68, 0x3, 0xFF, 0x00, 0x00, 0x0B, 0xB8, 0x00, 0x01, 0x77, 0xFA, 0x00, 0x01, 0x03, 0xFC, 0x03, 0xFC, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
	                           0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x01, 0xC2, 0x00, 0x00, 0x00, 0x62, 0x00, 0x60, 0x00, 0x00, 0x1, 0xF4, 0x2A, 0x01,  0x0B, 0x35, 0xB0, 0xB3};
							                                                                                                                                   // speed slave   

uint8_t gpgga_mess[6] = {0x24, 0x47, 0x50, 0x47, 0x47, 0x41};    //$GPGGA
uint8_t gprmc_mess[6] = {0x24, 0x47, 0x50, 0x52, 0x4D, 0x43};    //$GPRMC


uint8_t to_sleep_sequence[16] = {0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x31, 0x37, 0x2C, 0x31, 0x36, 0x2A, 0x30, 0x42, 0x0D, 0x0A};                                                        // $PSRF117,16*0B\r\n
uint8_t set_rate[23]          = {0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2C, 0x30, 0x30, 0x2C, 0x36, 0x2C, 0x30, 0x30, 0x2C, 0x30, 0x2A, 0x32, 0x33, 0x0D, 0x0A};              // $PSRF103,00,6,00,0*23\r\n



uint8_t disable_rmc[25]  = {0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2C, 0x30, 0x34, 0x2C, 0x30, 0x30, 0x2C, 0x30, 0x31, 0x2C, 0x30, 0x31, 0x2A, 0x32, 0x31, 0x0D, 0x0A};                          
	                     //  $    P     S     R     F     1      0     3     ,     0     4     ,     0     0    ,      0     0     ,     0     1    *      2    0     <CR>  <LF>
uint8_t enable_rmc[25]  = {0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2C, 0x30, 0x34, 0x2C, 0x30, 0x30, 0x2C, 0x30, 0x31, 0x2C, 0x30, 0x31, 0x2A, 0x32, 0x31, 0x0D, 0x0A};       
                         //  $    P     S     R     F     1      0     3     ,     0     4     ,     0     0    ,      0     1     ,     0     1    *      2    1     <CR>  <LF>



uint8_t disable_gga[25] = {0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2C, 0x30, 0x30, 0x2C, 0x30, 0x30, 0x2C, 0x30, 0x31, 0x2C, 0x30, 0x31, 0x2A, 0x32, 0x34, 0x0D, 0x0A};
                         //  $    P     S     R     F     1      0     3     ,     0     0     ,     0     0    ,      0     0     ,     0     1    *      2    4     <CR>  <LF>        
uint8_t enable_gga[25]  = {0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2C, 0x30, 0x30, 0x2C, 0x30, 0x30, 0x2C, 0x30, 0x31, 0x2C, 0x30, 0x31, 0x2A, 0x32, 0x34, 0x0D, 0x0A};
                         //  $    P     S     R     F     1      0     3     ,     0     0     ,     0     0    ,      0     1     ,     0     1    *      2    4     <CR>  <LF>         




//uint8_t set_rate[23] = {0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2C, 0x34, 0x30, 0x2C, 0x36, 0x2C, 0x30, 0x30, 0x2C, 0x30, 0x2A, 0x32, 0x33, 0x0D, 0x0A};                  // for  GPRMC
//uint8_t set_rate[21] = {0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2C, 0x30, 0x30, 0x2C, 0x36, 0x2C, 0x30, 0x30, 0x2C, 0x30, 0x2A, 0x32, 0x33};

//Complete list of input commands for your reference to enable/disable output messages commands:

//$PSRF103,00,00,00,01*24<CR><LF> => disable GGA
//$PSRF103,01,00,00,01*25<CR><LF> => disable GLL
//$PSRF103,02,00,00,01*26<CR><LF> => disable GSA
//$PSRF103,03,00,00,01*27<CR><LF> => disable GSV

//$PSRF103,04,00,00,01*20<CR><LF> => disable RMC
//$PSRF103,08,00,00,01*2C<CR><LF> => disable ZDA

//$PSRF103,00,00,01,01*24<CR><LF> => enable GGA
//$PSRF103,01,00,01,01*25<CR><LF> => enable GLL
//$PSRF103,02,00,01,01*26<CR><LF> => enable GSA
//$PSRF103,03,00,01,01*27<CR><LF> => enable GSV

//$PSRF103,04,00,01,01*21<CR><LF> => enable RMC
//$PSRF103,08,00,01,01*2D<CR><LF> => enable ZDA


//*****************
void device_config(void)
{
	for (int j = 0; j < sizeof(gps_config); j++)
	   twi_write_one_reg(trcTwi, ADDR_GPS_W, 0, gps_config[j]);
}


//*****************
void enable_gprmc(void)
{
	for (int j = 0; j < sizeof(enable_rmc); j++)
 	   twi_write_one_reg(trcTwi, ADDR_GPS_W, 0, enable_rmc[j]);
}


//*****************
void disable_gprmc(void)
{
	for (int j = 0; j < sizeof(disable_rmc); j++)
       twi_write_one_reg(trcTwi, ADDR_GPS_W, 0, disable_rmc[j]);
}


//*****************
void enable_gpgga(void)
{
	for (int j = 0; j < sizeof(enable_gga); j++)
 	   twi_write_one_reg(trcTwi, ADDR_GPS_W, 0, enable_gga[j]);
}


//******************
void disable_gpgga(void)
{
	for (int j = 0; j < 25; j++)
 	   twi_write_one_reg(trcTwi, ADDR_GPS_W, 0, disable_gga[j]);
}


//********************
void set_device_rate(void)
{
	for (int j = 0; j < 23; j++)
	   twi_write_one_reg(trcTwi, ADDR_GPS_W, 0, set_rate[j]);
}


//**********************
uint8_t gps_activation(void)
{	
	//GPS_ON_OFF_PIN_TO_HIGH;    // old tracker
	//delay_ms(200);
	//GPS_ON_OFF_PIN_TO_LOW;
	
	GPS_ON_OFF_PIN_TO_LOW;      // new tracker
	delay_ms(50);
	GPS_ON_OFF_PIN_TO_HIGH;
	
 // read of GPS_STATUS to be added when GPS_STATUS input fixed in hardware
	
	//delay_ms(3000);	
	
	//
	//while(true)
	//{
		//if (GpioPoll(GPS_STATUS_PIN))
		//{
			////if (ioport_pin_is_high(GPS_STATUS_PIN)) // old tracker
			//if (ioport_pin_is_low(GPS_STATUS_PIN))    // new tracker   // ONLY IN HARDWARE WITH GPS AND BLUETOOTH WITH "TFIRA"
			//{
				//delay_ms(200);
				//break;
			//}
		//}
	//}
	
    device_config();
	disable_gprmc();
	enable_gpgga();
	set_device_rate();
	
	return(0);
}


//******************************
void add_gps_fix_to_flash_buff(void)
{		
#if 0
	flash_page_buf[flash_page_index] = GPS_PACK_DATA_ID;
	flash_page_index++;
	
	for (int ff = 0; ff < sizeof(measuredUtcTime); ff++)
	{
		flash_page_buf[flash_page_index] = measuredUtcTime[ff];
		flash_page_index++;
	}
	
	for (int ff = 0; ff < sizeof(measuredLatitude); ff++)
	{
		flash_page_buf[flash_page_index] = measuredLatitude[ff];
		flash_page_index++;
	}
	
	for (int ff = 0; ff < sizeof(measuredLongitude); ff++)
	{
		flash_page_buf[flash_page_index] = measuredLongitude[ff];
		flash_page_index++;
	}
	
	for (int ff = 0; ff < sizeof(measuredAltitude); ff++)
	{
		flash_page_buf[flash_page_index] = measuredAltitude[ff];
		flash_page_index++;
	}	
	
	for (int ff = 0; ff < sizeof(measuredSatelites); ff++)
	{
		flash_page_buf[flash_page_index] = measuredSatelites[ff];
		flash_page_index++;
	}
#endif
	gpsRowsRecorded++;
}



//*******************
void fix_led_toggle(void)
{
	if (fix_led_On)
	{
		ONBOARD_LED_3_ON;
		fix_led_On = false;
	}
	else
	{
    	ONBOARD_LED_3_OFF;
		fix_led_On = true;
	}
}	


//**************
#if 0
bool gpgga_fix()
{
	bool a_found   = false;
	bool latFound  = false;
	bool longFound = false;
	
	bool answ = false;
	
	uint8_t pozT   = 7;
	uint8_t pozLat = 0;
	uint8_t pozLon = 0;
	uint8_t pozAlt = 0;
	
	
	for (int k = 0; k < 99; k++)
	{
		if ((fromGps[k] == 0x4E) || (fromGps[k] == 0x53))       // N or S
		{
			latFound = true;
			pozLat = k;
			for (int p=0; p<9; p++)
   			   measuredLatitude[p] = fromGps[pozLat-10+p];
		}
		
		if ((fromGps[k] == 0x45) || (fromGps[k] == 0x57))       // E or W
		{
			longFound = true;
			pozLon = k;
			for (int u=0; u<10; u++)
   			   measuredLongitude[u] = fromGps[pozLon-11+u];
		}
		
		if (latFound && longFound && (fromGps[k] == 0x4D))      // M
		{
			if (pozAlt == 0)
			{
				for (int a = 0; a < 8; a++)
				   measuredAltitude[a] = 0x30;
				
				pozAlt = k;
				for (int u=7; u>=0; u--)
				{
					char ftmp = fromGps[(pozAlt-2)-(7-u)];
					if (ftmp == 0x2c)
					  break;
					measuredAltitude[u] = ftmp;
				}
			}
		}
	}
	
	if  ((latFound) && (longFound))
	{
		for (int v=0; v<10; v++)
  		   measuredUtcTime[v] = fromGps[pozT+v];
		
		for (int v=0; v<2; v++)
    	   measuredSatelites[v] = fromGps[(pozLon+4) + v];		
		
		if (record_to_flash)
		{
		   if (flash_page_index > 235)  
		      WritePageBufferToFlash();
		   else      
		   {  		 		        
		      add_gps_fix_to_flash_buff();
			  fix_led_toggle();			  
		   }
	    }
		
		answ = true;
	}
}
//**************
bool gprmc_fix()
{
	bool a_found   = false;
	bool latFound  = false;
	bool longFound = false;
	
	bool answ = false;
	
	uint8_t pozA   = 0;
	uint8_t pozLat = 0;
	uint8_t pozLon = 0;
	
	
	for (int k = 0; k < 99; k++)
	{
		if (fromGps[k] == 0x41)
		{
			a_found = true;
			pozA = k;
			for (int v=0; v<10; v++)
			{
				measuredUtcTime[v] = fromGps[pozA-11+v];
			}
		}
		
		if (a_found)
		{
			if ((fromGps[k] == 0x4E) || (fromGps[k] == 0x53))       // N or S
			{
				latFound = true;
				
				pozLat = k;
				for (int p=0; p<9; p++)
				{
					measuredLatitude[p] = fromGps[pozLat-10+p];
				}
			}
			
			if ((fromGps[k] == 0x45) || (fromGps[k] == 0x57))       // E or W
			{
				longFound = true;
				pozLon = k;
				for (int u=0; u<10; u++)
				{
					measuredLongitude[u] = fromGps[pozLon-11+u];
				}
			}
		}
	}
		
	if  ((a_found) && (latFound) && (longFound))
	{
	//  WriteSensorsDataToFlash();		
		answ = true;
	}
}
//*******************
bool gpsFixObtained()
{
	uint8_t answ = false;
	
	bool a_gprmc = true;
	bool a_gpgga = true;
	
	bool gprmc_result = false;
	bool gpgga_result = false;
	
	for (int k=0; k < 6; k++)
	{
		if (fromGps[k] != gprmc_mess[k])
		{
			a_gprmc = false;
			break;
		}
	}
	
	for (int k=0; k < 6; k++)
	{
		if (fromGps[k] != gpgga_mess[k])
		{
			a_gpgga = false;
			break;
		}
	}
	
	if (a_gprmc)
	gprmc_result = gprmc_fix();
	
	if (a_gpgga)
	gpgga_result = gpgga_fix();

	if ((gprmc_result) || (gpgga_result))
	   answ = true;
	
	return(answ);
}
#endif


////*********************   // FOR TEST ONLY ! TEMPORARY
//void green_led_toggle()
//{
	//if (green_led_On)
	//{
		//ONBOARD_LED_3_ON;
		//green_led_On = false;
	//}
	//else
	//{
		//ONBOARD_LED_3_OFF;
		//green_led_On = true;
	//}
//}	


//*****************
void get_gps_data(void)
{
	bool startSeq = false;
	int i;
//	uint8_t indx = 0;
	
	current_sensor_id = 2;
	
#if 0
	for (int k = 0; k < 99; k++)
	   fromGps[k] == 0xFF;
#else
//	memset(NMEA_data.sentence,0,sizeof(NMEA_data.sentence));
	NMEA_data.length = 0;
#endif
	
	while(1)
	{
		uint8_t gpsChar = twi_read_one_reg(trcTwi, ORG1140_ADDR, 0x00);
		if (gpsChar == 0)
		{
			gps_activation();
		   break;
		}
		
		//if (!record_to_flash)
#ifdef RELIABLE_UART
		UartWrapSendByte(USART_SERIAL_PC, gpsChar);
#else
		usart_putchar(USART_SERIAL_PC, gpsChar);
#endif
		if (gpsChar == '$')
		  startSeq = true;
		
		if  (startSeq)
		{
			NMEA_data.sentence[NMEA_data.length++] = gpsChar;
		//	indx++;
			
			if ((NMEA_data.length == sizeof(NMEA_data.sentence)) || (gpsChar == 0x0d /*0x2A*/)) // ;
			{
#if 0
				bool fansw = gpsFixObtained();
				if (fansw)
				{
					ONBOARD_LED_2_ON;
				}
				else
				{
					ONBOARD_LED_2_OFF;
				}
#else
				if (record_to_flash)
					TelemetryWriteToFlash(NMEA_TYPE,&NMEA_data);
#endif				
				break;
			}
		}
	}
}


