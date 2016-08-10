/*
 * adctest.c
 *
 * Created: 1/7/2014 2:21:35 PM
 *  Author: pc
 */ 
#include <asf.h>

/* Internal ADC funtions */
static void main_adc_init(void);

void adc_test()
{
	/* ADC and DAC initializations */
	main_adc_init();	
}

/**
 * \brief Initialize ADC
 */
static void main_adc_init(void)
{
	/* ADC module configuration structure */
	struct adc_config adc_conf;
	/* ADC channel configuration structure */
	struct adc_channel_config adcch_conf;

	/* Configure the ADC module:
	 * - signed, 12-bit results
	 * - voltage reference = AREFA = 1.8V
	 * - input is VCC/2
	 * - 200 kHz maximum clock rate
	 * - manual conversion triggering
	 */
	adc_read_configuration(&ADCA, &adc_conf); /* Initialize structures. */
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12, ADC_REF_AREFA);
	adc_set_clock_rate(&adc_conf, 200000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_FREERUN, 1, 0);
	adc_write_configuration(&ADCA, &adc_conf);

	/* Configure ADC channel:
	 * - differential measurement mode
	 * - Input voltage V+ is ADC3 pin (PA3 pin)
	 * - Input voltage V- is nothing
	 * - 1x gain
	 * - interrupt flag set on completed conversion
	 */
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN3, ADCCH_NEG_NONE, 1);
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_disable_interrupt(&adcch_conf);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	/* Enable ADC */
	adc_enable(&ADCA);
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
}
