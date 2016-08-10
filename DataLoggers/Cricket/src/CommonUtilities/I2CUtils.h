/*
 * I2CUtils.h
 *
 * Created: 7/1/2014 2:05:01 PM
 *  Author: User
 */ 


#ifndef I2CUTILS_H_
#define I2CUTILS_H_


//TWI_t * trcTwi = &TWIE;

void I2C_Init(TWI_t *twiname);
uint8_t twi_read_one_reg(TWI_t *twiname, uint8_t slaveTwiAddr, uint8_t regAddr);
int get_two_bytes_data(TWI_t *twiname, uint8_t slaveTwiAddr, uint8_t regHighAddr, uint8_t regLowAddr);
void twi_write_one_reg(TWI_t *twiname, uint8_t slaveTwiAddr, uint8_t regAddr, uint8_t writeData);
void writeBits(TWI_t *twiname, uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
void writeBit(TWI_t *twiname, uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);



uint16_t get_uint_two_bytes_data(TWI_t *twiname, uint8_t slaveTwiAddr, uint8_t regHighAddr, uint8_t regLowAddr);

uint16_t twi_read_ms5611_coef(TWI_t *twiname, uint8_t slaveTwiAddr, uint8_t regAddr);
uint32_t twi_read_ms5611_pt(TWI_t *twiname, uint8_t slaveTwiAddr, uint8_t regAddr);


#endif /* I2CUTILS_H_ */