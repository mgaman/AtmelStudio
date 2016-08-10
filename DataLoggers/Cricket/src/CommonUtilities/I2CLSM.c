/*
 * I2CLSM.c
 *
 * Created: 7/17/2014 11:48:12 AM
 *  Author: User
 */ 


#include <string.h>
#include <math.h>
#include <asf.h>
#include <stdlib.h>
#include <stdio.h>

#include "Utilities.h"
#include "I2CUtils.h"
#include "I2CLSM.h"
#include "conf_board.h"
#include "SpiFlash.h"
#include "ReliableUART.h"
#include "telemetry.h"
#include "DataLogger.h"

extern TWI_t * trcTwi;
extern uint8_t info_to_send_to_host;

float gyroScale, accelScale,magScale;

extern uint8_t mpu_init_count;

volatile uint32_t time_before, time_after;

int timespan;
struct sLSM9DS0_Data tLSM9SD0; // telemetry block
//extern bool green_led_On;

extern char lsm_ax[6];
extern char lsm_ay[6];
extern char lsm_az[6];

extern char lsm_gx[6];
extern char lsm_gy[6];
extern char lsm_gz[6];

extern char lsm_mx[6];
extern char lsm_my[6];
extern char lsm_mz[6];

extern bool record_to_flash;
extern bool write_one_row_to_flash;
extern bool request_to_delete_flash;

volatile extern char     flash_page_buf[];
volatile extern uint16_t flash_page_index;

volatile extern unsigned long NextPageAvailable;

//extern unsigned long gpsRowsRecorded;
extern unsigned long lsmRowsRecorded;
extern unsigned long varRowsRecorded;

extern volatile uint32_t time_stamp;

extern volatile unsigned long pagesWrittenToFlash;

extern uint8_t current_sensor_id;

extern bool page_written_to_flash;

#define LSM_PACK_DATA_ID 0x21

#define LSM_BUFFER_LIMIT 177


//**********************
void clean_lsm_buffers()
{
	for (int ff = 0; ff < sizeof(lsm_ax); ff++)
	   lsm_ax[ff] = 0x20; //0x20;
	   
	for (int ff = 0; ff < sizeof(lsm_ay); ff++)
	   lsm_ay[ff] = 0x20;
	   
	for (int ff = 0; ff < sizeof(lsm_az); ff++)
	   lsm_az[ff] = 0x20;	
	   
	   
    for (int ff = 0; ff < sizeof(lsm_gx); ff++)
	   lsm_gx[ff] = 0x20;	
	   
    for (int ff = 0; ff < sizeof(lsm_gy); ff++)
	   lsm_gy[ff] = 0x20;
	   
	for (int ff = 0; ff < sizeof(lsm_gz); ff++)
	   lsm_gz[ff] = 0x20; 
	   
	   
    for (int ff = 0; ff < sizeof(lsm_mx); ff++)
	   lsm_mx[ff] = 0x20;	
	   
    for (int ff = 0; ff < sizeof(lsm_my); ff++)
	   lsm_my[ff] = 0x20;
	   
	for (int ff = 0; ff < sizeof(lsm_mz); ff++)
	   lsm_mz[ff] = 0x20; 
}


//**************************
void o_setAccelODR(uint8_t aRate)   //A_ODR_3125
{
	uint8_t temp = twi_read_one_reg(trcTwi, LSM9DS0_XM, CTRL_REG1_XM);           // We need to preserve the other bytes in CTRL_REG1_XM. So, first read it:
	temp &= 0xFF^(0xF << 4);                                                     // Then mask out the accel ODR bits:
	temp |= (aRate << 4);	                                                     // Then shift in our new ODR bits:
	twi_write_one_reg(trcTwi, LSM9DS0_XM_W, CTRL_REG1_XM, temp);   	             // And write the new register value back into CTRL_REG1_XM:
}



