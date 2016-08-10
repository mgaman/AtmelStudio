/*
 * PWM.c
 *
 * Created: 2/17/2014 11:42:23 AM
 *  Author: pc
 */ 

#include <asf.h>
#include "ModemPWM.h"

struct pwm_config pwm_cfg[2];	// support 2 devices


//---------------------------------------------------------------------
void PwmInit(uint8_t device, enum pwm_channel_t channel, uint16_t freq)
{
	pwm_init(&pwm_cfg[device],PWM_TCC0,channel,freq);	// hard wired to buzzer on PC2
}


//----------------------------------------------
void PwmStart(uint8_t device, uint8_t dutycycle)
{
	pwm_start(&pwm_cfg[device], dutycycle);		// 50% duty cycle	
}


//--------------------------
void PwmStop(uint8_t device)
{
	pwm_stop(&pwm_cfg[device]);
}