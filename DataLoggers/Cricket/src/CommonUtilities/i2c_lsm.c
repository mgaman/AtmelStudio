/*
 * i2c_lsm.c
 *
 * Created: 24-Nov-14 3:15:09 PM
 *  Author: User
 */ 

#include <string.h>
#include <math.h>
#include <asf.h>
#include <stdlib.h>
#include <stdio.h>

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "usart.h"
#include "avr_compiler.h"

#include "uart.h"
#include "twi_master_driver.h"

#define LSM9DS0_MAIN
#include "i2c_lsm.h"
#include "Utilities.h"
#include "I2CUtils.h"
#include "conf_board.h"
//#include "SpiFlash.h"
#include "DataLogger.h"
#include "Telemetry.h"
#include "ReliableUART.h"

uint8_t ConfigGRegs[] = {
	G_ODR_95_BW_25|PD|XEN|YEN|ZEN,      //CTRL_REG1_G
	0,                                  //CTRL_REG2_G
	I2_DRDY|I1_INT1,                    //CTRL_REG3_G
	G_SCALE_500DPS,	                    //CTRL_REG4_G
	0                                   //CTRL_REG5_G
};

uint8_t ConfigXMRegs[] = {
	0,                            //CTRL_REG0_XM
	A_ODR_100|AXEN|AYEN|AZEN,     //CTRL_REG1_XM
	0,                            //CTRL_REG2_XM
	0,		                      //CTRL_REG3_XM
	0,				              //CTRL_REG4_XM
	M_ODR_100|M_LO_RES,           //CTRL_REG5_XM
	0,                            //CTRL_REG6_XM
	0                             //CTRL_REG7_XM
};

struct sLSM9DS0_Data LSM9D0Data;
struct sLSM9DS0_Data LSM9D0Calib; // same structure as data

extern TWI_Master_t twiMaster;   
void wait_for_wif();
//void wait_for_rif();
uint8_t data[8];
//uint8_t regi;

//int timespan;

extern volatile uint32_t time_stamp;
//extern uint8_t current_sensor_id;


extern bool record_to_flash;
extern bool page_written_to_flash;
extern unsigned long lsmRowsRecorded;
extern bool LSMCalibRcvd;

extern volatile uint32_t glob_timestamp;
extern uint8_t myID;

//*******************************************************************
unsigned int twi_master_get_reg(uint8_t slave_addr, uint8_t reg_addr) 
//uint8_t twi_master_get_reg(uint8_t slave_addr, uint8_t reg_addr)
{ 
	//unsigned int ret; 
	unsigned int rC=0; 
	
	//uint8_t rC=0;

	uint8_t sendBuffer[1] = {reg_addr};
	TWI_MasterWriteRead(&twiMaster, slave_addr, sendBuffer, 1, 1);	
	while (twiMaster.status != TWIM_STATUS_READY);
	rC = twiMaster.readData[0];

	return rC; 
} 


//**********************************************************************************************
uint16_t twi_master_get_two_bytes_data(uint8_t slaveTwiAddr, uint8_t regHighAddr, uint8_t regLowAddr)
{
	uint16_t ret = 0;
	uint8_t high_byte = 0;
	uint8_t low_byte  = 0;
	
	high_byte = twi_master_get_reg(slaveTwiAddr, regHighAddr);
	delay_ms(1);
	low_byte  = twi_master_get_reg(slaveTwiAddr, regLowAddr);
	delay_ms(1);
	ret = (high_byte<<8) | low_byte;

	return(ret);
}


//********************************************************
void master_write_with_wait_for_bus_ready(uint8_t address)
{
	TWI_MasterWrite(&twiMaster, LSM9DS0_XM, data, 2);
	while (twiMaster.status != TWIM_STATUS_READY);
}


//**************
void initXM()
{
	int i;
	for (i=0;i<sizeof(ConfigXMRegs);i++)
	{
		data[0] = CTRL_REG0_XM + i;
		data[1] = ConfigXMRegs[i];
		TWI_MasterWrite(&twiMaster, LSM9DS0_XM, data, 2);
		wait_for_wif();
		while (twiMaster.status != TWIM_STATUS_READY);
		delay_ms(10);
	}
}

//*************
void initGyro()
{
	for (int i=0;i<sizeof(ConfigGRegs);i++)
	{
		data[0] = CTRL_REG1_G + i;
		data[1] = ConfigGRegs[i];
		TWI_MasterWrite(&twiMaster, LSM9DS0_G, data, 2);
		while (twiMaster.status != TWIM_STATUS_READY);
		wait_for_wif();
		delay_ms(10);
	}
}
#if 0
#define LSM9DS0_G_W  (LSM9DS0_G<<1)
extern TWI_t * trcTwi;

//******************************
void o_setGyroODR(uint8_t gRate)
{
	uint8_t temp = twi_read_one_reg(trcTwi, LSM9DS0_G, CTRL_REG1_G);                    // We need to preserve the other bytes in CTRL_REG1_G. So, first read it:
	temp &= 0xFF^(0xF << 4);                                                            // Then mask out the gyro ODR bits:
	temp |= (gRate << 4);                                                               // Then shift in our new ODR bits:
	twi_write_one_reg(trcTwi, LSM9DS0_G_W, CTRL_REG1_G, temp);   	                    // And write the new register value back into CTRL_REG1_G:   
}


