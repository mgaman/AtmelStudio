/*
 * I2CUtils.c
 *
 * Created: 7/1/2014 2:04:39 PM
 *  Author: User
 */ 


#include <string.h>
#include <math.h>
#include <asf.h>
#include "Utilities.h"
#include "I2CUtils.h"


extern uint8_t current_sensor_id;


//***************************
void I2C_Init(TWI_t *twiname)
{
	sysclk_enable_peripheral_clock(twiname);
	
	OSC.CTRL|=OSC_RC32MEN_bm;                                             // Sets clock at 32Mhz  
	//OSC.CTRL|= OSC_RC2MEN_bm;            // Sets clock at 2Mhz  
	
	while (!(OSC.STATUS & OSC_RC32MRDY_bm));
	CCP=CCP_IOREG_gc;
	CLK.CTRL=CLK_SCLKSEL_RC32M_gc;
	
  twiname->MASTER.BAUD = 35;                                           // Clock SCL = 400kHz - GPS does not return data
	//twiname->MASTER.BAUD = 0x9B;                                          // Clock SCL = 100kHz with 32Mhz internal oscillator
	twiname->MASTER.CTRLA = TWI_MASTER_ENABLE_bm;                         // Enable TWI Master
	twiname->MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;                 // Force bus state into idle
	
	while (!(twiname->MASTER.STATUS & TWI_MASTER_BUSSTATE_IDLE_gc)){}
}



//*****************
void wait_for_wif()
{             
	cpu_irq_disable();
	i2c_wait_flag_count = 50;
	i2c_wait_finished = false;
	cpu_irq_enable();
	
	//while((!(TWIE.MASTER.STATUS & 0b01000000)) && (!i2c_wait_finished));
	
	while(!(TWIE.MASTER.STATUS & 0b01000000))
	{
		cpu_irq_disable();
		if (i2c_wait_finished)
		{
			cpu_irq_enable();
			break;
		}
		cpu_irq_enable();
	}
}



//*****************
void wait_for_rif()
{                       
    cpu_irq_disable();
    i2c_wait_flag_count = 50;
	i2c_wait_finished = false;
    cpu_irq_enable();
	
	//while((!(TWIE.MASTER.STATUS & 0b10000000)) && (!i2c_wait_finished));
	while(!(TWIE.MASTER.STATUS & 0b10000000))
	{
		cpu_irq_disable();
		if (i2c_wait_finished)
		{
			cpu_irq_enable();
			break;
		}
		cpu_irq_enable();
	}
}



//*****************************************************************************
uint8_t twi_read_one_reg(TWI_t *twiname, uint8_t slaveTwiAddr, uint8_t regAddr)
{
	uint8_t ret = 0;
	
	twiname->MASTER.ADDR = (slaveTwiAddr<<1);                                    //slave write address
    wait_for_wif();

	twiname->MASTER.DATA = regAddr;                                              //register address
	wait_for_wif();
	
	twiname->MASTER.ADDR = (slaveTwiAddr<<1) | 0x01;                             //slave read address
    wait_for_rif();

 	twiname->MASTER.CTRLC = 0x06;                                                //NACK, indicating that we are done reading
	uint8_t fudata = twiname->MASTER.DATA;
//	twiname->MASTER.CTRLC = 0x06;
	
    //twiname->MASTER.CTRLC = 0x03;                                                //send a STOP command   
    //twiname->MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;       //send a ACK and STOP command  
	//twiname->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc; 
	
	//if (current_sensor_id == 1)
	//{
		delay_us(50);
	//}
	
	return (fudata);
}



//**********************************************************************************
uint16_t twi_read_ms5611_coef(TWI_t *twiname, uint8_t slaveTwiAddr, uint8_t regAddr)
{
	uint16_t fudata = 0;
	
	twiname->MASTER.ADDR = (slaveTwiAddr<<1);                 //slave write address
	//while(! (TWIE.MASTER.STATUS & 0b01000000));               //wait for WIF after slave_address write - twi_read_ms5611_coef
	wait_for_wif();

	twiname->MASTER.DATA = regAddr;                           //register address
	//while(! (TWIE.MASTER.STATUS & 0b01000000));               //wait for WIF after reg_address write - twi_read_ms5611_coef
	wait_for_wif();
	
	twiname->MASTER.ADDR = (slaveTwiAddr<<1) | 0x01;          //slave read address
	//while(! (TWIE.MASTER.STATUS & 0b10000000));               //wait for RIF - twi_read_ms5611_coef
	wait_for_rif();

 	twiname->MASTER.CTRLC = 0x06;                           //NACK, indicating that we are done reading
	
    fudata  = twiname->MASTER.DATA;
    twiname->MASTER.CTRLC = TWI_MASTER_ACKACT_bm;
	
	uint8_t fudata2 = twiname->MASTER.DATA;
    twiname->MASTER.CTRLC = TWI_MASTER_ACKACT_bm;
	
	fudata = (fudata<<8) | fudata2;

    delay_ms(1); 
	
	return (fudata);
}



