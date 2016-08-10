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
#include "Parser.h"
volatile int done;
int main (void)
{
	board_init();
	ParseInit();
	while (ParseDataAvailable())
	{
		if (Parse(ParseGet()))
			done = 99;
		else
			done = 999;
	}
	// Insert application code here, after the board has been initialized.
	done = 9999;
}
