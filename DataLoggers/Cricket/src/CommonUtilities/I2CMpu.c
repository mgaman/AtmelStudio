/*
 * I2CMpu.c
 *
 * Created: 7/1/2014 2:36:17 PM
 *  Author: User
 */ 



#include <string.h>
#include <math.h>
#include <asf.h>
#include "Utilities.h"
#include "I2CUtils.h"
#include "I2CMpu.h"
#include "conf_board.h"


extern TWI_t * trcTwi;
extern uint8_t info_to_send_to_host;

float gyroScale, accelScale,magScale;

extern uint8_t mpu_init_count;

//*********************************
void setClockSource(uint8_t source)
{
	writeBits(trcTwi, MPU6050_ADDR, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}



//***************************************
void setFullScaleGyroRange(uint8_t range)
{
	writeBits(trcTwi, MPU6050_ADDR, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}



//****************************************
void setFullScaleAccelRange(uint8_t range)
{
	writeBits(trcTwi, MPU6050_ADDR, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}



//********************************
void setSleepEnabled(bool enabled)
{
	writeBit(trcTwi, MPU6050_ADDR, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}



//****************************************
void mpu_setI2CBypassEnabled(bool enabled)
{
	writeBit(trcTwi, MPU6050_ADDR, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}


//*********************
void mpu_initialize_1()
{
	setClockSource(MPU6050_CLOCK_PLL_XGYRO);
	setFullScaleGyroRange(MPU6050_GYRO_FS_250);
	setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
	setSleepEnabled(false);                              // thanks to Jack Elston for pointing this one out!
}


//******************************
void set_accel_scale(int scale)
{
	//accelScale = ((2000 * scale) / float(0xFFFF));
	
	if (scale == 2)  scale = 0;
	else 
	if (scale == 4)	 scale = 8;
	else 
	if (scale == 8)  scale = 16;
	else 
	if (scale == 16) scale = 24;

    twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x1C, scale);   // accel config
}


//****************************
void set_gyro_scale(int scale)
{
	//gyroScale = ((2 * scale) / float(0xFFFF));
	
	if (scale == 250) scale = 0;
	else 
	if (scale == 500) scale = 8;
	else 
	if (scale == 1000) scale = 16;
	else 
	if (scale == 2000) scale = 24;

    twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x1B, scale);   // gyro config
}


//********************************
void mpu_initialize_refresh_dude()
{
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6b, 0x01);   // sleep to 0 and clock to X axis gyro
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6a, 0x00);   // disable i2c master
 // twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x37, 0x00);   // disable i2c passthrough	
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x1b, 0x00);   // gyro  config
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x1c, 0x00);   // accel config	
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x19, 0x00);   // desired sample rate divider
	
  //twi_write_one_reg(trcTwi, ADDR_MAG_W, 0x0A, 0x01);   // set mag
}


//*********************
void mpu_initialize_3()
{
    twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6B, 0x00);   // wake up and clock to internal 8MHz oscilator 	
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6A, 0x00);   // disable i2c master
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x1B, 0x00);   // gyro  config                 // set_gyro_scale(250);
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x1C, 0x00);   // accel config	             // set_accel_scale(2);	
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x37, 0x02);   // bypassI2C enable
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x1A, 0x06);   // Config FSYNC and DLPF to 5Hz	
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6C, 0x00);   // Disable MPU6050 standby mode

	twi_write_one_reg(trcTwi, ADDR_MAG_W, 0x0A, 0x01);   // set mag
}


//*********************
void mpu_initialize_4()
{
  //twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6B, 0x00);   // Clock source is Internal 8MHz oscillator
    twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6B, 0x01);   // Clock source is PLL with X axis gyroscope reference
  //twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6B, 0x02);   // Clock source is PLL with Y axis gyroscope reference	
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6A, 0x01);   // Disable master mode and clear all signal paths
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x1B, 0x18);   // Full gyro range
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x1C, 0x08);   // 4G accel range	
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x37, 0x02);   // bypassI2C
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x1A, 0x03);   // 42 Hz LPF
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6C, 0x00);   // Disable MPU6050 standby mode

	//twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x19, 0x04);   // Set SMPLRT_DIV to 0x04; this gives a 200 Hz sample rate when using the DLPF	
	//twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x38, 0x01);   // Data ready interrupt enabled	
	//twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x37, 0x32);   // Interrupts pin stays high until cleared, cleared on any read, I2C bypass
	
	twi_write_one_reg(trcTwi, ADDR_MAG_W, 0x0A, 0x01);   // set mag
}



void mpu_initialize_5()
{
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6B, 0x80);   // step1. The register 107 is set to 0x80, do a while until reset is done.
    twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x19, 0x18);    // step2. The register 25 is set to 0x18.
    twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x1C, 0x00);    // step3. The register 28 is set to 0x00. 
    twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x1A, 0x04);    // step4. The register 26 is set to 0x04.
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x38, 0x01);    // step5. The register 56 is set to 0x01.
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6B, 0x20);    // step6. The register 107 is set to 0x20.
	twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6C, 0xC7);    // step7. The register 108 is set to 0xC7.
}



//**********************
uint8_t mpu_init_check()
{
	uint8_t whoAmI = twi_read_one_reg(trcTwi, MPU6050_ADDR, MPU_WHO_AM_I);
	if (whoAmI != 0x68)
	{
		return(1);
	}
	
	//uint8_t howAmI = twi_read_one_reg(trcTwi, MPU6050_ADDR, MPU6050_PWR_MGMT_1);
	//if (howAmI != 0)
	//{
		//twi_write_one_reg(trcTwi, ADDR_MPU_W, MPU6050_PWR_MGMT_1, 0);
		//howAmI = twi_read_one_reg(trcTwi, MPU6050_ADDR, MPU6050_PWR_MGMT_1);
		//if (howAmI != 0)
		//{
			//return(2);
		//}
	//}
	
	
	//mpu_initialize_1();
	//mpu_initialize_refresh_dude();
    //mpu_initialize_3();	
	
	mpu_initialize_4();
	
	//mpu_initialize_5();
	delay_s(1);
	
	return(0);
}



