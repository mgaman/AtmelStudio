/*
 * ModemPWM.h   (avoid confusion with ASF PWM.h)
 *
 * Created: 2/17/2014 11:42:47 AM
 *  Author: pc
 */ 


#ifndef MODEMPWM_H_
#define MODEMPWM_H_
void PwmInit(uint8_t device,enum pwm_channel_t channel,uint16_t freq);
void PwmStart(uint8_t device,uint8_t dutycycle);
void PwmStop(uint8_t device);
#endif /* MODEMPWM_H_ */