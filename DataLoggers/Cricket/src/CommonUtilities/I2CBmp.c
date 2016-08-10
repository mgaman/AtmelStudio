/*
 * I2CBmp.c
 *
 * Created: 7/1/2014 3:00:59 PM
 *  Author: User
 */ 


#include <string.h>
#include <math.h>
#include <asf.h>
#include "Utilities.h"
#include "I2CUtils.h"
#include "I2CBmp.h"
#include "conf_board.h"


int AC1,AC2,AC3,VB1,VB2,MB,MC,MD;
unsigned int AC4,AC5,AC6;
double c5,c6,mc,md,x0,x1,x2,y0,y1,y2,p0,p1,p2;

extern double measuredBmpAlt, baselinePress;

extern TWI_t * trcTwi;

extern uint8_t info_to_send_to_host;


//*********************
double getTemperature()
{
	double tu, a,  temper;
	
	twi_write_one_reg(trcTwi, ADDR_BMP_W, BMP180_REG_CONTROL, BMP180_COMMAND_TEMPERATURE);
	
	uint8_t d0 = twi_read_one_reg(trcTwi, BMP180_ADDR, BMP180_REG_RESULT);
	uint8_t d1 = twi_read_one_reg(trcTwi, BMP180_ADDR, BMP180_REG_RESULT);
	
	tu = (d0 * 256.0) + d1;

	//example from Bosch datasheet
	//tu = 27898;

	//example from http://wmrx00.sourceforge.net/Arduino/BMP085-Calcs.pdf
	//tu = 0x69EC;
	
	a = c5 * (tu - c6);
	temper = a + (mc / (a + md));
	
	return(temper);
}



//*********************************************************
double getPressure(char oversampling, double measured_temp)
{
	double pu,s,x,y,z, P;
	uint8_t bmp_comm;

	switch (oversampling)
	{
		case 0 : bmp_comm = BMP180_COMMAND_PRESSURE0; break;
		case 1 : bmp_comm = BMP180_COMMAND_PRESSURE1; break;
		case 2 : bmp_comm = BMP180_COMMAND_PRESSURE2; break;
		case 3 : bmp_comm = BMP180_COMMAND_PRESSURE3; break;
		default: bmp_comm = BMP180_COMMAND_PRESSURE0; break;
	}
	
	twi_write_one_reg(trcTwi, ADDR_BMP_W, BMP180_REG_CONTROL, bmp_comm);
	
	uint8_t d0 = twi_read_one_reg(trcTwi, BMP180_ADDR, BMP180_REG_RESULT);
	uint8_t d1 = twi_read_one_reg(trcTwi, BMP180_ADDR, BMP180_REG_RESULT);
	uint8_t d2 = twi_read_one_reg(trcTwi, BMP180_ADDR, BMP180_REG_RESULT);
	
	pu = (d0 * 256.0) + d1 + (d2/256.0);

	//example from Bosch datasheet
	//pu = 23843;

	//example from http://wmrx00.sourceforge.net/Arduino/BMP085-Calcs.pdf, pu = 0x982FC0;
	//pu = (0x98 * 256.0) + 0x2F + (0xC0/256.0);
	
	s = measured_temp - 25.0;
	x = (x2 * pow(s,2)) + (x1 * s) + x0;
	y = (y2 * pow(s,2)) + (y1 * s) + y0;
	z = (pu - x) / y;
	P = (p2 * pow(z,2)) + (p1 * z) + p0;
	
	return(P);
}



//**********************
double readBmpPressure()
{
	double tmpTemp, tmpPress;
	
	tmpTemp  = getTemperature();
	tmpPress = getPressure(3, tmpTemp);
	
	return(tmpPress);
}



// Given a pressure measurement P (mb) and the pressure at a baseline P0 (mb),
// return altitude (meters) above baseline.
//*****************************************
double getAltitude(double measPress, double basePress)
{
	return(44330.0*(1-pow(measPress/basePress, 1/5.255)));
}



//****************
uint8_t bmp_init()
{
	double c3,c4,b1;
	
	uint8_t retval = 0;
	
	// The BMP180 includes factory calibration data stored on the device.
	// Each device has different numbers, these must be retrieved and
	// used in the calculations when taking pressure measurements.

	// Retrieve calibration data from device:
	
	AC1 = get_two_bytes_data(trcTwi, BMP180_ADDR, 0xAA, 0xAB);
	AC1 = get_two_bytes_data(trcTwi, BMP180_ADDR, 0xAA, 0xAB);	
	AC2 = get_two_bytes_data(trcTwi, BMP180_ADDR, 0xAC, 0xAD);
	AC3 = get_two_bytes_data(trcTwi, BMP180_ADDR, 0xAE, 0xAF);
	AC4 = get_two_bytes_data(trcTwi, BMP180_ADDR, 0xB0, 0xB1);
	AC5 = get_two_bytes_data(trcTwi, BMP180_ADDR, 0xB2, 0xB3);
	AC6 = get_two_bytes_data(trcTwi, BMP180_ADDR, 0xB4, 0xB5);
	VB1 = get_two_bytes_data(trcTwi, BMP180_ADDR, 0xB6, 0xB7);
	VB2 = get_two_bytes_data(trcTwi, BMP180_ADDR, 0xB8, 0xB9);
	MB  = get_two_bytes_data(trcTwi, BMP180_ADDR, 0xBA, 0xBB);
	MC  = get_two_bytes_data(trcTwi, BMP180_ADDR, 0xBC, 0xBD);
	MD  = get_two_bytes_data(trcTwi, BMP180_ADDR, 0xBE, 0xBF);
	

	// Example from Bosch datasheet
	// AC1 = 408; AC2 = -72; AC3 = -14383; AC4 = 32741; AC5 = 32757; AC6 = 23153;
	// B1 = 6190; B2 = 4; MB = -32768; MC = -8711; MD = 2868;

	// Example from http://wmrx00.sourceforge.net/Arduino/BMP180-Calcs.pdf
	// AC1 = 7911; AC2 = -934; AC3 = -14306; AC4 = 31567; AC5 = 25671; AC6 = 18974;
	// VB1 = 5498; VB2 = 46; MB = -32768; MC = -11075; MD = 2432;

	// Compute floating-point polynominals:

	c3 = 160.0 * pow(2,-15) * AC3;
	c4 = pow(10,-3) * pow(2,-15) * AC4;
	b1 = pow(160,2) * pow(2,-30) * VB1;
	c5 = (pow(2,-15) / 160) * AC5;
	c6 = AC6;
	mc = (pow(2,11) / pow(160,2)) * MC;
	md = MD / 160.0;
	x0 = AC1;
	x1 = 160.0 * pow(2,-13) * AC2;
	x2 = pow(160,2) * pow(2,-25) * VB2;
	y0 = c4 * pow(2,15);
	y1 = c4 * c3;
	y2 = c4 * b1;
	p0 = (3791.0 - 8.0) / 1600.0;
	p1 = 1.0 - 7357.0 * pow(2,-20);
	p2 = 3038.0 * 100.0 * pow(2,-36);
	
	baselinePress = readBmpPressure();
	
	return (retval);
}