//************************
void o_setGyroScale(uint8_t gScl)
{
	uint8_t temp = twi_read_one_reg(trcTwi, LSM9DS0_G, CTRL_REG4_G);                   // We need to preserve the other bytes in CTRL_REG4_G. So, first read it:
	temp &= 0xFF^(0x3 << 4);                                                           // Then mask out the gyro scale bits:
	temp |= gScl << 4;                                                                 // Then shift in our new scale bits:
	twi_write_one_reg(trcTwi, LSM9DS0_G_W, CTRL_REG4_G, temp);                         // And write the new register value back into CTRL_REG4_G:	
	
	// We've updated the sensor, but we also need to update our class variables
	// First update gScale:
	//gScale = gScl;
	// Then calculate a new gRes, which relies on gScale being set correctly:
	//calcgRes();
}

o_initGyro()
{
	trcTwi = &TWIE;
	twi_write_one_reg(trcTwi, LSM9DS0_G_W, CTRL_REG1_G, 0x0F);
	twi_write_one_reg(trcTwi, LSM9DS0_G_W, CTRL_REG2_G, 0x00);
	twi_write_one_reg(trcTwi, LSM9DS0_G_W, CTRL_REG3_G, 0x88);
	twi_write_one_reg(trcTwi, LSM9DS0_G_W, CTRL_REG4_G, 0x00);
	twi_write_one_reg(trcTwi, LSM9DS0_G_W, CTRL_REG5_G, 0x00);
	
	// Temporary !!! For testing !!! Remove !!! Or make useful !!!
	//configGyroInt(0x2A, 0, 0, 0, 0); // Trigger interrupt when above 0 DPS...
	
	o_setGyroODR(0xB);       // 100 Hz
	o_setGyroScale(0x01);
}
#endif

//***********************
void setGyroODR(uint8_t gRate)
{
	uint8_t temp = twi_master_get_reg(LSM9DS0_G, CTRL_REG1_G);                          // We need to preserve the other bytes in CTRL_REG1_G. So, first read it:
	
	temp &= 0xFF^(0xF << 4);                                                            // Then mask out the gyro ODR bits:
	temp |= (gRate << 4);                                                               // Then shift in our new ODR bits:

	data[0] = CTRL_REG1_G;
	data[1] = temp;
	TWI_MasterWrite(&twiMaster, LSM9DS0_G, data, 2);                                    // And write the new register value back into CTRL_REG1_G: 
	while (twiMaster.status != TWIM_STATUS_READY);
	//master_write_with_wait_for_bus_ready(LSM9DS0_G);
}


//*****************************
void setGyroScale(uint8_t gScl)
{
	uint8_t temp = twi_master_get_reg(LSM9DS0_G, CTRL_REG4_G);                         // We need to preserve the other bytes in CTRL_REG4_G. So, first read it:
	temp &= 0xFF^(0x3 << 4);                                                           // Then mask out the gyro scale bits:
	temp |= gScl << 4;                                                                 // Then shift in our new scale bits:

	data[0] = CTRL_REG4_G;
	data[1] = temp;
	TWI_MasterWrite(&twiMaster, LSM9DS0_G, data, 2);
	while (twiMaster.status != TWIM_STATUS_READY);
	//master_write_with_wait_for_bus_ready(LSM9DS0_G);
}

void x_InitGyro()
{
	data[0] = CTRL_REG1_G;
	data[1] = ConfigGRegs[0]; //0x0F;
	TWI_MasterWrite(&twiMaster, LSM9DS0_G, data, 2);
	while (twiMaster.status != TWIM_STATUS_READY);
	//master_write_with_wait_for_bus_ready(LSM9DS0_G);
	
	delay_ms(10);
	
	data[0] = CTRL_REG2_G;
	data[1] = ConfigGRegs[1]; //0x00;
	TWI_MasterWrite(&twiMaster, LSM9DS0_G, data, 2);
	while (twiMaster.status != TWIM_STATUS_READY);
	//master_write_with_wait_for_bus_ready(LSM9DS0_G);
	
	delay_ms(10);
	
	data[0] = CTRL_REG3_G;
	data[1] = ConfigGRegs[2]; //0x88;
	TWI_MasterWrite(&twiMaster, LSM9DS0_G, data, 2);
	while (twiMaster.status != TWIM_STATUS_READY);
	//master_write_with_wait_for_bus_ready(LSM9DS0_G);
	
	delay_ms(10);
	
	data[0] = CTRL_REG4_G;
	data[1] = ConfigGRegs[3]; //0x00;
	TWI_MasterWrite(&twiMaster, LSM9DS0_G, data, 2);
	while (twiMaster.status != TWIM_STATUS_READY);
	//master_write_with_wait_for_bus_ready(LSM9DS0_G);
	
	delay_ms(10);
	
	data[0] = CTRL_REG5_G;
	data[1] = ConfigGRegs[4]; //0x00;
	TWI_MasterWrite(&twiMaster, LSM9DS0_G, data, 2);
	while (twiMaster.status != TWIM_STATUS_READY);
	//master_write_with_wait_for_bus_ready(LSM9DS0_G);
	
	delay_ms(10);

	
	setGyroODR(G_ODR_190_BW_50);      /*G_ODR_380_BW_50*/  /*G_ODR_760_BW_50*/
	delay_ms(10);
	setGyroScale(G_SCALE_500DPS);
	delay_ms(10);
}



