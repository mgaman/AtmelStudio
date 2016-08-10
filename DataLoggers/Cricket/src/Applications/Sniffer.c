/*
 * Sniffer.c
 *
 * Created: 1/6/2014 7:53:20 AM
 *  Author: pc
 */ 
#include <asf.h>
#include "Applications.h"
#include "RFHandler.h"
#include "Utilities.h"

RF_ENUM Ret = 0xff;

static void HardwareInit()
{
	HIPOWER_LED_ON;
	delay_s(2);
	HIPOWER_LED_OFF;
}
static void SoftwareInit()
{
	Ret = RFInitReceiver(0);
}

void sniffer()
{
	int j;
	uint8_t *cp;
	
	HardwareInit();
	SoftwareInit();
	
	while (true)
	{
		decodedPacket.header.packetLen = 0;
		RFReceiveMode_Handler(0);
		if (decodedPacket.header.packetLen != 0)
		{
			HIPOWER_LED_TOGGLE;
			
		 // simply send all data to uart
		//	for (j=0,cp = (uint8_t *)&decodedPacket.header.packetFlags;j<decodedPacket.header.packetLen;j++,cp++)  
		//		usart_putchar(USART_SERIAL_PC,*cp);
			}
	}
}