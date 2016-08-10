/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

// External oscillator settings.
// Uncomment and set correct values if external oscillator is used.

// External oscillator frequency
//#define BOARD_XOSC_HZ          8000000

// External oscillator type.
//!< External clock signal
//#define BOARD_XOSC_TYPE        XOSC_TYPE_EXTERNAL
//!< 32.768 kHz resonator on TOSC
//#define BOARD_XOSC_TYPE        XOSC_TYPE_32KHZ
//!< 0.4 to 16 MHz resonator on XTALS
//#define BOARD_XOSC_TYPE        XOSC_TYPE_XTAL

// External oscillator startup time
//#define BOARD_XOSC_STARTUP_US  500000

// see pinout.pdf
#define GPIO_0 IOPORT_CREATE_PIN(PORTA,0)
#define GPIO_1 IOPORT_CREATE_PIN(PORTA,1)
#define GPIO_2 IOPORT_CREATE_PIN(PORTA,2)
#define GPIO_3 IOPORT_CREATE_PIN(PORTA,3)
#define GPIO_4 IOPORT_CREATE_PIN(PORTD,1)
#define GPIO_5 IOPORT_CREATE_PIN(PORTD,0)
#define GPIO_6 IOPORT_CREATE_PIN(PORTC,2)
#define GPIO_7 IOPORT_CREATE_PIN(PORTC,1)
#define POWER_LATCH IOPORT_CREATE_PIN(PORTB,2)
// Specific uses
#define HIGH_POWER_LED GPIO_7
#define PUSH_BUTTON_1 GPIO_1
#define LED0 GPIO_0
#define LED1 GPIO_1

#define PUSH_BUTTON_2 GPIO_2
#define PUSH_BUTTON_3 GPIO_3
#define ON_BOARD_LED IOPORT_CREATE_PIN(PORTC,3)

/*
				Medium						Small
				+-----+      				+-----+
			VCC |     | GND	 				|     | pulldown output PC1
			RX  |     | GND	 				|     | VCC HP Led
			TX  |     | GND	 				|     | pulldown input PC2 push button 3
			GND |     | GND	 				|     | GND
			GND	|     | GND	 				|     | GND
			PA0	|     | GND	 				+---+-+
   push B 1 PA1 |     | PD1                     |
   push B 2 PA2 |     | PD0	 				    VCC
   push B 3 PA3 |     | PC2 		
			GND	|     | PC1 		
				+-----+		 		
                        PC3 on board led
*/
#endif // USER_BOARD_H
