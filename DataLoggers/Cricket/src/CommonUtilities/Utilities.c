/*
 * Utilities.c
 *
 * Created: 11/18/2013 4:05:46 PM
 *  Author: pc
 */ 
#include <asf.h>
#define UTILITIES_MAIN
#include "Utilities.h"
#include "spi_interface.h"
#include "RFHandler.h"

#include "i2c_lsm.h"
#include "i2c_bar.h"

// generic structure for performing a limited number of actions e.g. flash a led
extern struct sTimedActions TimedActions[];
void RTC_UpdateTimeEvents(void);

bool firefly_slave_is_sleeping;
volatile uint8_t firefly_slave_sleep_count;
volatile uint8_t firefly_slave_awake_count;

volatile uint8_t gsm_modem_wake_up_count = 0;

bool firefly_blinking_flag;
volatile uint8_t firefly_blink_timeout_count;


extern volatile uint32_t time_stamp;

extern bool led_one_on;
volatile extern char     flash_page_buf[];
volatile extern uint16_t flash_page_index;
extern volatile uint16_t lsm_check_count;

//-------------------------------------
void tc1ms_ovf_interrupt_callback(void)
{
	ms1Ticks++;                                    // enters here every millisecond when not in power_save mode
	
	if  (time_stamp == UINT32_MAX)
	   time_stamp = 0;
    else	   
	   time_stamp++;
	
	
	if (i2c_wait_flag_count > 0)
	{
	   i2c_wait_flag_count--;
	   if (i2c_wait_flag_count == 0)
	   {
	      i2c_wait_finished = true;
	   }
	}	 
#if 0
	if (lsm_check_count > 0)
	{
	   lsm_check_count--;
	}
	if (bar_check_count > 0)
	{
	   bar_check_count--;
	}
#endif
}


//--------------------------------------------
#ifdef TRACKER_TEST
volatile bool rtcdone;
uint32_t rtc_count = 0;
#endif

void rtc_ovf_interrupt_callback(uint32_t time)
{
#ifdef TRACKER_TEST
	rtcdone = true;
#else
  //rtcTicks++;	
	
 // guard against overflow as processor does not zero CNT
	if (RTC.CNT > (RTC.CNT - 100))	// start checking before the last moment
	{
		rtc_init();
		rtc_set_alarm_relative(0);	
	}
	else
		rtc_set_alarm(time);	
		
		
	//if (gsm_modem_wake_up_count > 0)
	//{
	   //gsm_modem_wake_up_count--;
	//}
		
	//RTC_UpdateTimeEvents();
#endif
}


//------------------
void OnBoardToggle()
{
	ONBOARD_LED_TOGGLE;
}


//------------------
void HiPowerToggle()
{
	HIPOWER_LED_TOGGLE;
}


//-------------------
void HiPower1Toggle()
{
	HIPOWER_LED_1_TOGGLE;
}


void HiPower2Toggle()
{
	//HIPOWER_LED_2_TOGGLE;
}


void Spare0Toggle()
{
	//ioport_toggle_pin(SPARE_GPIO_0);
}


void Spare1Toggle()
{
	//ioport_toggle_pin(SPARE_GPIO_1);	
}


//----------------------
void GahliIRLed1Toggle()
{
	//ioport_toggle_pin(GAHLILIT_IR_LED_1);
}


//----------------------
void GahliIRLed2Toggle()
{
	//ioport_toggle_pin(GAHLILIT_IR_LED_2);
}


#if 0
void RepeatAction(int index,int count,int time0,int time1,void (*function)(void))
#else
void RepeatAction(int index,int count,int time0,int time1)
#endif
{
	TimedActions[index].repetitions_left = count;
	TimedActions[index].endless = (count == 0);
		
	TimedActions[index].ticks_period_0 = time0;
	TimedActions[index].ticks_period_1 = time1;
	
//	TimedActions[index].function = function;
	TimedActions[index].active = true;
	TimedActions[index].ticks_left = time0;
}


// preload functions used by each action
//-------------------------
void RepeatActionInit(void)
{
	TimedActions[RTC_ONBOARD].function = OnBoardToggle;
	TimedActions[RTC_HIPOWER].function = HiPowerToggle;
	TimedActions[RTC_HIPOWER1].function = HiPower1Toggle;
	TimedActions[RTC_HIPOWER2].function = HiPower2Toggle;
	TimedActions[RTC_SPARE0].function = Spare0Toggle;
	TimedActions[RTC_SPARE1].function = Spare1Toggle;
	
	TimedActions[RTC_IR_1].function = GahliIRLed1Toggle;
	TimedActions[RTC_IR_2].function = GahliIRLed2Toggle;
}
/*
	General purpose code to handle RTC tick events
*/
void RTC_UpdateTimeEvents(void)
{
	int j;
	struct sTimedActions *pTA = TimedActions;
	// handle timed actions
	for (j=0;j<RTC_TOTAL;j++,pTA++)
	{
		if (pTA->active)
		{
			if (--(pTA->ticks_left) == 0)
			{
				(*pTA->function)();	// call the function
				pTA->period0 = !pTA->period0;	// switch modes
				pTA->ticks_left = pTA->period0 ? pTA->ticks_period_0 : pTA->ticks_period_1;
				if (!pTA->endless && --(pTA->repetitions_left) == 0)
					pTA->active = false;				
			}
		}
	}
}

