/*
 * UartBluetooth.c
 *
 * Created: 10/1/2014 10:13:34 AM
 *  Author: User
 */ 


#include <string.h>
#include <stdio.h>
#include <math.h>
#include <asf.h>
#include "Utilities.h"
#include "conf_board.h"
#include "UartBluetooth.h"

extern char bluetoothBuffer[100];
extern uint8_t bluetoothBufferIndex;

extern char modemBuffer[100];
extern uint8_t modemBufferIndex;

const char check_bt[3]       = "AT\r";
const char bt_help[8]        = "AT+HELP\r";
const char bt_addr[8]        = "AT+ADDR\r";
const char bt_role[9]        = "AT+ROLE?\r";
const char bt_name[9]        = "AT+NAME?\r";
const char bt_ver[7]         = "AT+VER\r";
const char bt_scan[8]        = "AT+SCAN\r";
const char bt_devs[11]       = "AT+DEVLIST\r";
const char bt_advert[12]     = "AT+ADVERT=1\r";
const char bt_set_centr[10]  = "AT+ROLE=1\r";
const char bt_set_perif[10]  = "AT+ROLE=2\r";
const char bt_save[8]        = "AT+SAVE\r";
const char bt_reset[9]       = "AT+RESET\r";
const char bt_notify[12]     = "AT+NOTIFY=1\r";
const char bt_rssi[8]        = "AT+RSSI\r";
const char bt_auto_mode[11]  = "AT+PAIRM=0\r";
const char bt_echo_on[7]     = "AT+E=1\r";
const char bt_echo_off[7]    = "AT+E=0\r";



uint8_t get_addr[4] = {0x01, 0x09, 0x10 , 0x00}; 



//*****************
void bluetooth_id()
{
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(check_bt); j++)
	  usart_putchar(USART_SERIAL_MODEM, check_bt[j]);
	
	delay_ms(500);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_help); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_help[j]);
	
	delay_ms(500);	
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_addr); j++)
	   usart_putchar(USART_SERIAL_MODEM, bt_addr[j]);
	
	delay_ms(500);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_name); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_name[j]);
	
	delay_ms(500);	

	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_ver); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_ver[j]);
	
	delay_ms(500);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_role); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_role[j]);
	  
	delay_ms(500);
	 
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_notify); j++)
	 usart_putchar(USART_SERIAL_MODEM, bt_notify[j]);
	 
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_save); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_save[j]);
	 	
	delay_ms(500);
}



//************************
void bluetooth_as_periph()
{
  	modemBufferIndex = 0;
  	for (int j = 0; j < sizeof(bt_set_perif); j++)
  	  usart_putchar(USART_SERIAL_MODEM, bt_set_perif[j]);
	
	delay_ms(500);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_save); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_save[j]);
		
	delay_ms(500);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_reset); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_reset[j]);
		
	delay_ms(500);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_role); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_role[j]);
	
	delay_ms(500);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_advert); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_advert[j]);	 
	  
    delay_ms(500);	 
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_save); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_save[j]);
	
	delay_ms(500); 	 
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_auto_mode); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_auto_mode[j]);
	  
	delay_ms(500);
	 
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_save); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_save[j]);
	  	
	delay_ms(500);
	  	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_reset); j++)
	  	usart_putchar(USART_SERIAL_MODEM, bt_reset[j]);
	  	
	delay_ms(500);
}
	
	
	
//************************
void bluetooth_as_central()
{	  
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_set_centr); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_set_centr[j]);
	
	delay_ms(500);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_save); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_save[j]);
	
	delay_ms(500);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_reset); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_reset[j]);
	
	delay_ms(500);	  
	
    modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_role); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_role[j]);
		
	delay_ms(500);
}



//*************************
void bluetooth_first_test()
{
	//MODEM_ON_OFF_PIN_TO_HIGH;
	//while(true)
	//{
		//modemBufferIndex = 0;
		//for (int j = 0; j < sizeof(bt_addr); j++)
		  //usart_putchar(USART_SERIAL_MODEM, bt_addr[j]);
		//
		//delay_ms(3000);
	//}
	
	MODEM_ON_OFF_PIN_TO_LOW;	
    bluetooth_id();
	
  //bluetooth_as_central();
	bluetooth_as_periph();
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(bt_echo_off); j++)
	  usart_putchar(USART_SERIAL_MODEM, bt_echo_off[j]);
	
	while(true)
	{
		//modemBufferIndex = 0;
	    //for (int j = 0; j < sizeof(bt_scan); j++)
	       //usart_putchar(USART_SERIAL_MODEM, bt_scan[j]);
	  //
	    //delay_ms(5000);		
		//
		//modemBufferIndex = 0;
		//for (int j = 0; j < sizeof(bt_devs); j++)
	  	  //usart_putchar(USART_SERIAL_MODEM, bt_devs[j]);
		   
		  // usart_putchar(USART_SERIAL_MODEM, 'H');
		   //usart_putchar(USART_SERIAL_MODEM, 'I');
		   //usart_putchar(USART_SERIAL_MODEM, ' ');
		
		modemBufferIndex = 0;
		for (int j = 0; j < sizeof(bt_role); j++)
		   usart_putchar(USART_SERIAL_MODEM, bt_role[j]);	   
		   
		delay_ms(1000);
		
		//modemBufferIndex = 0;
		//for (int j = 0; j < sizeof(bt_rssi); j++)
		   //usart_putchar(USART_SERIAL_MODEM, bt_rssi[j]);
		   	//delay_ms(500);
	}
}