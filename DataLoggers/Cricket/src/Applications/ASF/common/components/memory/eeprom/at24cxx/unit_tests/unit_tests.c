/**
 * \file
 *
 * \brief Unit tests for at24cxx driver.
 *
 * Copyright (c) 2011 - 2013 Atmel Corporation. All rights reserved.
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

#include <stdint.h>
#include <stdbool.h>
#include <board.h>
#include <sysclk.h>
#include <string.h>
#include <unit_test/suite.h>
#include <stdio_serial.h>
#include <conf_clock.h>
#include <conf_board.h>
#include <conf_test.h>
#include <at24cxx.h>
#include "twi.h"

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the at24cxx driver.
 * It consists of test cases for the following functionality:
 * - at24cxx data read/write
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * SAM3X, SAM4C devices can be used.
 * This example has been tested with the following setup:
 * - sam3x8h_sam3x_ek
 * - sam4c16c_sam4c_ek
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for ARM. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 * Support and FAQ: http://support.atmel.no/
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

/** Memory Start Address of AT24CXX chips */
#define AT24C_MEM_ADDR  0
/** TWI Bus Clock 400kHz */
#define AT24C_TWI_CLK   400000UL
/** Test data */
#define TEST_DATA  0xA5u

bool result = false;

#define  TEST_DATA_LENGTH  (sizeof(test_data_tx)/sizeof(uint8_t))
static uint8_t test_data_tx[] = {
	'A', 'T', '2', '4', 'C', 'X', 'X', ' ', 'U', 'N', 'I', 'T', ' ',
	'T', 'E', 'S', 'T', 'S'
};
static uint8_t test_data_rx[TEST_DATA_LENGTH];

/* Memory Pattern */
#define MEMORY_PATTERN  TEST_DATA
/* EEPROM Page Size */
#define PAGE_SIZE  128UL
/* EEPROM Page Address */
#define PAGE_ADDR  511UL
uint8_t page_read_buf[PAGE_SIZE];
uint8_t page_write_buf[PAGE_SIZE];

static bool buffer_cmp(const uint8_t *pbuf_0, const uint8_t *pbuf_1,
		uint32_t len)
{
	for (; len != 0; len--) {
		if (*(pbuf_0++) != *(pbuf_1++)) {
			return false;
		}
	}
	return true;
}

static void buffer_fill(uint8_t *pbuf, uint32_t len, const uint8_t pattern)
{
	for (; len != 0; len--) {
		*(pbuf++) = pattern;
	}
}

/**
 * \brief Test data read/write API functions.
 *
 * This test calls the data read/write API functions and check the data
 * consistency.
 *
 * \param test Current test case.
 */
static void run_test_setup(const struct test_case *test)
{
	twi_options_t opt;
	uint32_t tmp = 0;

	/* Configure the options of TWI driver */
	opt.master_clk = sysclk_get_cpu_hz();
	opt.speed = AT24C_TWI_CLK;

	tmp = twi_master_setup(BOARD_AT24C_TWI_INSTANCE, &opt);

	result = (tmp == TWI_SUCCESS);
	test_assert_true(test, result, "TWI master setup failed!");
}

/**
 * \brief Test data write API function.
 *
 * This test calls the data write API function and check if the write operation
 * succeeded.
 *
 * \param test Current test case.
 */
static void run_test_byte_access(const struct test_case *test)
{
	uint8_t data = 0;

	if (result == true) {
		if (at24cxx_write_byte(AT24C_MEM_ADDR, TEST_DATA) !=
				AT24C_WRITE_SUCCESS) {
			result = false;
			test_assert_true(test, false, "Byte Write NG!");
		}
		if (at24cxx_read_byte(AT24C_MEM_ADDR, &data) != AT24C_READ_SUCCESS) {
			result = false;
			test_assert_true(test, false, "Byte Read NG!");
		}
		result = (data == TEST_DATA);
		test_assert_true(test, result, "Byte Comparison NG!");
	}
}

/**
 * \brief Test data read API function.
 *
 * This test calls the data read API function and check if the read operation
 * succeeded.
 *
 * \param test Current test case.
 */
