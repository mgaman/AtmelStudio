/*
 * TwiEx1.h
 *
 * Created: 6/2/2014 5:48:10 PM
 *  Author: User
 */ 


#ifndef TWIEX1_H_
#define TWIEX1_H_


#include <avr/io.h>


extern volatile int8_t xAvg;
extern volatile	int8_t yAvg;
extern volatile int8_t zAvg;
extern volatile uint8_t TWI_receivedData;

//#define		ACCELEROMETER_WRITE_ADDRESS			0x38
//#define		ACCELEROMETER_READ_ADDRESS			0x39
//#define		WHO_AM_I_VAL						0x2A
//#define		WHO_AM_I_ADDR						0x0D

#define MPU6050_ADDR          0x68
#define MPU6050_RA_WHO_AM_I   0x75
#define MPU6050_PWR_MGMT_1    0x6B
#define MPU6050_ACCEL_XOUT_H  0x3B

#define TWI_SLAVE_ADDR MPU6050_ADDR

#define		ACCELEROMETER_WRITE_ADDRESS		(TWI_SLAVE_ADDR<<1);
#define		ACCELEROMETER_READ_ADDRESS		(TWI_SLAVE_ADDR<<1)|0x01
#define		WHO_AM_I_VAL					0x2A
#define		WHO_AM_I_ADDR					MPU6050_RA_WHO_AM_I




void	TWI_setup					(	void	);
void	TWI_singleByteRead			(	uint8_t );
uint8_t TWI_polledWHOAMI			(	void	);
int8_t	TWI_polledReadSingleByte	(	uint8_t	);
void	TWI_polledWriteSingleByte	(	uint8_t address,	uint8_t data);

static void		testStatusRegisters			(	uint8_t	);
void	testReceivedByte			(	uint8_t byteToTest,	uint8_t	rowNumber	);

#endif /* TWIEX1_H_ */