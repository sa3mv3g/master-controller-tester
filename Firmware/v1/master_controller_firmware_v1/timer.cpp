/*
 * timer.cpp
 *
 * Created: 4/23/2021 2:10:21 PM
 * Author : sanmveg saini
 * Company: ADVANCE INSTRUMENTATION 'n' CONTROL SYSTEMS
 * Website: www.aics.co.in
 */ 

#include "main.h"


volatile uint32_t ms_count = 0;
volatile uint8_t did_tick = 0;

void TMR0_Init(){
	// timer frequency = 1meg / (125 * 8) = 1meg / 1k = 1k
	TCNT0 = 0xff - 125 + 1;
	TCCR0 = 2; // prescalar of 8
	TIMSK |= 1<<TOIE0;
}

uint32_t millis(){
	return ms_count;
}

ISR(TIMER0_OVF_vect){
	TCNT0 = 0xff - 125 + 1;
	
	ms_count++;
	did_tick = 1;
	Buttons_scan();
}