//**********************************
void o_setAccelScale(uint8_t aScl)
{
	uint8_t temp = twi_read_one_reg(trcTwi, LSM9DS0_XM, CTRL_REG2_XM);           // We need to preserve the other bytes in CTRL_REG2_XM. So, first read it:
	temp &= 0xFF^(0x3 << 3);                                                     // Then mask out the accel scale bits:
	temp |= aScl << 3;	                                                         // Then shift in our new scale bits:
	twi_write_one_reg(trcTwi, LSM9DS0_XM_W, CTRL_REG2_XM, temp);                 // And write the new register value back into CTRL_REG2_XM: 	
	
	// We've updated the sensor, but we also need to update our class variables
	// First update aScale:
	//aScale = aScl;
	// Then calculate a new aRes, which relies on aScale being set correctly:
	//calcaRes();
}


//**************
void o_initAccel()
{
	twi_write_one_reg(trcTwi, LSM9DS0_XM_W, CTRL_REG0_XM, 0x00);   
	twi_write_one_reg(trcTwi, LSM9DS0_XM_W, CTRL_REG1_XM, 0x57);   
	twi_write_one_reg(trcTwi, LSM9DS0_XM_W, CTRL_REG2_XM, 0x00); 
	twi_write_one_reg(trcTwi, LSM9DS0_XM_W, CTRL_REG3_XM, 0x04); 
	
	o_setAccelODR(0x06);          // 100 Hz
	o_setAccelScale(0x00);	
}


//**********************
void o_setMagODR(uint8_t mRate)
{
    uint8_t temp = twi_read_one_reg(trcTwi, LSM9DS0_XM, CTRL_REG5_XM);      // We need to preserve the other bytes in CTRL_REG5_XM. So, first read it:
	temp &= 0xFF^(0x7 << 2);                                                // Then mask out the mag ODR bits:
	temp |= (mRate << 2);	                                                // Then shift in our new ODR bits:
	twi_write_one_reg(trcTwi, LSM9DS0_XM_W, CTRL_REG5_XM, temp);            // And write the new register value back into CTRL_REG5_XM:
}


//****************************
void o_setMagScale(uint8_t mScl)
{
	uint8_t temp = twi_read_one_reg(trcTwi, LSM9DS0_XM, CTRL_REG6_XM);      // We need to preserve the other bytes in CTRL_REG6_XM. So, first read it:
	temp &= 0xFF^(0x3 << 5);	                                            // Then mask out the mag scale bits:
	temp |= mScl << 5;                                                      // Then shift in our new scale bits:
	twi_write_one_reg(trcTwi, LSM9DS0_XM_W, CTRL_REG6_XM, temp);            // And write the new register value back into CTRL_REG6_XM:  
	
	// We've updated the sensor, but we also need to update our class variables
	// First update mScale:
	//mScale = mScl;
	// Then calculate a new mRes, which relies on mScale being set correctly:
	//calcmRes();
}


//************
void o_initMag()
{	
	twi_write_one_reg(trcTwi, LSM9DS0_XM_W, CTRL_REG5_XM, 0x14);   
	twi_write_one_reg(trcTwi, LSM9DS0_XM_W, CTRL_REG6_XM, 0x00);   
	twi_write_one_reg(trcTwi, LSM9DS0_XM_W, CTRL_REG7_XM, 0x00);   
	twi_write_one_reg(trcTwi, LSM9DS0_XM_W, CTRL_REG4_XM, 0x04); 
	twi_write_one_reg(trcTwi, LSM9DS0_XM_W, INT_CTRL_REG_M, 0x09); 
	
	o_setMagODR(0x05);      // 100 Hz
	o_setMagScale(0x00);
}


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


