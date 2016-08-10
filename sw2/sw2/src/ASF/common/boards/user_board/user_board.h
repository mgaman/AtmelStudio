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
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

#define ONBOARD_LED_1		  IOPORT_CREATE_PIN(PORTA,0)
#define ONBOARD_LED_2		  IOPORT_CREATE_PIN(PORTA,1)
#define ACTIVATION_PIN		  3
#define ACTIVATION_BUTTON     IOPORT_CREATE_PIN(PORTA,ACTIVATION_PIN)
#define POWER_LATCH	          IOPORT_CREATE_PIN(PORTB,2)

#define LED1OFF  ioport_set_pin_level(ONBOARD_LED_1,LOW)
#define LED2OFF  ioport_set_pin_level(ONBOARD_LED_2,LOW)
#define LED1ON  ioport_set_pin_level(ONBOARD_LED_1,HIGH)
#define LED2ON  ioport_set_pin_level(ONBOARD_LED_2,HIGH)
#endif // USER_BOARD_H
