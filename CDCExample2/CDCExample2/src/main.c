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
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
void cdcexample(void);

volatile bool cdc_up = false;

bool my_callback_cdc_enable(void)
{
	cdc_up = true;
	return true;
}

void my_callback_cdc_disable(void)
{
	cdc_up = false;
}

int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).
	pmic_init();
	irq_initialize_vectors();
	sysclk_init();
	board_init();
	cpu_irq_enable();
	udc_start();
	cdcexample();
	// Insert application code here, after the board has been initialized.
}
