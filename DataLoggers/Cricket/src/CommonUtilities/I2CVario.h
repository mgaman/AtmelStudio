/*
 * I2CVario.h
 *
 * Created: 13-Nov-14 1:19:21 PM
 *  Author: User
 */ 


#ifndef I2CVARIO_H_
#define I2CVARIO_H_


#include "twi_master_driver.h"


#define SLAVE_ADDRESS  0xEC

/*! CPU speed 2MHz, BAUDRATE 100kHz and Baudrate Register Settings */
//#define CPU_SPEED   2000000
//#define CPU_SPEED   32000000
//#define BAUDRATE	100000
//#define TWI_BAUDSETTING TWI_BAUD(CPU_SPEED, BAUDRATE)

#define F_CPU 2000000UL

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


void vario_i2c_init(void);
//unsigned int cmd_prom(char coef_num);
unsigned char crc4(unsigned int n_prom[]); 
unsigned long cmd_adc(char cmd);


#endif /* I2CVARIO_H_ */