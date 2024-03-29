/**
 * \file
 *
 * \brief Main functions for Mouse unit test
 *
 * Copyright (c) 2011 - 2014 Atmel Corporation. All rights reserved.
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

#include <stdio.h>
#include <asf.h>
#include <unit_test/suite.h>
#include <stdio_serial.h>
#include <conf_test.h>

/**
 * \mainpage ASF USB Device HID Mouse
 *
 * \section intro Introduction
 * This example implement an USB Device HID Mouse enumeration
 * but the mouse events to move and click are not linked.
 * It is to validated the HID mouse enumeration
 * and USB core callback (VBus presence, resume, suspend and SOF).
 * This example uses the native HID driver for these operating systems.
 *
 */

//! \name Unit test configuration
//@{
/**
 * \def CONF_TEST_USART
 * \brief USART to redirect STDIO to
 */
/**
 * \def CONF_TEST_BAUDRATE
 * \brief Baudrate of USART
 */
/**
 * \def CONF_TEST_CHARLENGTH
 * \brief Character length (bits) of USART
 */
/**
 * \def CONF_TEST_PARITY
 * \brief Parity mode of USART
 */
/**
 * \def CONF_TEST_STOPBITS
 * \brief Stopbit configuration of USART
 */
//@}

static bool main_b_vbus_event = false;
static bool main_b_resume_event = false;
static bool main_b_suspend_event = false;
static bool main_b_sof_event = false;
static bool main_b_mouse_enumerated = false;
#if SAMD21
/* Structure for UART module connected to EDBG (used for unit test output) */
static struct usart_module cdc_uart_module;
#endif

static void run_usb_mouse_test(const struct test_case *test)
{
	UNUSED(test);
	// Start USB stack to authorize VBus monitoring
	udc_start();

	if (!udc_include_vbus_monitoring()) {
		// VBUS monitoring is not available on this product
		// thereby VBUS has to be considered as present
		main_vbus_action(true);
	}

	// The main loop manages only the power mode
	// because the USB management is done by interrupt
	while (!main_b_mouse_enumerated) {
		sleepmgr_enter_sleep();
		// No test_assert_true() call
		// because it must be a timeout from server
	}

	udc_stop();
}

static void run_usb_vbus_test(const struct test_case *test)
{
	test_assert_true(test, main_b_vbus_event, "Event vbus not receive");
}

static void run_usb_resume_test(const struct test_case *test)
{
	test_assert_true(test, main_b_resume_event, "Event resume not receive");
}

static void run_usb_suspend_test(const struct test_case *test)
{
	test_assert_true(test, main_b_suspend_event, "Event suspend not receive");
}

static void run_usb_sof_test(const struct test_case *test)
{
	test_assert_true(test, main_b_sof_event, "Event sof not receive");
}

/**
 * \brief Run usb device mouse unit tests
 *
 * Initializes the clock system, board and serial output, then sets up the
 * usb unit test suite and runs it.
 */
int main(void)
{
#if !SAMD21
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits   = CONF_TEST_STOPBITS,
	};
#else
	struct usart_config usart_conf;
#endif

	irq_initialize_vectors();
	cpu_irq_enable();

#if !SAMD21
	sysclk_init();
	board_init();
#else
	system_init();
#endif
	// Initialize the sleep manager
	sleepmgr_init();

#if !SAMD21
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);
#else
	/* Configure USART for unit test output */
	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = CONF_STDIO_MUX_SETTING;
	usart_conf.pinmux_pad0 = CONF_STDIO_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = CONF_STDIO_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = CONF_STDIO_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = CONF_STDIO_PINMUX_PAD3;
	usart_conf.baudrate    = CONF_STDIO_BAUDRATE;

	stdio_serial_init(&cdc_uart_module, CONF_STDIO_USART, &usart_conf);
	usart_enable(&cdc_uart_module);
#endif
	// Define all the timestamp to date test cases
	DEFINE_TEST_CASE(usb_mouse_test, NULL,
			run_usb_mouse_test, NULL,
			"USB Device mouse enumeration test");
	DEFINE_TEST_CASE(usb_vbus_test, NULL,
			run_usb_vbus_test, NULL,
			"USB vbus event test");
	DEFINE_TEST_CASE(usb_resume_test, NULL,
			run_usb_resume_test, NULL,
			"USB resume event test");
	DEFINE_TEST_CASE(usb_suspend_test, NULL,
			run_usb_suspend_test, NULL,
			"USB suspend event test");
	DEFINE_TEST_CASE(usb_sof_test, NULL,
			run_usb_sof_test, NULL,
			"USB sof event test");

	// Put test case addresses in an array
	DEFINE_TEST_ARRAY(usb_mouse_tests) = {
		&usb_mouse_test,
		&usb_vbus_test,
		&usb_resume_test,
		&usb_suspend_test,
		&usb_sof_test,
	};

	// Define the test suite
	DEFINE_TEST_SUITE(usb_mouse_suite, usb_mouse_tests,
			"Common usb HID mouse service with test suite");

	// The unit test prints message via UART which does not support deep sleep mode.
#if SAM
	sleepmgr_lock_mode(SLEEPMGR_ACTIVE);
#else
	sleepmgr_lock_mode(SLEEPMGR_IDLE);
#endif

	// Run all tests in the suite
	test_suite_run(&usb_mouse_suite);

	while (1) {
		// Intentionally left empty.
	}
}

void main_vbus_action(bool b_high)
{
	if (b_high) {
		main_b_vbus_event = true;
	}
}

void main_suspend_action(void)
{
	main_b_resume_event = true;
}

void main_resume_action(void)
{
	main_b_suspend_event = true;
}

void main_sof_action(void)
{
	main_b_sof_event = true;
}

void main_remotewakeup_enable(void)
{
}

void main_remotewakeup_disable(void)
{
}

bool main_mouse_enable(void)
{
	main_b_mouse_enumerated = true;
	return true;
}

void main_mouse_disable(void)
{
}