//**************************
bool new_lsm_init_check()
{
	uint8_t whoAmI_xm = 0;
	uint8_t whoAmI_g  = 0;	
	
	whoAmI_xm = twi_master_get_reg(LSM9DS0_XM, WHO_AM_I_XM);
	delay_ms(5);
	whoAmI_g  = twi_master_get_reg(LSM9DS0_G, WHO_AM_I_G);
	delay_ms(5);
	if ((whoAmI_xm == LSM9DS0_XM_ID) && (whoAmI_g == LSM9DS0_GYRO_ID))
	{
		initXM();
	//	initMag();
		//o_initGyro();
		initGyro();
	//	x_InitGyro();
		memset(&LSM9D0Data,0xff,sizeof(LSM9D0Data));
		memset(&LSM9D0Calib,0xff,sizeof(LSM9D0Calib));
		return true;
	}
	else
		return false;
}

//*********************
void LSM9DS0Handler()
{
	uint8_t *cm;
	uint32_t cookedLength;
	int i;

    // these must be in the same order as in telemetry block
	LSM9D0Data.Accelerometer[0] = twi_master_get_two_bytes_data(LSM9DS0_XM, OUT_X_H_A, OUT_X_L_A);
	LSM9D0Data.Accelerometer[1] = twi_master_get_two_bytes_data(LSM9DS0_XM, OUT_Y_H_A, OUT_Y_L_A);	   
	LSM9D0Data.Accelerometer[2] = twi_master_get_two_bytes_data(LSM9DS0_XM, OUT_Z_H_A, OUT_Z_L_A);	
	
	LSM9D0Data.Magnetometer[0] = twi_master_get_two_bytes_data(LSM9DS0_XM, OUT_X_H_M, OUT_X_L_M);
	LSM9D0Data.Magnetometer[1] = twi_master_get_two_bytes_data(LSM9DS0_XM, OUT_Y_H_M, OUT_Y_L_M);
	LSM9D0Data.Magnetometer[2] = twi_master_get_two_bytes_data(LSM9DS0_XM, OUT_Z_H_M, OUT_Z_L_M);
	
	LSM9D0Data.Gyro[0] = twi_master_get_two_bytes_data(LSM9DS0_G, OUT_X_H_G,  OUT_X_L_G);
	LSM9D0Data.Gyro[1] = twi_master_get_two_bytes_data(LSM9DS0_G, OUT_Y_H_G,  OUT_Y_L_G);
	LSM9D0Data.Gyro[2] = twi_master_get_two_bytes_data(LSM9DS0_G, OUT_Z_H_G,  OUT_Z_L_G);		
	LSM9D0Data.header.ID = myID;
	LSM9D0Data.header.type = LSM9DS0_ALL;
	LSM9D0Data.header.timestamp = time_stamp;
	LSM9D0Calib.header.ID = myID;
	LSM9D0Calib.header.type = LSM9DS0_CALIB;
	LSM9D0Calib.header.timestamp = time_stamp;
	
	if (!LSMCalibRcvd)
	{
		UartWrapSendByte(USART_SERIAL_PC, StartLSMCalibToHost);
		cm = UartWrap(&LSM9D0Calib,sizeof(struct sLSM9DS0_Data),&cookedLength);
		for (i=0;i<cookedLength;i++)
			usart_putchar(USART_SERIAL_PC,*cm++);
		UartWrapSendByte(USART_SERIAL_PC, EndLSMCalibToHost);			
	}
	UartWrapSendByte(USART_SERIAL_PC, StartLSMDataToHost);
	cm = UartWrap(&LSM9D0Data,sizeof(struct sLSM9DS0_Data),&cookedLength);
	for (i=0;i<cookedLength;i++)
		usart_putchar(USART_SERIAL_PC,*cm++);
	UartWrapSendByte(USART_SERIAL_PC, EndLSMDataToHost);
	// always write to host, optionally to host
	if (record_to_flash)
	{
		if (!LSMCalibWrittenToTelem)
		{
			LSMCalibWrittenToTelem = true;
			TelemetryWriteToFlash(LSM9DS0_CALIB,&LSM9D0Calib);
		}
		TelemetryWriteToFlash(LSM9DS0_ALL,&LSM9D0Data);
		lsmRowsRecorded++;
	}
}

bool LSM9DS0GReady()
{
	uint8_t temp = twi_master_get_reg(LSM9DS0_G,STATUS_REG_G);
	return ((temp & XYZDA) == XYZDA);
}

void LSM9DS0Reset()
{
	LSMCalibWrittenToTelem = true;	// not handled by PC yet
	LSMCalibRcvd = false;
}