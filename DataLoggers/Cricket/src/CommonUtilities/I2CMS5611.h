/*
 * I2CMS5611.h
 *
 * Created: 10/21/2014 1:52:18 PM
 *  Author: User
 */ 


#ifndef I2CMS5611_H_
#define I2CMS5611_H_


// addresses of the device
#define MS561101BA_ADDR_CSB_HIGH  0x76   //CBR=1 0x76 I2C address when CSB is connected to HIGH (VCC)
#define MS561101BA_ADDR_CSB_LOW   0x77   //CBR=0 0x77 I2C address when CSB is connected to LOW (GND)

#define MS561101BA_ADDR_CSB_HIGH_W (MS561101BA_ADDR_CSB_HIGH<<1)
#define MS561101BA_ADDR_CSB_LOW_W  (MS561101BA_ADDR_CSB_LOW<<1)

// registers of the device
#define MS561101BA_D1 0x40
#define MS561101BA_D2 0x50
#define MS561101BA_RESET 0x1E

#define MS5611_RA_NOP 0x01 

// D1 and D2 result size (bytes)
#define MS561101BA_D1D2_SIZE 3

// OSR (Over Sampling Ratio) constants
#define MS561101BA_OSR_256 0x00
#define MS561101BA_OSR_512 0x02
#define MS561101BA_OSR_1024 0x04
#define MS561101BA_OSR_2048 0x06
#define MS561101BA_OSR_4096 0x08

#define MS561101BA_PROM_BASE_ADDR 0xA2    // by adding ints from 0 to 6 we can read all the prom configuration values.

// C1 will be at 0xA2 and all the subsequent are multiples of 2
#define MS561101BA_PROM_REG_COUNT 6 // number of registers in the PROM
#define MS561101BA_PROM_REG_SIZE 2  // size in bytes of a prom registry.


uint8_t ms5611_init_check(void);
void get_ms5611_data(void);

#endif /* I2CMS5611_H_ */