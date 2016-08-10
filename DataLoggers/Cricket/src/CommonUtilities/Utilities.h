/*
 * Utilities.h
 *
 * Created: 11/18/2013 4:05:26 PM
 *  Author: pc
 */ 


#ifndef UTILITIES_H_
#define UTILITIES_H_

enum ePollMode {PIN_CHANGED,PIN_UP,PIN_DOWN};
void rtc_ovf_interrupt_callback(uint32_t time);
void OnBoardToggle(void);
void HiPowerToggle(void);
void HiPower1Toggle(void);
void HiPower2Toggle(void);
void Spare0Toggle(void);
void Spare1Toggle(void);

void RepeatAction(int index,int count,int time0,int time1/*,void (*function)(void)*/);
void RepeatActionInit(void);

void GpioPollInit(port_pin_t pin,enum ePollMode pollmode);
bool GpioPoll(port_pin_t pin);
void GpioPinClear(void);
void tc1ms_ovf_interrupt_callback(void);

uint8_t ReadRFRegister(uint8_t device,uint8_t reg);
bool WriteRFRegister(uint8_t device,uint8_t reg, uint8_t valtoset);

#ifdef UTILITIES_MAIN
    volatile uint32_t rtcTicks;
    volatile uint32_t ms1Ticks = 0;
#else
    extern volatile uint32_t rtcTicks;
    extern volatile uint32_t ms1Ticks;
#endif

extern bool firefly_slave_is_sleeping;
extern volatile uint8_t firefly_slave_sleep_count;
extern volatile uint8_t firefly_slave_awake_count;

extern volatile uint8_t gsm_modem_wake_up_count;

extern volatile uint16_t i2c_wait_flag_count;
extern bool i2c_wait_finished;
extern volatile uint16_t bar_check_count;
extern volatile uint16_t lsm_check_count;
extern bool time_to_read_gps;

extern bool firefly_blinking_flag;
extern volatile uint8_t firefly_blink_timeout_count;


//void clean_flash_buff(void);

void led_one_toggle(void);


#endif /* UTILITIES_H_ */