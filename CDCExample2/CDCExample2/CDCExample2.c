/*
 * CDCExample2.c
 *
 * Created: 14/03/2015 20:01:00
 *  Author: henryd
 */ 


#include <asf.h>
#include <avr/io.h>
#include <stdio.h>

extern volatile bool cdc_up;
char buffer[100];
void cdcexample(void)
{
	int count = 0;
	int length;
    while(1)
    {
        if (cdc_up)
		{
			delay_s(1);
			length = sprintf(buffer,"line %d\r\n",count++);
			udi_cdc_write_buf(buffer,length);
		}
    }
}