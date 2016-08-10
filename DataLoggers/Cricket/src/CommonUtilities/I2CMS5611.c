/*
 * I2CMS5611.c
 *
 * Created: 10/21/2014 1:58:33 PM
 *  Author: User
 */ 

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <asf.h>

#include "Utilities.h"
#include "I2CUtils.h"
#include "I2CLSM.h"
#include "I2CMS5611.h"
#include "conf_board.h"
#include "SpiFlash.h"
#include "ReliableUART.h"
#include "DataLogger.h"
#include "Telemetry.h"

extern TWI_t * trcTwi;

struct sMS5611_Calibration MSCalib;
struct sMS5611_Data MSData;

uint16_t fc[MS561101BA_PROM_REG_COUNT];		// calibration registers
uint32_t rawPress,rawTemp;
uint8_t CurrentOSR = MS561101BA_OSR_256;
bool CalibSent = false;

extern bool record_to_flash;
extern bool write_one_row_to_flash;
extern bool request_to_delete_flash;

extern uint8_t current_sensor_id;





//*****************
void ms5611_reset(void)
{
	twi_write_one_reg(trcTwi, MS561101BA_ADDR_CSB_HIGH, MS561101BA_RESET,   0);
	delay_s(1);
	twi_write_one_reg(trcTwi, MS561101BA_ADDR_CSB_HIGH, MS561101BA_RESET,   0);
}


//*************************
uint8_t ms5611_init_check(void)
{
	uint32_t raw_temp = 0; uint32_t raw_pres = 0;
	
	char temp_buf[5];
    char temp_buf_1[2];
	char temp_buf_2[2];
	
	char pres_buf[7];	
	
	memset(fc,0,sizeof(fc));
		
	ms5611_reset();
	
	for (int i=0; i<MS561101BA_PROM_REG_COUNT; i++)
	   fc[i] = twi_read_ms5611_coef(trcTwi, MS561101BA_ADDR_CSB_HIGH, MS561101BA_PROM_BASE_ADDR + (i * MS561101BA_PROM_REG_SIZE));
	   
	
	bool bad_coef = false;
	for (int i=0; i<MS561101BA_PROM_REG_COUNT; i++)
	{
		if (fc[i] == 0)
		{
			bad_coef = true;
			break;
		}
	}
	
	if (!bad_coef)
	{
		return(0);
	}
		
	return(1);
}


//********************************************************************
void send_vario_raw_data_to_host(uint32_t raw_temp, uint32_t raw_pres)
{
    char printbuf[20];
	char *cm;
	uint32_t length,i;
	
	snprintf(printbuf,20,"%lu;%lu;",raw_temp,raw_pres);
	
    UartWrapSendByte(USART_SERIAL_PC, StartVarioDataToHost);
	cm = UartWrap(printbuf,strlen(printbuf),&length);
	for (i=0;i<length;i++)
		usart_putchar(USART_SERIAL_PC,*cm++);
	UartWrapSendByte(USART_SERIAL_PC,EndVarioDataToHost);
}

void WriteVarioDataToFlash()
{	
	if (!CalibSent)
	{
		CalibSent = true;
		memcpy(MSCalib.C,fc,sizeof(fc));
		TelemetryWriteToFlash(MS561101BA03_CALIB,&MSCalib);
	}
	MSData.pressure = rawPress;
	MSData.temperature = rawTemp;
	TelemetryWriteToFlash(MS561101BA03,&MSData);
}

//********************
void get_ms5611_data(void)
{
	current_sensor_id = 0;
   rawPress = twi_read_ms5611_pt(trcTwi, MS561101BA_ADDR_CSB_HIGH, MS561101BA_D1 + CurrentOSR);  //rawPressure(OSR);
   rawTemp = twi_read_ms5611_pt(trcTwi, MS561101BA_ADDR_CSB_HIGH, MS561101BA_D2 + CurrentOSR);
	
	if (record_to_flash)
	{
		WriteVarioDataToFlash();
	}
	else
	{
		send_vario_raw_data_to_host(rawTemp, rawPress);
		  
		if (write_one_row_to_flash)
		{
			WriteVarioDataToFlash();
			write_one_row_to_flash = false;
		}
	}
}




















