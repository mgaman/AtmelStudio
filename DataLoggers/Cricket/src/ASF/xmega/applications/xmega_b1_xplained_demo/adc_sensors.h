/**
 * \file
 *
 * \brief ADC Sensor interface
 *
 * Copyright (c) 2011 Atmel Corporation. All rights reserved.
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
#ifndef ADC_SENSORS_H_INCLUDED
#define ADC_SENSORS_H_INCLUDED

#include "asf.h"

/**
 * \brief Initialize ADC channels for on-board sensors.
 *
 * This will set up the ADC for reading the NTC, light sensor, potentiometer,
 * and external voltage input present on the A3BU-Xplained board.
 */
void adc_sensors_init(void);

/**
 * \brief Stop the ADC sensors
 *
 */
void adc_sensors_stop(void);

/**
 * \brief Start a new burst of ADC conversions.
 *
 * This will initiated a new burst of ADC conversions to update the on-board
 * sensors values.
 */
void adc_sensors_start_conversions(void);

/**
 * \brief Read the temperature value.
 *
 * \return temperature in Degree Celsius
 */
int8_t adc_sensor_get_temperature(void);

/**
 * \brief Read the light sensor value.
 *
 * This will read the ADC value of the channel and pin connected to the
 * lightsensor.
 *
 * \return the raw light sensor value from the ACD
 */
uint16_t adc_sensor_get_light(void);

/**
 * \brief Read the potentiometer value.
 *
 * This will read the ADC value of the channel and pin connected to the
 * potentiometer.
 *
 * \return the potentiometer voltage value in millivolt
 */
uint16_t adc_sensor_get_potentiometer(void);

/**
 * \brief Read the external voltage input value.
 *
 * This will read the ADC value of the channel and pin connected to the
 * external voltage input.
 *
 * \return the external voltage input value in millivolt
 */
uint16_t adc_sensor_get_ext_voltage(void);


#endif /* ADC_SENSORS_H_INCLUDED */
