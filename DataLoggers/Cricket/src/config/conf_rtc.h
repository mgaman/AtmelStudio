/**
 * \file
 *
 * \brief RTC configuration
 *
 * Copyright (c) 2010-2012 Atmel Corporation. All rights reserved.
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
#ifndef CONF_RTC_H
#define CONF_RTC_H

//#define CONFIG_RTC_PRESCALER          RTC_PRESCALER_DIV1024_gc
#define CONFIG_RTC_COMPARE_INT_LEVEL  RTC_COMPINTLVL_LO_gc
#define CONFIG_RTC_OVERFLOW_INT_LEVEL RTC_OVFINTLVL_LO_gc


#if defined(YUVALIT_SLAVE) || defined(YUVALIT_MASTER) || defined(GAHLILIT_MASTER) || defined(GAHLILIT_SLAVE) 
#define RTC_TICKS_SEC 1
#elif defined(TRACKER_TEST)
#define RTC_TICKS_SEC 1024
#else
#define RTC_TICKS_SEC 64
#endif

#if RTC_TICKS_SEC == 1
#define	CONFIG_RTC_PRESCALER RTC_PRESCALER_DIV1024_gc;	// every 1024msec
#elif RTC_TICKS_SEC == 4
#define	CONFIG_RTC_PRESCALER RTC_PRESCALER_DIV256_gc;	// every 256msec
#elif RTC_TICKS_SEC == 16
#define	CONFIG_RTC_PRESCALER RTC_PRESCALER_DIV64_gc; // every 64msec
#elif RTC_TICKS_SEC == 64
#define	CONFIG_RTC_PRESCALER RTC_PRESCALER_DIV16_gc; // every 16msec
#elif RTC_TICKS_SEC == 128
#define	CONFIG_RTC_PRESCALER RTC_PRESCALER_DIV8_gc; // every 8msec
#elif RTC_TICKS_SEC == 512
#define	CONFIG_RTC_PRESCALER RTC_PRESCALER_DIV2_gc; // every 2msec
#elif RTC_TICKS_SEC == 1024
#define	CONFIG_RTC_PRESCALER RTC_PRESCALER_DIV1_gc; // every 1msec
#else
#error Invalid value for RTC_TICKS_SEC
#endif

#endif /* CONF_RTC_H */
