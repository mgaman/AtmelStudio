/*
 * TwiEx1.c
 *
 * Created: 6/2/2014 5:50:20 PM
 *  Author: User
 */ 


#include "TwiEx1.h"
#include <util/delay.h>
#include <asf.h>
#include "Utilities.h"
#include "twi_common.h"


#define		TWI_interruptPriorityHIGH_bm		( 3 << 6 )

volatile int8_t xAvg	=	0x00;
volatile int8_t yAvg	=	0x00;
volatile int8_t zAvg	=	0x00;



volatile uint8_t TWI_receivedData = 0;
static volatile uint8_t readAddress = 0;


//------------------
void TWI_setup(void)
{
	//TWIC.MASTER.CTRLA	|=		( TWI_MASTER_ENABLE_bm ) ;							//	TWI_interruptPriorityHIGH_bm	);
	//TWIC.MASTER.CTRLB	|=		( TWI_MASTER_SMEN_bm	);							//	enable smart mode
	//TWIC.MASTER.CTRLB	|=		( 3 << 2 );											//	200us inactive timeout;
	//TWIC.MASTER.BAUD	 =			50;													//	approx. 350 kHz
	//TWIC.MASTER.STATUS	 =		TWI_MASTER_BUSSTATE_IDLE_gc;						//force initial state: IDLE
	
	
	sysclk_enable_peripheral_clock(&TWIC);
	OSC.CTRL|=OSC_RC32MEN_bm;                                         // Sets clock at 32Mhz
	while (!(OSC.STATUS & OSC_RC32MRDY_bm));
	CCP=CCP_IOREG_gc;
	CLK.CTRL=CLK_SCLKSEL_RC32M_gc;
	TWIC_MASTER_BAUD = 0x9B;                                          // Clock SCL = 100kHz with 32Mhz internal oscillator
	TWIC_MASTER_CTRLA = TWI_MASTER_ENABLE_bm;                         // Enable TWI Master
	TWIC.MASTER.CTRLB |= TWI_MASTER_SMEN_bm;                          // Enable smart mode
	TWIC_MASTER_STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;                 // Force bus state into idle
	while (!(TWIC_MASTER_STATUS & TWI_MASTER_BUSSTATE_IDLE_gc)){}	
}


//--------------------------------------
void TWI_singleByteRead(uint8_t address)
{
	readAddress = address;
	TWIC.MASTER.CTRLC	|=			( 1 << 2);					                         	// SM action == NACK
	TWIC.MASTER.CTRLA	|=			( TWI_MASTER_WIEN_bm | TWI_MASTER_RIEN_bm );              //|( 3 << 6 ));
	TWIC.MASTER.ADDR	 =			ACCELEROMETER_WRITE_ADDRESS;
}


//----------------------------
uint8_t TWI_polledWHOAMI(void)
{
	testStatusRegisters(0);

	TWIC.MASTER.CTRLC	|=			( 1 << 2);

	TWIC.MASTER.ADDR = ACCELEROMETER_WRITE_ADDRESS;	                 //testStatusRegisters(1);

	while (  ! ( TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm ) );         // waiting for writeComplete
	TWIC.MASTER.DATA = WHO_AM_I_ADDR;								 // send register address              //testStatusRegisters(2);

	while(  !  ( TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm ) );        // waiting for writeComplete
	TWIC.MASTER.ADDR = ACCELEROMETER_READ_ADDRESS;					 //Issue RepeatedStartConditon, and transmit device readAddress     //testStatusRegisters(3);

	while( !( TWIC.MASTER.STATUS & TWI_MASTER_RIF_bm ) );			//wait for readBit set
	uint8_t twivar = TWIC.MASTER.DATA;								//read recieved data.                                               //testStatusRegisters(4);

	// NOTE: SMART MODE enabled, and response set to send NACK after recieveing data.
	// This must be changed for multipleRead/write !!!
//	testReceivedByte(twivar,6);
	TWIC.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;   // force bus state to idle
	//testStatusRegisters(5);

	return twivar;
}


//----------------------------------------------
int8_t TWI_polledReadSingleByte(uint8_t address)
{
	//testStatusRegisters(0);

	TWIC.MASTER.CTRLC |= ( 1 << 2);

	TWIC.MASTER.ADDR = ACCELEROMETER_WRITE_ADDRESS;                 //testStatusRegisters(1);
	while (  ! ( TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm ) );       // waiting for writeComplete
	
	TWIC.MASTER.DATA = address;								       // send register address                  //testStatusRegisters(2);
	while(  !  ( TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm ) );       // waiting for writeComplete	
	
	TWIC.MASTER.ADDR = ACCELEROMETER_READ_ADDRESS;						//Issue RepeatedStartConditon, and transmit device readAddress    //testStatusRegisters(3);
	while( !( TWIC.MASTER.STATUS & TWI_MASTER_RIF_bm ) );			   //wait for readBit set
	uint8_t twivar = TWIC.MASTER.DATA;										//read received data.
	
	//testStatusRegisters(4);
	// NOTE: SMART MODE enabled, and response set to send NACK after recieveing data.
	//testReceivedByte(twivar,6);

	TWIC.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;   // force bus state to idle
	//testStatusRegisters(5);


	int8_t* c =  (int8_t*) &twivar;
	//testReceivedByte(*c,7);
	return *c;
}


//------------------------------------------------------------
void TWI_polledWriteSingleByte(uint8_t address, uint8_t data )
{
	//testStatusRegisters(0);

	TWIC.MASTER.CTRLC |= ( 1 << 2);

	TWIC.MASTER.ADDR = ACCELEROMETER_WRITE_ADDRESS;

	//testStatusRegisters(1);

	while (!( TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm ));          // waiting for writeComplete
	TWIC.MASTER.DATA = address;								      // send register address

	//testStatusRegisters(2);

	while(  !  ( TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm ) );       // waiting for writeComplete
	TWIC.MASTER.DATA = data;									   // send data

	//testStatusRegisters(3);

	while( !( TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm ) );			//wait for readBit set
	TWIC.MASTER.CTRLC = ( 3 << 0 );								    //send NACK and STOP condition
	//testStatusRegisters(4);

	TWIC.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;              // force bus state to idle
	//testStatusRegisters(5);
}



//--------------------------------------------
void testStatusRegisters ( uint8_t rowNumber )
{
	for ( uint8_t i = 0 ; i < 8 ; ++i )
	   if ( TWIC.MASTER.STATUS & ( 1 << i ))
	   {
		   	//DisplayData[i][rowNumber * 3] = 200;
			  // delay_ms(1);  // to compile
	   }

}


//------------------------------------------------------------
void testReceivedByte( uint8_t byteToTest, uint8_t rowNumber )
{
	for ( uint8_t i = 0 ; i < 8 ; ++i )
	{
		if ( byteToTest & ( 1 << i ))
		{
			
			
		}
		//DisplayData[i][(rowNumber * 3) +2 ] = 200;
		// delay_ms(1);
	}

}

