/*
 * i2c_bar.c
 *
 * Created: 28-Dec-14 9:06:43 AM
 *  Author: User
 */ 



#include <string.h>
#include <stdio.h>
#include <math.h>
#include <asf.h>

#include "Utilities.h"
#include "uart.h"
#include "avr_compiler.h"
#include "twi_master_driver.h"
#define MS5611_MAIN
#include "i2c_bar.h"
#include "SpiFlash.h"
#include "DataLogger.h"
#include "ReliableUART.h"
#include "Telemetry.h"

extern TWI_Master_t twiMaster; 
struct sMS5611_Calibration MSCalib;
struct sMS5611_Data MSData;

unsigned long D1; // ADC value of the pressure conversion
unsigned long D2; // ADC value of the temperature conversion
uint16_t C[8]; // calibration coefficients
double PRESI; // compensated pressure value
double TEMPE; // compensated temperature value
double dT; // difference between actual and measured temperature
double OFF; // offset at actual temperature
double SENS; // sensitivity at actual temperature
int i;
unsigned char n_crc; // crc value of the prom

extern bool record_to_flash;
extern volatile uint32_t time_stamp;
extern volatile char flash_page_buf[];
extern bool page_written_to_flash;
extern volatile uint16_t flash_page_index;

#define CMD_RESET 0x1E // ADC reset command
#define CMD_ADC_READ 0x00 // ADC read command
#define CMD_ADC_CONV 0x40 // ADC conversion command
#define CMD_ADC_D1 0x00 // ADC D1 conversion
#define CMD_ADC_D2 0x10 // ADC D2 conversion
#define CMD_ADC_256 0x00 // ADC OSR=256
#define CMD_ADC_512 0x02 // ADC OSR=512
#define CMD_ADC_1024 0x04 // ADC OSR=1024
#define CMD_ADC_2048 0x06 // ADC OSR=2048
#define CMD_ADC_4096 0x08 // ADC OSR=4096
#define CMD_PROM_RD 0xA0 // Prom read command

#define VARIO_SLAVE_ADDRESS  0xEC

#define BAR_PACK_DATA_ID 0x22

void n_send_vario_calib_to_host()
{
	uint8_t *cm;
	uint32_t cookedlength;
	int i;
		// send SM5611 calibration data to host
	MSCalib.header.type = MS561101BA03_CALIB;
	memcpy(MSCalib.C,&C[1],6*sizeof(uint16_t));
	UartWrapSendByte(USART_SERIAL_PC,StartVarioCalibToHost);
	cm = UartWrap(&MSCalib,sizeof(MSCalib),&cookedlength);
	for (i=0;i<cookedlength;i++)
		usart_putchar(USART_SERIAL_PC,*cm++);
	UartWrapSendByte(USART_SERIAL_PC,EndVarioCalibToHost);
}
//******************************************************** 
//! @brief Read calibration coefficients 
//! 
//! @return coefficient 
//******************************************************** 
uint16_t cmd_prom(char coef_num) 
{ 
	uint16_t ret; 
	uint16_t rC=0; 
	/*! Buffer with test data to send.*/
	uint8_t sendBuffer[1] = {CMD_PROM_RD+coef_num*2};

	TWI_MasterWriteRead(&twiMaster, VARIO_SLAVE_ADDRESS >> 1, &sendBuffer[0], 1, 2);
	while (twiMaster.status != TWIM_STATUS_READY);
	rC = twiMaster.readData[0] * 256;
	rC += twiMaster.readData[1];

	return rC; 
} 


//********************************************************
//! @brief calculate the CRC code
//!
//! @return crc code
//********************************************************
unsigned char crc4(unsigned int n_prom[])
{
	int cnt; // simple counter
	unsigned int n_rem; // crc reminder
	unsigned int crc_read; // original value of the crc
	unsigned char n_bit;
	n_rem = 0x00;
	crc_read=n_prom[7]; //save read CRC
	n_prom[7]=(0xFF00 & (n_prom[7])); //CRC byte is replaced by 0
	for (cnt = 0; cnt < 16; cnt++) // operation is performed on bytes
	{// choose LSB or MSB
		if (cnt%2==1) n_rem ^= (unsigned short) ((n_prom[cnt>>1]) & 0x00FF);
		else n_rem ^= (unsigned short) (n_prom[cnt>>1]>>8);
		for (n_bit = 8; n_bit > 0; n_bit--)
		{
			if (n_rem & (0x8000))
			{
				n_rem = (n_rem << 1) ^ 0x3000;
			}
			else
			{
				n_rem = (n_rem << 1);
			}
		}
	}
	n_rem= (0x000F & (n_rem >> 12)); // final 4-bit reminder is CRC code
	n_prom[7]=crc_read; // restore the crc_read to its original place
	return (n_rem ^ 0x0);
}