/*
	Poll GPIO pins to check (1) change in state or (2) level 0 (button down)
	We must use polling as GPIO interrupts are not available in PSAVE sleep mode
*/
struct sPinPoll
 {
	port_pin_t pin;
	enum ePollMode pollmode;
	bool current_state;
	bool previous_state;
	bool in_use;
	};
#define MAX_PINS_TO_FOLLOW 6	// 4 pins define as pushbuttons plus 2 undefined GPIOs
struct sPinPoll PinPoll[MAX_PINS_TO_FOLLOW] = {{0}};
	
	
void GpioPinClear()
{
	memset(PinPoll,0,sizeof(PinPoll));
}



//-------------------------------------------------------
void GpioPollInit(port_pin_t pin,enum ePollMode pollmode)
{
	// add info to next free entry
	// be pedantic, add code to check table overflow
	
	int j = 0;

//  while ((PinPoll[j].in_use) && (j < MAX_PINS_TO_FOLLOW))
	
	while (PinPoll[j].pin != 0)
		j++;
		
	PinPoll[j].pin = pin;
	PinPoll[j].pollmode = pollmode;
	PinPoll[j].current_state = ioport_get_pin_level(pin);
	PinPoll[j].previous_state = ioport_get_pin_level(pin);
	PinPoll[j].in_use = true;	
}



// report if this pin reports the desired event
//---------------------------
bool GpioPoll(port_pin_t pin)
{
	bool rc = false;
	int j = 0;
	
	// be pedantic, add code to check table overflow
	//while ((PinPoll[j].pin != pin) && (j < MAX_PINS_TO_FOLLOW))
	
	while (PinPoll[j].pin != pin)
	{
		j++;
	}
		
	PinPoll[j].current_state = ioport_get_pin_level(pin);
	bool pinChanged = (PinPoll[j].current_state != PinPoll[j].previous_state);
	switch (PinPoll[j].pollmode)
	{
		case PIN_CHANGED:
			rc = pinChanged;
			break;
			
		case PIN_UP:
			if (pinChanged)
				rc = PinPoll[j].current_state == true;
			break;
			
		case PIN_DOWN:
			if (pinChanged)
				rc = PinPoll[j].current_state == false;
			break;
	}
	
	PinPoll[j].previous_state = PinPoll[j].current_state;
	return rc;
}


//-------------------------------------------------
uint8_t ReadRFRegister(uint8_t device, uint8_t reg)
{
	uint8_t data[5];
	bool success;
	
 // select device
	switch (device)
	{
		case 0:
			vChipSelectRF0();
			break;
			
		case 1:
			vChipSelectRF1();
			break;
	}
	
 // unselect device
	success = SPI2RF_Read(device, reg, data, 1);
	switch (device)
	{
		case 0:
			vChipUnSelectRF0();
			break;
			
		case 1:
			vChipUnSelectRF1();
			break;
	}
	
	if (success)
		return data[0];
	else
		return 0xff;
}


//-----------------------------------------------------------------
bool WriteRFRegister(uint8_t device, uint8_t reg, uint8_t valtoset)
{
	//uint8_t data[5];
	bool success;
	
 // select device
	switch (device)
	{
		case 0:
		vChipSelectRF0();
		break;
		
		case 1:
		vChipSelectRF1();
		break;
	}
	
 // unselect device
	success = SPI2RF_Write(device, reg, valtoset, 1);
	switch (device)
	{
		case 0:
		vChipUnSelectRF0();
		break;
		
		case 1:
		vChipUnSelectRF1();
		break;
	}
	
	return success;
}


#if 0
//*********************
void clean_flash_buff()
{
	flash_page_index = 0;
	
	for (int ff = 0; ff < sizeof(flash_page_buf); ff++)
   	   flash_page_buf[ff] = 0x20;		  
}
#endif

	
//*******************
void led_one_toggle()
{
	if (led_one_on)
	{
		ONBOARD_LED_2_ON;
		led_one_on = false;
	}
	else
	{
    	ONBOARD_LED_2_OFF;
		led_one_on = true;
	}
}	