/*
 * button.cpp
 *
 * Created: 4/23/2021 2:32:40 PM
 * Author : sanmveg saini
 * Company: ADVANCE INSTRUMENTATION 'n' CONTROL SYSTEMS
 * Website: www.aics.co.in
 */ 

#include "main.h"

volatile button b1, b2, b3, b4;

volatile bool do_callibrate_current = false;
volatile bool do_callibrate_voltage = false; 

void Buttons_init(){
	DDRD &= ~((1<<DDD5) | (1<<DDD4) | (1<<DDD3) | (1<<DDD2));
}

void Buttons_scan(){
	static uint32_t prev_b1_time = 0;
	static uint32_t prev_b2_time = 0;
	static uint32_t prev_b3_time = 0;
	static uint32_t prev_b4_time = 0;
	
	static uint32_t curr_calib_falling_edge = 0;
	static uint32_t volt_calib_falling_edge = 0;
	
	static bool pb1t = 1;
	static bool pb2t = 1;
	static bool pb3t = 1;
	static bool pb4t = 1;
	
	bool b1t = PIND & (1 << PIND5);
	bool b2t = PIND & (1 << PIND4);
	bool b3t = PIND & (1 << PIND3);
	bool b4t = PIND & (1 << PIND2);
	
	uint32_t c_millis = millis();
	
	if(b1t != pb1t) prev_b1_time = c_millis;
	if(b2t != pb2t) prev_b2_time = c_millis;
	if(b3t != pb3t) prev_b3_time = c_millis;
	if(b4t != pb4t) prev_b4_time = c_millis;
	
	if(c_millis - prev_b1_time > 50) {
		b1.prev = b1.curr;
		b1.curr = b1t ? 1 : 0;
	}
	if(c_millis - prev_b2_time > 50) {
		b2.prev = b2.curr;
		b2.curr = b2t ? 1 : 0;
	}
	if(c_millis - prev_b3_time > 50) {
		b3.prev = b3.curr;
		b3.curr = b3t ? 1 : 0;
	}
	if(c_millis - prev_b4_time > 50) {
		b4.prev = b4.curr;
		b4.curr = b4t ? 1 : 0;
	}
	
	pb1t = b1t;
	pb2t = b2t;
	pb3t = b3t;
	pb4t = b4t;

	if(FALLING_EDGE(b4)){
		curr_calib_falling_edge = c_millis;
	}
	if(FALLING_EDGE(b3)){
		volt_calib_falling_edge = c_millis;
	}
	if(b4.curr == 0 && (c_millis - curr_calib_falling_edge > 5000)){
		do_callibrate_current = true;
		
	}
	if(b3.curr == 0 && (c_millis - volt_calib_falling_edge > 5000)){
		do_callibrate_voltage = true;
	}
}

