/*
 * cdcexample.cpp
 *
 * Created: 14/03/2015 19:47:06
 *  Author: henryd
 */ 


#include <asf.h>
#include <avr/io.h>
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

int main(void)
{
	pmic_init();
	irq_initialize_vectors();
	sysclk_init();
	board_init();
	udc_start();
   while(1)
    {
        //TODO:: Please write your application code 
    }
}