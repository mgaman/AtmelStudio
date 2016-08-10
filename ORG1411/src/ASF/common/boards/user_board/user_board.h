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
//-------------------- TRACKER -------------------------
/*
                                +--------------------+
                         LED3 A0|                    |B0 ADC8
                         LED2 A1|                    |B1 ADC9 VBAT
                              A2|    ATXMEGA1284U    |B2 POWER_LATCH
                      POW_INT A3|                    |B3
                    CHARGE_ON A4|                    |B4
                   GPS_ON_OFF A5|                    |B5
                   MODEM STAT A6|                    |B6
                    ENABLE 3V A7|                    |B7
                                |                    |
                              C0|                    |D0 red power led
                              C1|                    |D1 CPU_WAKEUP
                       BL_RTS C2|                    |D2 BL_RX
                       BL_CTS C3|                    |D3 BL_TX
          W25Q128FVWIG SPI SS C4|                    |D4
                         MOSI C5|                    |D5
                         MISO C6|                    |D6 USB D-
                         SCLK C7|                    |D7 USB D+
                                |                    |
   GPS LSM9DS0 MS5611 I2C SDA E0|                    |
                      I2C SCL E1|                    |
                     UART0_RX E2|                    |
                     UART0_TX E3|                    |
                              E4|                    |
                              E5|                    |
                              E6|                    |
                              E7|                    |
                                +--------------------+
*/
  
#define ONBOARD_LED_3		  IOPORT_CREATE_PIN(PORTA,0)
#define ONBOARD_LED_2		  IOPORT_CREATE_PIN(PORTA,1)
#define ACTIVATION_BUTTON     IOPORT_CREATE_PIN(PORTA,3)
#define GPS_ON_OFF_PIN		  IOPORT_CREATE_PIN(PORTA,5)
#define GPS_WAKEUP_PIN        IOPORT_CREATE_PIN(PORTA,6)

#define POWER_LATCH	          IOPORT_CREATE_PIN(PORTB,2)

#define SPI_SS		          IOPORT_CREATE_PIN(PORTC,4)
#define SPI_MISO		      IOPORT_CREATE_PIN(PORTC,5)
#define SPI_MOSI   		      IOPORT_CREATE_PIN(PORTC,6)
#define SPI_CLK               IOPORT_CREATE_PIN(PORTC,7)

#define TWI_DATA	IOPORT_CREATE_PIN(PORTE,0)
#define TWI_CLK		IOPORT_CREATE_PIN(PORTE,1)

#define UNUSED2     IOPORT_CREATE_PIN(PORTA,2)
#define UNUSED4     IOPORT_CREATE_PIN(PORTA,4)
#define UNUSED7     IOPORT_CREATE_PIN(PORTA,7)

#define UNUSED8     IOPORT_CREATE_PIN(PORTB,0)
#define UNUSED9     IOPORT_CREATE_PIN(PORTB,1)
#define UNUSED10    IOPORT_CREATE_PIN(PORTB,2)
#define UNUSED11    IOPORT_CREATE_PIN(PORTB,4)
#define UNUSED12    IOPORT_CREATE_PIN(PORTB,5)
#define UNUSED13    IOPORT_CREATE_PIN(PORTB,6)
#define UNUSED14    IOPORT_CREATE_PIN(PORTB,7)

#define UNUSED15    IOPORT_CREATE_PIN(PORTC,0)
#define UNUSED16    IOPORT_CREATE_PIN(PORTC,1)
#define UNUSED31    IOPORT_CREATE_PIN(PORTC,2)
#define UNUSED32    IOPORT_CREATE_PIN(PORTC,3)

#define UNUSED18    IOPORT_CREATE_PIN(PORTD,1)
#define UNUSED19    IOPORT_CREATE_PIN(PORTD,2)
#define UNUSED20    IOPORT_CREATE_PIN(PORTD,3)
#define UNUSED21    IOPORT_CREATE_PIN(PORTD,4)
#define UNUSED22    IOPORT_CREATE_PIN(PORTD,5)
#define UNUSED23    IOPORT_CREATE_PIN(PORTD,6)
#define UNUSED24    IOPORT_CREATE_PIN(PORTD,7)

#define UNUSED25    IOPORT_CREATE_PIN(PORTE,2)
#define UNUSED26    IOPORT_CREATE_PIN(PORTE,3)
#define UNUSED27    IOPORT_CREATE_PIN(PORTE,4)
#define UNUSED28    IOPORT_CREATE_PIN(PORTE,5)
#define UNUSED29    IOPORT_CREATE_PIN(PORTE,6)
#define UNUSED30    IOPORT_CREATE_PIN(PORTE,7)

#define TWI_ORG1411   TWIE
//#define TWI_MASTER_GPS  TWIE
#define TWI_PORT_GPS	PORTE
#define TWI_SPEED        400000

#define GPS_AS_SLAVE_ADDR	 0x60
#define GPS_AS_MASTER_ADDR	 0x62

#define _MAX_RX_FIFO_BUFFER_LENGTH  128 // see ASF documentation
#endif // USER_BOARD_H