//*****************
void get_bmp_data()
{
	double tmpPress;
	
	tmpPress = readBmpPressure();
	measuredBmpAlt = getAltitude(tmpPress, baselinePress);	
}



//********************************************************
void send_bmp_info(unsigned long params_buf[], int fusize)
{
	char buf[5];
	char buf_all[12];
	
	for (int ff = 0; ff < sizeof(buf_all); ff++)
	   buf_all[ff] = 0x30;
	
	
	int buff_all_indx = 0;
	for (int i = 0; i < fusize; i++)
	{
		for (int ff = 0; ff < sizeof(buf); ff++)
		   buf[ff] = 0x30;
		
		unsigned long tmpval = params_buf[i];
		itoa(tmpval, buf, 5);
		for (int ff = 0; ff < sizeof(buf); ff++)
		{
			buf_all[buff_all_indx] = buf[ff];
			buff_all_indx++;
		}
		
		buf_all[buff_all_indx] = 0x3B;
		buff_all_indx++;
	}
	
	
	//for (int ff = 0; ff < sizeof(buf_all); ff++)      // For test
	 //  usart_putchar(USART_SERIAL_PC, buf_all[ff]);    // For test
}




//*********************
void new_get_bmp_data()
{
	uint8_t msb, lsb, xlsb;
	
	long bmpVals[2];
	
	char buf[5];
	char buf_all[12];
	
//	twi_write_one_reg(trcTwi, ADDR_BMP_W, BMP180_REG_CONTROL, BMP180_COMMAND_TEMPERATURE);

	twi_write_one_reg(trcTwi, ADDR_BMP_W, 0xF4, 0x2E);
	uint16_t UT = get_two_bytes_data(trcTwi, BMP180_ADDR, 0xF6, 0xF7);
  
    uint8_t ff = 0x34 + (0<<6); 
    twi_write_one_reg(trcTwi, ADDR_BMP_W, 0xF4, ff);	

    msb  = twi_read_one_reg(trcTwi, BMP180_ADDR, 0xF6);
	lsb  = twi_read_one_reg(trcTwi, BMP180_ADDR, 0xF7);
	xlsb = twi_read_one_reg(trcTwi, BMP180_ADDR, 0xF8);	
	
	long temp  = (long)xlsb;
    temp |= ((long)lsb) << 8;
    temp |= ((long)msb) << 16;
    temp = (temp >> 8);	
	long UP = temp;	
	
	//int32_t f1 = (UT-AC6);
	//int32_t f2 = AC5/pow(2,15);
	//int32_t f3 = f1*f2;
	//
	//int32_t X1 = (UT-AC6) * AC5/(pow(2,15));	
	//int32_t X2 = (MC * pow(2,11))/(X1+MD);
	//int32_t B5 = X1 + X2;
	//int32_t T = (B5+8)/pow(2,4);
	
	
	//int32_t X1 = (UT-AC6) * AC5/32768;
	//int32_t X2 = MC;
	//X2 *= 2048;
	//X2 /= (X1 + MD);
	//
	//int32_t B5 = X1 + X2;
	//int32_t T = (B5+8)/16;
	
	bmpVals[0] = UT;
	bmpVals[1] = UP;		

	for (int ff = 0; ff < sizeof(buf_all); ff++)
	   buf_all[ff] = 0x30;
		
	int buff_all_indx = 0;
	for (int i = 0; i < 2; i++)
	{
		for (int ff = 0; ff < sizeof(buf); ff++)
		buf[ff] = 0x30;
		
		uint16_t tmpval = (int)bmpVals[i];
		itoa(tmpval, buf, 5);
		for (int ff = 0; ff < sizeof(buf); ff++)
		{
			buf_all[buff_all_indx] = buf[ff];
			buff_all_indx++;
		}
		
		buf_all[buff_all_indx] = 0x3B;
		buff_all_indx++;
	}
	
	
	//for (int ff = 0; ff < sizeof(buf_all); ff++)      // For test
	//usart_putchar(USART_SERIAL_PC, buf_all[ff]);    // For test
	
	
	
	

	// if (info_to_send_to_host == 2)

	//send_bmp_info(bmpVals, 2);
}
