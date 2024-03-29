/**
 * \file
 *
 * \brief Unit tests for QTouch component driver with I2C interface.
 *
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
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

#include "asf.h"
#include "conf_test.h"
#include "string.h"

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for QTouch component driver
 * with I2C interface.
 *
 * It consists of test cases for the following functionalities:
 * - Check if communication is ready with QT device
 * - Configure QT device and check
 * - Get all status from QT device and check
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_qt_i2c.h
 * - \ref conf_test.h
 *
 * \section device_info Device Info
 * This example has been tested with the following setup:
 * - AT42QT2160 on SAM4E-EK
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for ARM. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 * Support and FAQ: http://support.atmel.com/
 */

/** Storage for QT status */
struct qt_status status;
/** Storage for QT setup block 0 */
struct qt_setup_block setup_block_0;
/** Storage for QT setup block 1 */
struct qt_setup_block setup_block_1;

/**
 * \brief Initialize TWI communication interface.
 */
static void init_interface(void)
{
	/* TWI master initialization options. */
	twi_master_options_t twi_opt;

	memset((void *)&twi_opt, 0, sizeof(twi_master_options_t));
	twi_opt.speed = 100000;    /* 100KHz for I2C speed */

	/* Initialize the TWI master driver. */
	twi_master_setup(BOARD_QT_TWI_INSTANCE, &twi_opt);
}

/**
 * \brief Test if communication is ready with QT device.
 *
 * \param test Current test case.
 */
static void run_qt_test_comm_ready(const struct test_case *test)
{
	enum status_code ret;

	/* Initialize communication interface */
	init_interface();

	/* Reset QT device */
	qt_hardware_reset();

	/* Check communication is ready and able to read Chip ID */
	ret = qt_get_comm_ready();
	test_assert_true(test, ret != ERR_IO_ERROR,
			"Can't communicate with QT device");
	test_assert_true(test, ret != ERR_UNSUPPORTED_DEV,
			"Unsupported QT device");
}

/**
 * \brief Test if set QT device setup block correctly.
 *
 * \param test Current test case.
 */
static void run_qt_test_setup_block(const struct test_case *test)
{
	uint32_t i;
	uint8_t *ptr_block_0 = (uint8_t *)&setup_block_0;
	uint8_t *ptr_block_1 = (uint8_t *)&setup_block_1;
	enum status_code ret;

	/* Read setup block */
	ret = qt_read_setup_block(&setup_block_0);
	test_assert_true(test, ret == STATUS_OK, "Fail to read setup block");

	/* Modify setup block parameters */
	ut_set_qt_param(&setup_block_0);

	/* Write setup block */
	ret = qt_write_setup_block(&setup_block_0);
	test_assert_true(test, ret == STATUS_OK, "Fail to write setup block");

	/* Read setup block back and check */
	memset((uint8_t *)&setup_block_1, 0, sizeof(struct qt_setup_block));
	qt_read_setup_block(&setup_block_1);
	for (i = 0; i < sizeof(struct qt_setup_block); i++) {
		if (*ptr_block_0++ != *ptr_block_1++) {
			test_assert_true(test, false, "Fail to check setup block: %d", i);
		}
	}
}

/**
 * \brief Test if key status correctly.
 *
 * For unit test board, it assume there is no key been pressed in the test.
 *
 * \param test Current test case.
 */
static void run_qt_test_get_status(const struct test_case *test)
{
	uint32_t i;
	enum status_code ret;

	/* Test if any key pressed in 2 second (2000 * 1ms) */
	for (i = 0; i < 2000; i++) {
		if (qt_is_change_line_low()) {
			ret = qt_get_status(&status);
			test_assert_true(test, ret == STATUS_OK, "Fail to get QT status");

			test_assert_false(test, ut_is_any_key_pressed(&status),
					"Unexpect key pressed happen");
		}

		delay_ms(1);
	}
}

/**
 * \brief Run QTouch component driver with I2C interface unit tests.
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate = CONF_TEST_BAUDRATE,
		.paritytype = CONF_TEST_PARITY
	};

	/* Initialize the system clock and board */
	sysclk_init();
	board_init();

	/* Initialize serial port */
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	/* Define all the test cases */
	DEFINE_TEST_CASE(qt_test_comm_ready, NULL, run_qt_test_comm_ready, NULL,
			"Test QT communication ready");

	DEFINE_TEST_CASE(qt_test_setup_block, NULL, run_qt_test_setup_block, NULL,
			"Test QT setup block");

	DEFINE_TEST_CASE(qt_test_get_status, NULL, run_qt_test_get_status, NULL,
			"Test QT get key status");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(qt_tests) = {
		&qt_test_comm_ready,
		&qt_test_setup_block,
		&qt_test_get_status
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(qt_suite, qt_tests,
			"QTouch device with I2C interface test suite");

	/* Run all tests in the test suite */
	test_suite_run(&qt_suite);

	while (1) {
		/* Busy-wait forever */
	}
}