//*****************************
//! @brief preform adc conversion
//!
//! @return 24bit result
//*****************************
unsigned long cmd_adc(char cmd)
{
	unsigned int ret;
	unsigned long temp=0;
	
	
	uint8_t sendBuffer[1] = {CMD_ADC_CONV+cmd};
	TWI_MasterWriteRead(&twiMaster, VARIO_SLAVE_ADDRESS >> 1, &sendBuffer[0], 1, 0);
	switch (cmd & 0x0f) // wait necessary conversion time
	{
		case CMD_ADC_256 : _delay_us(900); break;
		case CMD_ADC_512 : _delay_ms(3); break;
		case CMD_ADC_1024: _delay_ms(4); break;
		case CMD_ADC_2048: _delay_ms(6); break;
		case CMD_ADC_4096: _delay_ms(10); break;
	}
	
	sendBuffer[0] = CMD_ADC_READ;
	bool ok = TWI_MasterWriteRead(&twiMaster, VARIO_SLAVE_ADDRESS >> 1, &sendBuffer[0], 1, 3);
	while (twiMaster.status != TWIM_STATUS_READY);
	if (ok)
	{//issuing start condition ok, device accessible
		ret = twiMaster.readData[0]; // read MSB and acknowledge
		temp=65536*ret;
		ret = twiMaster.readData[1]; // read byte and acknowledge
		temp=temp+256*ret;
		ret = twiMaster.readData[2]; // read LSB and not acknowledge
		temp=temp+ret;
	}
	return temp;
}


#if 0
//*****************************************************
void add_vario_data_to_flash_buff()
{	
	if (!calibWritten)
	{
		memcpy(MSCalib.C,&C[1],6*sizeof(uint16_t));	// copy config words 1 to 6
		TelemetryWriteToFlash(MS561101BA03_CALIB,&MSCalib);
		calibWritten = true;
	}
	MSData.D1 = D1;
	MSData.D2 = D2;
	TelemetryWriteToFlash(MS561101BA03,&MSData);
}
#endif

//*************
bool init_bar()
{
	// no chip id here so just look for a sensible value
	C[1] = 0xffff;
	for (int i=0;i<sizeof(C)/sizeof(uint16_t);i++)                               // read coefficients
	  C[i] = cmd_prom(i);
	  
	n_crc=crc4(C);                                      // calculate the CRC
//	return (C[1] != 0xff && C[1] != 0);
	return true;
}


//**********************************
void n_send_vario_raw_data_to_host()
{
	char *cm;
	uint32_t cookedlength;
	int i;
    if (!VarioCalibRcvd)
	{
		n_send_vario_calib_to_host();
	}
	MSData.header.type = MS561101BA03;
	MSData.header.type = MS561101BA03;
	MSData.header.timestamp = time_stamp;
	MSData.D1 = D1;
	MSData.D2 = D2;
    UartWrapSendByte(USART_SERIAL_PC, StartVarioDataToHost);
	cm = UartWrap(&MSData,sizeof(MSData),&cookedlength);
	for (i=0;i<cookedlength;i++)
		usart_putchar(USART_SERIAL_PC,*cm++);
	UartWrapSendByte(USART_SERIAL_PC, EndVarioDataToHost);
}
//******************
// just read raw data, no processing
void read_bar_data()
{
	D2=cmd_adc(CMD_ADC_D2+CMD_ADC_256);     //CMD_ADC_4096
	D1=cmd_adc(CMD_ADC_D1+CMD_ADC_256);
}


/*! TWIE Master Interrupt vector. */
ISR(TWIE_TWIM_vect)
{
	TWI_MasterInterruptHandler(&twiMaster);
}

void MS561101BA03Handler()
{
	read_bar_data();
	// creates telemetry block & sends to host
	n_send_vario_raw_data_to_host();
	// if recording to flash send same block to flash
	if (record_to_flash)
	{
		if (!VarioCalibWrittenToTelem)
		{
			memcpy(MSCalib.C,&C[1],6*sizeof(uint16_t));	// copy config words 1 to 6
			TelemetryWriteToFlash(MS561101BA03_CALIB,&MSCalib);
			VarioCalibWrittenToTelem = true;
		}
		TelemetryWriteToFlash(MS561101BA03,&MSData);
		varRowsRecorded++;
	}
}

void MS561101BA03Reset()
{
	VarioCalibWrittenToTelem = false;
	VarioCalibRcvd = false;
}