//**********************************************
void send_mpu_info(int params_buf[], int fusize)
{
	char buf[10];
	char buf_all[110];
	
	for (int ff = 0; ff < sizeof(buf_all); ff++)
	buf_all[ff] = 0x30;
	
	
	int buff_all_indx = 0;
	for (int i = 0; i < fusize; i++)
	{
		for (int ff = 0; ff < sizeof(buf); ff++)
		   buf[ff] = 0x30;
		
		int tmpval = params_buf[i];
		itoa(tmpval, buf, 10);
		for (int ff = 0; ff < sizeof(buf); ff++)
		{
			buf_all[buff_all_indx] = buf[ff];
			buff_all_indx++;
		}
		
		buf_all[buff_all_indx] = 0x3B;
		buff_all_indx++;
	}
	
	
//	for (int ff = 0; ff < sizeof(buf_all); ff++)           // For test
//	   usart_putchar(USART_SERIAL_PC, buf_all[ff]);      // For test
}



//*****************
void get_mpu_data()
{
	uint8_t who = 0;
	uint8_t high_byte = 0;
	uint8_t low_byte = 0;
	char buf[10];
	int mpuVals[10];
	
	int ax=0; int ay=0; int az=0; int te=0; int gx=0; int gy=0; int gz=0; int mx=0; int my=0; int mz=0;	
	
  //  twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x37, 0x00);                       // bypassI2C disable 
  //  uint8_t f1 = twi_read_one_reg(trcTwi, MPU6050_ADDR, 0x02);  

  //  twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x37, 0x02);                       // bypassI2C enable 
  //  uint8_t f2 = twi_read_one_reg(trcTwi, AK8975_ADDR,  0x02);
  
  //  mpu_initialize_refresh_dude();
  
	//if (mpu_init_count == 2)
	//{
	   //mpu_init_count = 0;
		//
	   //twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x68, 0x07);            // a, t, g reset
	  ////  twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6B, 0x80);            // device reset
	  ////  twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6B, 0x00);            // wake, clock
		//
	   //mpu_initialize_4();
	//}
		
	// uint8_t f1 = twi_read_one_reg(trcTwi, MPU6050_ADDR, 0x6B);  	
	// uint8_t f1 = twi_read_one_reg(trcTwi, MPU6050_ADDR, 0x72);  
	// uint8_t f2 = twi_read_one_reg(trcTwi, MPU6050_ADDR, 0x73);  
	
  
    //for (int i=0; i < 5; i++)
	//{
    ax = get_two_bytes_data(trcTwi, MPU6050_ADDR, MPU6050_ACCEL_XOUT_H, MPU6050_ACCEL_XOUT_L);
	ay = get_two_bytes_data(trcTwi, MPU6050_ADDR, MPU6050_ACCEL_YOUT_H, MPU6050_ACCEL_YOUT_L);
	az = get_two_bytes_data(trcTwi, MPU6050_ADDR, MPU6050_ACCEL_ZOUT_H, MPU6050_ACCEL_ZOUT_L);
	te = get_two_bytes_data(trcTwi, MPU6050_ADDR, MPU6050_TEMP_OUT_H,   MPU6050_TEMP_OUT_L);
	gx = get_two_bytes_data(trcTwi, MPU6050_ADDR, MPU6050_GYRO_XOUT_H,  MPU6050_GYRO_XOUT_L);
	gy = get_two_bytes_data(trcTwi, MPU6050_ADDR, MPU6050_GYRO_YOUT_H,  MPU6050_GYRO_YOUT_L);
	gz = get_two_bytes_data(trcTwi, MPU6050_ADDR, MPU6050_GYRO_ZOUT_H,  MPU6050_GYRO_ZOUT_L);
	//}
	
	
	twi_write_one_reg(trcTwi, ADDR_MAG_W, 0x0A, 0x01);                                                // set mag
	delay_ms(10);
	
	mx = get_two_bytes_data(trcTwi, AK8975_ADDR, MPU9150_RA_MAG_XOUT_H, MPU9150_RA_MAG_XOUT_L);
	my = get_two_bytes_data(trcTwi, AK8975_ADDR, MPU9150_RA_MAG_YOUT_H, MPU9150_RA_MAG_YOUT_L);
	mz = get_two_bytes_data(trcTwi, AK8975_ADDR, MPU9150_RA_MAG_ZOUT_H, MPU9150_RA_MAG_ZOUT_L);
	
//	}
	
	mpuVals[0] = ax;
	mpuVals[1] = ay;
	mpuVals[2] = az;
	mpuVals[3] = te;
	mpuVals[4] = gx;
	mpuVals[5] = gy;
	mpuVals[6] = gz;
	mpuVals[7] = mx;
	mpuVals[8] = my;
	mpuVals[9] = mz;
	
	mpu_init_count++;
	
	delay_ms(1000);	

    if (info_to_send_to_host == 1)
	  send_mpu_info(mpuVals, 10);
	
  //twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6B, 0x80);      // reset	
  //twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x6A, 0x01);      // clear sensors reg data
  //twi_write_one_reg(trcTwi, ADDR_MPU_W, 0x68, 0x07);      // clear sensors signal path
}



