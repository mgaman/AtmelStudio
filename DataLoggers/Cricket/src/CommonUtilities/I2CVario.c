/*
 * I2CVario.c
 *
 * Created: 13-Nov-14 1:17:56 PM
 *  Author: User
 */ 


#include <string.h>
#include <math.h>
#include <asf.h>

#include "I2CVario.h"

extern TWI_Master_t twiMaster;    



//*******************
void vario_i2c_init()
{
	PORTE.DIRSET = 0xFF;                                                               /* Initialize PORTE for output and PORTD for inverted input. */
	PORTCFG.MPCMASK = 0xFF;
	
	cli();
	//TWI_MasterInit(&twiMaster,	&TWIE,	TWI_MASTER_INTLVL_LO_gc,	TWI_BAUDSETTING);   /* Initialize TWI master. */
	PMIC.CTRL |= PMIC_LOLVLEN_bm;	                                                    /* Enable LO interrupt level. */
	sei();
}



////**********************************
//unsigned int cmd_prom(char coef_num) 
//{ 
	//unsigned int ret; 
	//unsigned int rC=0; 
	///*! Buffer with test data to send.*/
	//uint8_t sendBuffer[1] = {CMD_PROM_RD+coef_num*2};
//
	//TWI_MasterWriteRead(&twiMaster, SLAVE_ADDRESS >> 1, &sendBuffer[0], 1, 2);
	//while (twiMaster.status != TWIM_STATUS_READY);
	//rC = twiMaster.readData[0] * 256;
	//rC += twiMaster.readData[1];
//
	//return rC; 
//} 


////***************************************
//unsigned char crc4(unsigned int n_prom[])
//{
	//int cnt; // simple counter
	//unsigned int n_rem; // crc reminder
	//unsigned int crc_read; // original value of the crc
	//unsigned char n_bit;
	//n_rem = 0x00;
	//crc_read=n_prom[7]; //save read CRC
	//n_prom[7]=(0xFF00 & (n_prom[7])); //CRC byte is replaced by 0
	//for (cnt = 0; cnt < 16; cnt++) // operation is performed on bytes
	//{// choose LSB or MSB
		//if (cnt%2==1) n_rem ^= (unsigned short) ((n_prom[cnt>>1]) & 0x00FF);
		//else n_rem ^= (unsigned short) (n_prom[cnt>>1]>>8);
		//for (n_bit = 8; n_bit > 0; n_bit--)
		//{
			//if (n_rem & (0x8000))
			//{
				//n_rem = (n_rem << 1) ^ 0x3000;
			//}
			//else
			//{
				//n_rem = (n_rem << 1);
			//}
		//}
	//}
	//n_rem= (0x000F & (n_rem >> 12)); // final 4-bit reminder is CRC code
	//n_prom[7]=crc_read; // restore the crc_read to its original place
	//return (n_rem ^ 0x0);
//}


////*****************************
//unsigned long cmd_adc(char cmd)
//{
	//unsigned int ret;
	//unsigned long temp=0;
	//uint8_t sendBuffer[1] = {CMD_ADC_CONV+cmd};
	//TWI_MasterWriteRead(&twiMaster, SLAVE_ADDRESS >> 1, &sendBuffer[0], 1, 0);
	//switch (cmd & 0x0f) // wait necessary conversion time
	//{
		//case CMD_ADC_256 : _delay_us(900); break;
		//case CMD_ADC_512 : _delay_ms(3); break;
		//case CMD_ADC_1024: _delay_ms(4); break;
		//case CMD_ADC_2048: _delay_ms(6); break;
		//case CMD_ADC_4096: _delay_ms(10); break;
	//}
	//sendBuffer[0] = CMD_ADC_READ;
	//bool ok = TWI_MasterWriteRead(&twiMaster, SLAVE_ADDRESS >> 1, &sendBuffer[0], 1, 3);
	//while (twiMaster.status != TWIM_STATUS_READY);
	//if (ok)
	//{//issuing start condition ok, device accessible
		//ret = twiMaster.readData[0]; // read MSB and acknowledge
		//temp=65536*ret;
		//ret = twiMaster.readData[1]; // read byte and acknowledge
		//temp=temp+256*ret;
		//ret = twiMaster.readData[2]; // read LSB and not acknowledge
		//temp=temp+ret;
	//}
	//return temp;
//}


///*! TWIE Master Interrupt vector. */
//ISR(TWIE_TWIM_vect)
//{
	//TWI_MasterInterruptHandler(&twiMaster);
//}