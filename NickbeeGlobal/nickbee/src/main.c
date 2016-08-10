/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include "EEPROM.h"
#include "Parser.h"
#include "Application.h"
extern struct sMessage UartParsedMessage;
extern volatile  unsigned UartParseBufferFill;
extern volatile  unsigned UartParseBufferEmpty;
uint8_t *msgptr;
int main (void)
{
	cpu_irq_disable();
#if 0
	eeprom.NodeID = 1;
	eeprom.IamRemote = true;
//	eeprom.project = Demo;
//	eeprom.project = Firefly;
	eeprom.project = Yuvalit;
	eeprom.MessageRepeatCount = 5;
//	eeprom.project = Yuvalit;
	eeprom.FireflyGroup = GROUP_1;
	eeprom.FireflyBlinkCount = 5;
#ifdef USE_RTC
	eeprom.MessageRepeatDelay = 2;
	eeprom.FireflyBlinkOffPeriod = 1; // sec
	eeprom.FireflyBlinkOnPeriod = 1;
	eeprom.TxTimeout = 5; // sec	
#else
	eeprom.MessageRepeatDelay = 2000;
	eeprom.FireflyBlinkOffPeriod = 1000; // msec
	eeprom.FireflyBlinkOnPeriod = 1000;
	eeprom.TxTimeout = 5000; // msec
#endif
	WriteEEPROM(EEPROM_PARAMETER_PAGE,0,(uint8_t *)&eeprom,sizeof(struct sEEProm));
#else
	ReadEEPROM(EEPROM_PARAMETER_PAGE,0,(uint8_t *)&eeprom,sizeof(struct sEEProm));
#endif
	sysclk_init();
	board_init();
#if 0
	UartParseInit();
	cpu_irq_enable();
	// setup application
	appInit();
	// test parser
	while (true)
	{
		if (UartParseDataAvailable())
		{
			if (UartParse(UartParseGet()))
				msgRx(UART,&UartParsedMessage);
		}
	}
#else
	cpu_irq_enable();
	while (true){
		if (gpio_pin_is_high(PUSH_BUTTON_3))
		{
			gpio_set_pin_high(LED0);
			gpio_set_pin_low(LED1);
		}
		else
		{
			gpio_set_pin_high(LED0);
			gpio_set_pin_low(LED1);
			
		}
	}
#endif
}
