/*
 * uart.cpp
 *
 */ 

/*! Define that selects the Usart used in example. */
#define USART USARTD1


#ifdef __cplusplus
extern "C"
{
	#include "usart_driver.h"
}
#endif


void uart_init()
{
		/* Variable used to send and receive data. */
	uint8_t sendData;
	uint8_t receivedData;
		
	/* This PORT setting is only valid to USARTC0 if other USARTs is used a
	 * different PORT and/or pins is used. */
	/* PD7 (TXD1) as output. */
	PORTD.DIRSET = PIN7_bm;

	/* PD6 (RXD1) as input. */
	//PORTD.DIRCLR = PIN6_bm;

	/* USARTC0, 8 Data bits, No Parity, 1 Stop bit. */
	USART_Format_Set(&USART, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);

	/* Set Baudrate to 9600 bps:
	 * Use the default I/O clock fequency that is 2 MHz.
	 * Do not use the baudrate scale factor
	 *
	 * Baudrate select = (1/(16*(((I/O clock frequency)/Baudrate)-1)
	 *                 = 12
	 */
	USART_Baudrate_Set(&USART, 12 , 0);

	/* Enable both RX and TX. */
	//USART_Rx_Enable(&USART);
	USART_Tx_Enable(&USART);
}

void sendChar(char c)
{
	do{
		/* Wait until it is possible to put data into TX data register.
		 * NOTE: If TXDataRegister never becomes empty this will be a DEADLOCK. */
	}while(!USART_IsTXDataRegisterEmpty(&USART));
	USART_PutChar(&USART, c);
}

void uart_sendString(char *text)
{
	while(*text)
	{
		sendChar(*text++);
	}
}
