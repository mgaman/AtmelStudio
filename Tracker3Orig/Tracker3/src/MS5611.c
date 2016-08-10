/*
    MS5611 handler
*/

#include <asf.h>
#include <string.h>
#include <math.h>
#include "MS5611.h"

//void _delay_us(double __us);
//void _delay_ms(double __us);
void myWaitMs(unsigned int);

status_code_t master_status;
	// Package to send
twi_package_t packet = {
	.addr_length = 0,
	.addr = 0,
	// issue to slave
	.chip = MS5611_SLAVE_ADDRESS,
	// Wait if bus is busy
	.no_wait = false,
	.buffer = 0,
	.length = 0
	};
uint16_t C[8]; // calibration coefficients
unsigned char n_crc; // crc value of the prom
unsigned char crc4(uint16_t *);

//********************************************************
//! @brief calculate the CRC code
//!
//! @return crc code
//********************************************************
unsigned char crc4(uint16_t n_prom[])
{
	int cnt; // simple counter
	uint16_t n_rem; // crc reminder
	uint16_t crc_read; // original value of the crc
	uint8_t n_bit;
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

bool MS561101BA03Reset()
{
	int i;
	packet.addr[0] = CMD_RESET;
	packet.addr_length = 1;
	packet.chip = MS5611_SLAVE_ADDRESS;
	packet.length = 1;
	// Send package to slave
	master_status = twi_master_write(&TWI_MASTER, &packet);
	if (master_status == STATUS_OK)
	{
		// read calibration registers, no autoincrement so need to read separately
		memset(C,0xff,sizeof(C));
		for (i=0;i<sizeof(C)/sizeof(uint16_t);i++)
//		i = 0;
//		while (master_status == STATUS_OK && i < sizeof(C)/sizeof(uint16_t))
		{
			packet.addr[0] = CMD_PROM_RD+ (i*2);
			packet.addr_length = 1;
			packet.buffer = &C[i];
			packet.length = sizeof(uint16_t);
			master_status = twi_master_read(&TWI_MASTER, &packet);	
		//	i++;		
		}
		n_crc=crc4(C);    // calculate the CRC
	}
	//return n_crc == C[7]&0x0f;
	return true;
}

uint32_t MS561101BA03ReadADC(uint8_t command)
{
	uint8_t tp[3];	
	// start conversion
	packet.addr[0] = CMD_ADC_CONV | command;
	packet.addr_length = 1;
	packet.buffer = 0;
	packet.length = 0;
	master_status = twi_master_write(&TWI_MASTER, &packet);
	// wait for conversion to finish
	switch (command & 0xf)
	{
		case CMD_ADC_256 : myWaitMs(1); break;
		case CMD_ADC_512 : myWaitMs(3); break;
		case CMD_ADC_1024: myWaitMs(4); break;
		case CMD_ADC_2048: myWaitMs(6); break;
		case CMD_ADC_4096: myWaitMs(10); break;		
	}
	// read 24 bits of data
	packet.addr[0] = CMD_ADC_READ;
	packet.addr_length = 1;
	packet.buffer = tp;
	packet.length = sizeof(tp) * 3;
	master_status = twi_master_read(&TWI_MASTER, &packet);
	return master_status == STATUS_OK ? *(uint32_t *)tp : 0xffff;
}

void MS561101BA03FormatAltimeter(uint32_t D1,uint32_t D2,double *Temp,double *Press)
{
	// orignal code in C example refers to C[4] in an array of 8
	// we have an array of 6 starting from 2nd item in original array so C[n] becomes C[n-1]
	//double P; // compensated pressure value
	//double T; // compensated temperature value
	double dT; // difference between actual and measured temperature
	double OFF; // offset at actual temperature
	double SENS; // sensitivity at actual temperature
	dT=D2-C[4]*pow(2.0,8.0);
	OFF=C[1]*pow(2.0,17.0)+dT*C[3]/pow(2.0,6.0);
	SENS=C[0]*pow(2.0,16.0)+dT*C[2]/pow(2.0,7.0);
	
	*Temp=(2000+(dT*C[5])/pow(2.0,23.0))/100;   // temperature * 100
	*Press=(((D1*SENS)/pow(2.0,21.0)-OFF)/pow(2.0,15.0))/100; // millibar
}
