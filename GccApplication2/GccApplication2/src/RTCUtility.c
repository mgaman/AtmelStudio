/*
 * RTCUtility.c
 *
 * Created: 04-Feb-15 5:19:34 PM
 *  Author: Saba
 */ 

#include <asf.h>
#include "RTCUtility.h"

bool rtcdone;

void RtcDelay(uint32_t t)
{
	rtcdone = false;
	rtc_set_alarm_relative(t);
	while (!rtcdone);
}

void rtcallback(uint32_t t)
{
	rtcdone = true;
}
