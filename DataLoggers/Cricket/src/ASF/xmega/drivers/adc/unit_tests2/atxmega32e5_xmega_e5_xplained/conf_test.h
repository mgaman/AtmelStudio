/**
 * \file
 *
 * \brief Unit test configuration
 *
 * Copyright (C) 2013 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#ifndef CONF_TEST_H
#define CONF_TEST_H

/* ! \note USART0 on PORTD (RX on PD6, TX on PD7) */
#define CONF_TEST_USART      &USARTD0
/* ! \note 38.4 kbaud */
#define CONF_TEST_BAUDRATE   38400
/* ! \note 8-bit character length */
#define CONF_TEST_CHARLENGTH USART_CHSIZE_8BIT_gc
/* ! \note No parity check */
#define CONF_TEST_PARITY     USART_PMODE_DISABLED_gc
/* ! \note No extra stopbit, i.e., use 1 */
#define CONF_TEST_STOPBITS   false

/* ! \note Maximum acceptable deviation for measurements in standard mode */
#define CONF_TEST_ACCEPT_DELTA             200

/* ! \note Maximum acceptable delta for measurements between
 * several values in averaging mode (unit mV)
 */
#define CONF_TEST_ACCEPT_DELTA_AVERAGING   4

/* ! \note Maximum acceptable deviation for measurements in averaging
 * and offset/gain corrections mode (unit mV)
 */
#define CONF_TEST_ACCEPT_DELTA_CORRECTION  6

#endif
