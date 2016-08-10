/*
 * uart.h
 *
 */ 


#ifndef UART_H_
#define UART_H_

#ifdef __cplusplus
extern "C"
{
	#include "usart_driver.h"
}
#endif

void uart_init(void);
void uart_sendString(char *text);

#endif /* UART_H_ */