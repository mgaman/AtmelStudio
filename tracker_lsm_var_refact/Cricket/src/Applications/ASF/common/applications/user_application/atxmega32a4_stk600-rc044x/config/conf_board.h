/**
 * \file
 *
 * \brief ATxmega32A4 on STK600 board configuration template
 *
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

// Enable on-board AT45DBX interface (SPI)
#define CONF_BOARD_AT45DBX

// Enable UART Communication Port interface (UART)
#define CONF_BOARD_ENABLE_USARTC0
#define CONF_BOARD_ENABLE_USARTC1
#define CONF_BOARD_ENABLE_USARTD0
#define CONF_BOARD_ENABLE_USARTD1
#define CONF_BOARD_ENABLE_USARTE0

#endif // CONF_BOARD_H
