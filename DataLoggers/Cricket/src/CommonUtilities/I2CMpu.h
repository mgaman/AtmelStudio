/*
 * I2CMpu.h
 *
 * Created: 7/1/2014 2:36:37 PM
 *  Author: User
 */ 


#ifndef I2CMPU_H_
#define I2CMPU_H_

#define MPU6050_RA_WHO_AM_I 0x75
#define MPU6050_PWR_MGMT_1  0x6B

//Magnetometer Registers
#define MPU9150_RA_MAG_ADDRESS		0x0C
#define MPU9150_RA_MAG_XOUT_L		0x03
#define MPU9150_RA_MAG_XOUT_H		0x04
#define MPU9150_RA_MAG_YOUT_L		0x05
#define MPU9150_RA_MAG_YOUT_H		0x06
#define MPU9150_RA_MAG_ZOUT_L		0x07
#define MPU9150_RA_MAG_ZOUT_H		0x08

#define MPU6050_RA_INT_PIN_CFG      0x37

#define MPU6050_ACCEL_XOUT_H       0x3B   // R
#define MPU6050_ACCEL_XOUT_L       0x3C   // R
#define MPU6050_ACCEL_YOUT_H       0x3D   // R
#define MPU6050_ACCEL_YOUT_L       0x3E   // R
#define MPU6050_ACCEL_ZOUT_H       0x3F   // R
#define MPU6050_ACCEL_ZOUT_L       0x40   // R
#define MPU6050_TEMP_OUT_H         0x41   // R
#define MPU6050_TEMP_OUT_L         0x42   // R
#define MPU6050_GYRO_XOUT_H        0x43   // R
#define MPU6050_GYRO_XOUT_L        0x44   // R
#define MPU6050_GYRO_YOUT_H        0x45   // R
#define MPU6050_GYRO_YOUT_L        0x46   // R
#define MPU6050_GYRO_ZOUT_H        0x47   // R
#define MPU6050_GYRO_ZOUT_L        0x48   // R


#define MPU6050_CLOCK_PLL_XGYRO 0x01
#define MPU6050_GYRO_FS_250     0x00
#define MPU6050_ACCEL_FS_2      0x00
#define MPU6050_RA_PWR_MGMT_1   0x6B
#define MPU6050_PWR1_CLKSEL_BIT    2
#define MPU6050_PWR1_CLKSEL_LENGTH 3

#define MPU6050_RA_GYRO_CONFIG       0x1B
#define MPU6050_GCONFIG_FS_SEL_BIT      4
#define MPU6050_GCONFIG_FS_SEL_LENGTH   2

#define MPU6050_RA_ACCEL_CONFIG        0x1C
#define MPU6050_ACONFIG_AFS_SEL_BIT       4
#define MPU6050_ACONFIG_AFS_SEL_LENGTH    2

#define MPU6050_PWR1_SLEEP_BIT  6

#define MPU6050_INTCFG_I2C_BYPASS_EN_BIT  1

#define MPU6050_ADDR 0x68
//#define MPU6050_ADDR 0x69
#define AK8975_ADDR  0x0C

#define ADDR_MAG_W (AK8975_ADDR<<1)
#define ADDR_MAG_R (AK8975_ADDR<<1) | 0x01

#define ADDR_MPU_W (MPU6050_ADDR<<1)
#define ADDR_MPU_R (MPU6050_ADDR<<1) | 0x01

#define MPU_WHO_AM_I 0x75



void get_mpu_data(void);
uint8_t mpu_init_check(void);


#endif /* I2CMPU_H_ */