/*
 * I2CBmp.h
 *
 * Created: 7/1/2014 3:01:17 PM
 *  Author: User
 */ 


#ifndef I2CBMP_H_
#define I2CBMP_H_


#define BMP180_ADDR                0x77                   // 7-bit address
#define	BMP180_REG_CONTROL         0xF4
#define	BMP180_REG_RESULT          0xF6
#define	BMP180_COMMAND_TEMPERATURE 0x2E
#define	BMP180_COMMAND_PRESSURE0   0x34
#define	BMP180_COMMAND_PRESSURE1   0x74

#define	BMP180_COMMAND_PRESSURE2   0xB4
#define	BMP180_COMMAND_PRESSURE3   0xF4


#define ADDR_BMP_W (BMP180_ADDR<<1)
#define ADDR_BMP_R (BMP180_ADDR<<1) | 0x01


uint8_t bmp_init(void);
void get_bmp_data(void);
void new_get_bmp_data(void);

#endif /* I2CBMP_H_ */