//********************************************************************************
uint32_t twi_read_ms5611_pt(TWI_t *twiname, uint8_t slaveTwiAddr, uint8_t regAddr)
{
	uint32_t ret = 0;
	
	twiname->MASTER.ADDR = 0xEC /*(slaveTwiAddr<<1)*/;                 //slave write address
	wait_for_wif();

	twiname->MASTER.DATA = regAddr;                           //register address
	wait_for_wif();

	twiname->MASTER.CTRLC = 0x03;                             //send a stop command 
	
	delay_ms(10);
	
 //------------------------------------------------- 
 
    twiname->MASTER.ADDR = 0xEC /*(slaveTwiAddr<<1)*/;                //slave write address
    wait_for_wif();  
		
	twiname->MASTER.DATA = 0x00;                              //from ms5611 example
	wait_for_wif();
	
	twiname->MASTER.ADDR = 0xEC | 0x01/*(slaveTwiAddr<<1) | 0x01*/;          //slave read address
	wait_for_rif();

   // twiname->MASTER.CTRLC = 0x06;                             //NACK, indicating that we are done reading
	uint8_t pt1 = twiname->MASTER.DATA;
	twiname->MASTER.CTRLC = TWI_MASTER_ACKACT_bm;
	uint8_t pt2 = twiname->MASTER.DATA;
	twiname->MASTER.CTRLC = TWI_MASTER_ACKACT_bm;
	uint8_t pt3 = twiname->MASTER.DATA;
	
	twiname->MASTER.CTRLC = 0x06;                             //NACK, indicating that we are done reading
	
	ret = ((uint32_t)pt1 << 16) | ((uint32_t)pt2 << 8) | pt3;	
	
	return (ret);
}



//**********************************************************************************************
void twi_write_one_reg(TWI_t *twiname, uint8_t slaveTwiAddr, uint8_t regAddr, uint8_t writeData)
{
	uint8_t i;
	uint8_t ret = 0;
	
	twiname->MASTER.ADDR = slaveTwiAddr;                         //slave write address
	wait_for_wif();
	
	twiname->MASTER.DATA = regAddr;                              //register address
    wait_for_wif();
	
	twiname->MASTER.DATA = writeData;		                     //data to write
    wait_for_wif();
	
	//delay_us(50);
	delay_us(10);

	return;
}


//***************************************************************************************************
int get_two_bytes_data(TWI_t *twiname, uint8_t slaveTwiAddr, uint8_t regHighAddr, uint8_t regLowAddr)
{
	int ret = 0;
	uint8_t high_byte = 0;
	uint8_t low_byte  = 0;
	
	high_byte = twi_read_one_reg(twiname, slaveTwiAddr, regHighAddr);
	low_byte  = twi_read_one_reg(twiname, slaveTwiAddr, regLowAddr);
	
	ret = (high_byte<<8) | low_byte;

	return(ret);
}


//*************************************************************************************************************
uint16_t get_uint_two_bytes_data(TWI_t *twiname, uint8_t slaveTwiAddr, uint8_t regHighAddr, uint8_t regLowAddr)
{
	uint16_t ret = 0;
	uint8_t high_byte = 0;
	uint8_t low_byte  = 0;
	
	high_byte = twi_read_one_reg(twiname, slaveTwiAddr, regHighAddr);
	low_byte  = twi_read_one_reg(twiname, slaveTwiAddr, regLowAddr);
	
	ret = (high_byte<<8) | low_byte;

	return(ret);
}



//**************************************************************************************************************
void writeBits(TWI_t *twiname, uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data)
{
	//      010 value to write
	// 76543210 bit numbers
	//    xxx   args: bitStart=4, length=3
	// 00011100 mask byte
	// 10101111 original value (sample)
	// 10100011 original & ~mask
	// 10101011 masked | value
	
	uint8_t b = twi_read_one_reg(twiname, devAddr, regAddr);
	uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
	data <<= (bitStart - length + 1);                               // shift data into correct position
	data &= mask;                                                   // zero all non-important bits in data
	b &= ~(mask);                                                   // zero all important bits in existing byte
	b |= data;
	twi_write_one_reg(twiname, devAddr, regAddr, b);
	
	// combine data with existing byte
	//return writeByte(devAddr, regAddr, b);
}


//---------------------------------------------------------------------------
void writeBit(TWI_t *twiname, uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data)
{
	//	readByte(devAddr, regAddr, &b);

	uint8_t b = twi_read_one_reg(twiname, devAddr, regAddr);
	b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	twi_write_one_reg(twiname, devAddr, regAddr, b);
	
	//	return writeByte(devAddr, regAddr, b);
}



////-------------------------------------------
//uint8_t twi_write(uint8_t addr, uint8_t data)
//{
	//TWIE.MASTER.ADDR = addr;
	//while(! (TWIE.MASTER.STATUS & 0b01000000));
	//TWIE.MASTER.DATA = data;
	//while(! (TWIE.MASTER.STATUS & 0b01000000));
	//return 0;
//}
//
//
////-------------------------------------------
//uint8_t twi_read(uint8_t addr, uint8_t *data)
//{
	//TWIE.MASTER.ADDR = addr;
	//while(! (TWIE.MASTER.STATUS & 0b10000000));  //wait for RIF
//
	//*data = TWIE.MASTER.DATA;
	//TWIE.MASTER.CTRLC = 0x06;                             //NACK, indicating that we are done reading
	//
	//uint8_t fudata = TWIE.MASTER.DATA;
	//
	////TWIE.MASTER.CTRLC = 0x03;                            // send a stop command
	//
	//return (fudata);
//}		
		
		

		
  //twi_write(ADDR_MPU_W, MPU6050_ACCEL_XOUT_H);
  //twi_read(ADDR_MPU_R, &who);
  //uint8_t ax_high = (uint8_t)who;
  //
  //twi_write(ADDR_MPU_W, MPU6050_ACCEL_XOUT_L);
  //twi_read(ADDR_MPU_R, &who);
  //uint8_t ax_low = (uint8_t)who;
  
  	//data[0] = BMP180_REG_CONTROL;
  	//data[1] = BMP180_COMMAND_TEMPERATURE;
  	
  	//twi_write(ADDR_BMP_W, BMP180_REG_CONTROL);
  	//twi_read(ADDR_BMP_R, &who);
  	//fhigh = who;