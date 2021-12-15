/*
 * led.cpp
 *
 * Created: 4/30/2021 12:58:55 PM
 * Author : sanmveg saini
 * Company: ADVANCE INSTRUMENTATION 'n' CONTROL SYSTEMS
 * Website: www.aics.co.in
 */ 

#include "main.h"

#define L1 0x01
#define L2 0x02
#define L3 0x04
#define L4 0x08
#define L5 0x10
#define L6 0x20
#define L7 0x40
#define L8 0x80

#define Forward 0x1
#define Reverse 0x2
#define Traction 0x04
#define Breaking 0x08
#define Level_15 0x10
#define Level_1_3 0x20
#define Level_2_3 0x40
#define Level_full 0x80

void LEDInit(){
	DDRB |= 0X0F;
	DDRA |= 0XF0;
	DDRC = 0; // all input 
	PORTC = 0;
}

void LEDon(uint8_t pos){
	if(pos > 7) return;
	if(pos < 4){
		PORTB &= 0xf0;
		PORTB |= 1<<pos;
	}else{
		PORTA &= 0X0F;
		PORTA |= 1<<pos;
	}
}

void LEDset(uint8_t vals){
	for(uint8_t i =0;i<8;i++){
		if(vals & (1<<i)) LEDon(i);
	}
}

void LEDAlloff(){
	PORTB &= 0xf0;
	PORTA &= 0x0f;
}

void LEDAllon(){
	PORTB |= 0x0f;
	PORTA |= 0xf0;
}
/// switching program test and driving and breaking drum test
void sw_dbd_test(){
	
	uint8_t data = PINC;
	uint8_t res = 0;
	data = ~data;
	 LEDAlloff();
	
	if((data & L1) && (data & L2))	res |= Forward;
	else if((data & L3) && (data & L4)) res |= Reverse;
	
	if(((data & L5) || (data & L6)) && (data & L7) && (data & L8) ){
		res |= Level_full | Level_2_3;
		if(data & L5) res |= Traction;
		if(data & L6) res |= Breaking;
	}
	if(((data & L5) || (data & L6)) && (data & L7) && !(data & L8) ){
		res |= Level_1_3;
		if(data & L5) res |= Traction;
		if(data & L6) res |= Breaking;
	}
	if(((data & L5) || (data & L6)) && !(data & L7) && !(data & L8) ){
		res |= Level_15;
		if(data & L5) res |= Traction;
		if(data & L6) res |= Breaking;
	}
	PORTB &= 0xf0;
	PORTB |= res & 0x0f;
	PORTA &= 0X0F;
	PORTA |= res & 0xf0;
	//LEDset(res);
}