//********
o_initGyro()
{
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


//**********************
// return true success else fail
//************************
bool lsm_init_check()
{
	uint8_t whoAmI_xm = twi_read_one_reg(trcTwi, LSM9DS0_XM, WHO_AM_I_XM);
	uint8_t whoAmI_g  = twi_read_one_reg(trcTwi, LSM9DS0_G,  WHO_AM_I_G);
	
	if (whoAmI_xm == LSM9DS0_XM_ID && whoAmI_g == LSM9DS0_GYRO_ID)
	{
		o_initGyro();
		o_initAccel();
		o_initMag();
		return true;
	}
	else
		return false;
}


//******************************************************
void send_lsm_data_to_host(int params_buf[], int fusize)
{
	uint8_t *cm;
	uint32_t length;
	int i;
	char buf_all[100];
	snprintf(buf_all,100,"%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;",
		params_buf[0],params_buf[1],params_buf[1],params_buf[3],params_buf[4],
		params_buf[5],params_buf[6],params_buf[7],params_buf[8],params_buf[9]
		);
	UartWrapSendByte(USART_SERIAL_PC, LSMDataToHost); 
	cm = UartWrap(buf_all,strlen(buf_all),&length);
	for (i=0;i<length;i++)
		usart_putchar(USART_SERIAL_PC, *cm++); 
} 



//***************************************************
void add_lsm_data_to_flash_buff(uint32_t futimestamp)
{		
	lsmRowsRecorded++;
}



//*****************
void get_lsm_data()
{
	uint8_t who = 0;
	uint8_t high_byte = 0;
	uint8_t low_byte = 0;
	char buf[10];
	int mpuVals[10];
	
	int16_t ax=0; int ay=0; int az=0; int te=0; int gx=0; int gy=0; int gz=0; int mx=0; int my=0; int mz=0;		
	current_sensor_id = 1;
	
	page_written_to_flash = false;
	
	//cpu_irq_disable();	
	//time_before = ms1Ticks;	
	//cpu_irq_enable();
	
	ax = get_two_bytes_data(trcTwi, LSM9DS0_XM, OUT_X_H_A, OUT_X_L_A);
	ay = get_two_bytes_data(trcTwi, LSM9DS0_XM, OUT_Y_H_A, OUT_Y_L_A);
	az = get_two_bytes_data(trcTwi, LSM9DS0_XM, OUT_Z_H_A, OUT_Z_L_A);
	
	mx = get_two_bytes_data(trcTwi, LSM9DS0_XM, OUT_X_H_M, OUT_X_L_M);
	my = get_two_bytes_data(trcTwi, LSM9DS0_XM, OUT_Y_H_M, OUT_Y_L_M);
	mz = get_two_bytes_data(trcTwi, LSM9DS0_XM, OUT_Z_H_M, OUT_Z_L_M);
	
	gx = get_two_bytes_data(trcTwi, LSM9DS0_G, OUT_X_H_G,  OUT_X_L_G);
	gy = get_two_bytes_data(trcTwi, LSM9DS0_G, OUT_Y_H_G,  OUT_Y_L_G);
	gz = get_two_bytes_data(trcTwi, LSM9DS0_G, OUT_Z_H_G,  OUT_Z_L_G);	
	
	//cpu_irq_disable();
	//time_after = ms1Ticks;
	//cpu_irq_enable();	
	
	if ((ax == 0) && (ay == 0) && (az == 0) && (mx == 0) && (my == 0) && (mz == 0) && (gx == 0) && (gy == 0) && (gz == 0))
	{
		return;
	}
	
	//timespan = 0;
	//if (time_after > time_before)
	  //timespan = time_after - time_before;
  	
	mpuVals[0] = ax;	
	mpuVals[1] = ay;
	mpuVals[2] = az;
	mpuVals[3] = gx;
	mpuVals[4] = gy;
	mpuVals[5] = gz;	
	mpuVals[6] = mx;
	mpuVals[7] = my;
	mpuVals[8] = mz;
	mpuVals[9] = timespan;
	
	if (!record_to_flash)
	{
	//   send_lsm_data_to_host(mpuVals, 10);	// DH temp turn off
	}
    else
	{
		tLSM9SD0.Accelerometer[0] = ax;
		tLSM9SD0.Accelerometer[1] = ay;
		tLSM9SD0.Accelerometer[2] = az;
		tLSM9SD0.Gyro[0] = gx;
		tLSM9SD0.Gyro[1] = gy;
		tLSM9SD0.Gyro[2] = gz;
		tLSM9SD0.Magnetometer[0] = mx;
		tLSM9SD0.Magnetometer[1] = my;
		tLSM9SD0.Magnetometer[2] = mz;
		TelemetryWriteToFlash(LSM9DS0_ALL,&tLSM9SD0);
		lsmRowsRecorded++;
	}	   	  
}