static void run_test_continuous_access(const struct test_case *test)
{
	if (result == true) {
		buffer_fill(test_data_rx, TEST_DATA_LENGTH, 0);

		if (at24cxx_write_continuous(AT24C_MEM_ADDR, TEST_DATA_LENGTH,
				test_data_tx) != AT24C_WRITE_SUCCESS) {
			result = false;
			test_assert_true(test, false, "Continuous Write NG!");
		}

		if (at24cxx_read_continuous(AT24C_MEM_ADDR, TEST_DATA_LENGTH,
				test_data_rx) != AT24C_READ_SUCCESS) {
			result = false;
			test_assert_true(test, false, "Continuous Read NG!");
		}

		result = buffer_cmp(test_data_tx, test_data_rx, TEST_DATA_LENGTH);
		test_assert_true(test, result, "Continuous Comparsion NG!");
	}
}

/**
 * \brief Test data read API function.
 *
 * This test calls the data read API function and check if the read operation
 * succeeded.
 *
 * \param test Current test case.
 */
static void run_test_page_access(const struct test_case *test)
{
	if (result == true) {
		buffer_fill(page_read_buf, PAGE_SIZE, 0);
		buffer_fill(page_write_buf, PAGE_SIZE, MEMORY_PATTERN);

		if (at24cxx_write_page(PAGE_ADDR, PAGE_SIZE, page_write_buf) !=
				AT24C_WRITE_SUCCESS) {
			result = false;
			test_assert_true(test, false, "Page Write NG!");
		}

		if (at24cxx_read_page(PAGE_ADDR, PAGE_SIZE, page_read_buf) !=
				AT24C_WRITE_SUCCESS) {
			result = false;
			test_assert_true(test, false, "Page Read NG!");
		}

		result = buffer_cmp(page_read_buf, page_write_buf, PAGE_SIZE);
		test_assert_true(test, result, "Continuous Comparsion NG!");
	}
}

/**
 * \brief Test data read/write API functions.
 *
 * This test calls the data read/write API functions and check the data
 * consistency.
 *
 * \param test Current test case.
 */
static void run_test_fill_pattern(const struct test_case *test)
{
	if (result == true) {
		buffer_fill(test_data_rx, TEST_DATA_LENGTH, 0);
		buffer_fill(test_data_tx, TEST_DATA_LENGTH, MEMORY_PATTERN);

		if (at24cxx_fill_pattern(AT24C_MEM_ADDR,
				AT24C_MEM_ADDR + TEST_DATA_LENGTH - 1, MEMORY_PATTERN) !=
				AT24C_WRITE_SUCCESS) {
			result = false;
			test_assert_true(test, false, "Pattern Fill NG!");
		}

		if (at24cxx_read_continuous(AT24C_MEM_ADDR, TEST_DATA_LENGTH,
				test_data_rx) != AT24C_READ_SUCCESS) {
			result = false;
			test_assert_true(test, false, "Continuous Read NG!");
		}

		result = buffer_cmp(test_data_tx, test_data_rx, TEST_DATA_LENGTH);
		test_assert_true(test, result, "Pattern Fill Comparison NG!");
	}
}

/**
 * \brief Run AT24CXX driver unit tests.
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits   = CONF_TEST_STOPBITS
	};

	sysclk_init();
	board_init();
	/* Reset EEPROM state to release TWI */
	at24cxx_reset();

	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	/* Define all the test cases */
	DEFINE_TEST_CASE(at24cxx_test_setup, NULL,
			run_test_setup, NULL,
			"at24cxx setup test");
	DEFINE_TEST_CASE(at24cxx_test_byte_access, NULL,
			run_test_byte_access, NULL,
			"at24cxx byte access test");
	DEFINE_TEST_CASE(at24cxx_test_continuous_access, NULL,
			run_test_continuous_access, NULL,
			"at24cxx continuous access test");
	DEFINE_TEST_CASE(at24cxx_test_page_access, NULL,
			run_test_page_access, NULL,
			"at24cxx page access test");
	DEFINE_TEST_CASE(at24cxx_test_fill_pattern, NULL,
			run_test_fill_pattern, NULL,
			"at24cxx fill pattern test");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(at24cxx_test_array) = {
		&at24cxx_test_setup,
		&at24cxx_test_byte_access,
		&at24cxx_test_continuous_access,
		&at24cxx_test_page_access,
		&at24cxx_test_fill_pattern
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(at24cxx_suite, at24cxx_test_array,
			"at24cxx driver test suite");

	/* Run all tests in the test suite */
	test_suite_run(&at24cxx_suite);

	while (1) {
		/* Busy-wait forever */
	}
}
