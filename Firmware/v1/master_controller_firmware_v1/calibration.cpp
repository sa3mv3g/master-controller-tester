/*
 * calibration.cpp
 *
 * Created: 4/26/2021 6:15:30 AM
 * Author : sanmveg saini
 * Company: ADVANCE INSTRUMENTATION 'n' CONTROL SYSTEMS
 * Website: www.aics.co.in
 */ 

#include "main.h"

#define DIGITS 4
union callibration_data_bytes_t{
	callibration_data_t data;
	uint8_t bytes[32];
} ;

callibration_data_bytes_t callibration_data_bytes;
callibration_data_t voltage_callibration, current_callibration;

void TM1638_DisplayNumber_calib(uint8_t num[], uint8_t hl,bool op = false){
	if(op){
		TM1638_DisplayNumber_raw(7,0x5c);
	}else{
		TM1638_DisplayNumber_raw(7,0);
	}
	
	if(hl == 1){
		TM1638_DisplayNumber_raw(6,0x74);
	}else{
		TM1638_DisplayNumber_raw(6,0x38);
	}
	
	TM1638_DisplayNumber_raw(0,num[0]);
	TM1638_DisplayNumber_raw(1,num[1]);
	TM1638_DisplayNumber_raw(2,num[2]);
	TM1638_DisplayNumber_raw(3,num[3]);
	TM1638_DisplayNumber_raw(4,num[4]);
	TM1638_DisplayNumber_raw(5,num[5]);
	
}

fixedPoint_t getNumber(fixedPoint_t initval, uint8_t hl){
	/* 
	b1 means alternate operation activated.
	if b1 is pressed even number of times then
		b2 will increase the digit by one at cursor position
		b3 will shift cursor position ahead
		b4 will place a decimal at the cursor position.
	if b1 is pressed odd number of times then 
		b2 will decrease the digit by one at the cursor position
		b3 will shift the cursor position behind
		b4 will exit the function and save the entered value.
	*/
	const uint8_t digits_to_raw_lut[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f };
	uint8_t digits[8] = {0,0,0,0,0,0,0,0};
	uint8_t digits_raw[8] = {0,0,0,0,0,0,0,0};
	uint8_t cursor = 0;
	int8_t decimal_place = initval.decPos;
	bool alternate_ops = false;
	
	for(uint8_t i = 0; i < DIGITS; i++){
		digits[i] = initval.number % 10;
		digits_raw[i] = digits_to_raw_lut[digits[i]];
		initval.number /= 10;
	}

	for(uint8_t i =0; i< 8;i++)
	if(i == decimal_place) digits_raw[i] |= 0x80;
	else digits_raw[i] &= ~0x80;
	
	TM1638_DisplayNumber_calib(digits_raw, hl);
	LEDon(7 - cursor);
	
	while(1){
		
		if(FALLING_EDGE(b1)){
			alternate_ops = !alternate_ops;
			TM1638_DisplayNumber_calib(digits_raw, hl, alternate_ops);
		}
		if(FALLING_EDGE(b2)){
			if(alternate_ops){
				digits[cursor] = (digits[cursor] == 0) ? 9 : digits[cursor] - 1;
			}else{
				uint8_t p = (digits[cursor]) ;
				digits[cursor] = (p + 1) >= 10 ? 0 : (p + 1) ;
			}
			digits_raw[cursor] = digits_to_raw_lut[digits[cursor]] | (decimal_place == cursor ? 0x80 : 0x0);
			TM1638_DisplayNumber_calib(digits_raw, hl, alternate_ops);
		}
		if(FALLING_EDGE(b3)){
			if(alternate_ops) cursor = cursor == 0 ? cursor = DIGITS - 1 : cursor - 1;
			else cursor = ( cursor + 1 ) >= DIGITS ? 0 : (cursor + 1);
			LEDon(7-cursor);
			TM1638_DisplayNumber_calib(digits_raw, hl, alternate_ops);
		}
		if(FALLING_EDGE(b4)){
			if(alternate_ops){
				break;
			}else{
				static bool even = false;
				if(even){
					decimal_place = -1;
				}else{
					decimal_place = cursor;
				}
				even = !even;
			}
			for(uint8_t i =0; i< 8;i++) 
				if(i == decimal_place) digits_raw[i] |= 0x80;
				else digits_raw[i] &= ~0x80;
			TM1638_DisplayNumber_calib(digits_raw, hl, alternate_ops);
		}
	}
	
	uint32_t place = 1;
	decimal_place = (decimal_place < 0 ? 0 : decimal_place);
	fixedPoint_t res = {0, decimal_place};
	for(unsigned char i = 0; i < 8; i++){
		res.number += place * digits[i];
		place *= 10;
	}
	
	return res;
}

void saveCallibrationData(){
	unsigned int address = 0;
	EEPROM_write(address++, 0xaa);
	callibration_data_bytes.data = voltage_callibration;
	for(uint8_t i = 0;i < 32;i++) 
	{
		EEPROM_write(address++, callibration_data_bytes.bytes[i]);
	}
	callibration_data_bytes.data = current_callibration;
	for(uint8_t i = 0;i < 32;i++) 
	{
		EEPROM_write(address++, callibration_data_bytes.bytes[i]);
	}
}

void loadCallibrationData(){
	unsigned int adderss = 0;
	uint8_t header = EEPROM_read(adderss++);
	if(header != 0xaa)
	{
		voltage_callibration.in_from.number = 0;
		voltage_callibration.in_from.decPos = 0;
		
		voltage_callibration.in_to.number = 1023;
		voltage_callibration.in_to.decPos = 0;
		
		voltage_callibration.out_from.number = 0;
		voltage_callibration.out_from.decPos = 0;
		
		voltage_callibration.out_to.number = 1023;
		voltage_callibration.out_to.decPos = 0;
		
		current_callibration.in_from.number = 0;
		current_callibration.in_from.decPos = 0;
		
		current_callibration.in_to.number = 4095;
		current_callibration.in_to.decPos = 0;
		
		current_callibration.out_from.number = 0;
		current_callibration.out_from.decPos = 0;
		
		current_callibration.out_to.number = 4095;
		current_callibration.out_to.decPos = 0;
		
		saveCallibrationData();
	}
	for(unsigned int i = 0;  i < 32; i++)
	{
		callibration_data_bytes.bytes[i] = EEPROM_read(adderss++);
	}
	voltage_callibration = callibration_data_bytes.data;
	for(unsigned int i = 0;  i < 32; i++)
	{
		callibration_data_bytes.bytes[i] = EEPROM_read(adderss++);
	}
	current_callibration = callibration_data_bytes.data;
}

void callibrate(uint8_t what){
	if(what == VOLTAGE){
		fixedPoint_t in_from = {0};
		fixedPoint_t in_to = {0};
		voltage_callibration.out_from = getNumber(voltage_callibration.out_from, false);
		flotofix(&in_from, (float)readVoltage(), 4);
		voltage_callibration.in_from = in_from;
		voltage_callibration.out_to = getNumber(voltage_callibration.out_to, true);
		flotofix(&in_to, (float)readVoltage(), 4);
		voltage_callibration.in_to = in_to;
		
	}else if(what == CURRENT){
		fixedPoint_t in_from = {0};
		fixedPoint_t in_to = {0};
		current_callibration.out_from = getNumber(current_callibration.out_from, false);
		
		#ifdef DEBUG
		UART_send(&current_callibration.out_from);
		#endif
		
		flotofix(&in_from, (float)readCurrent(), 4);
		
		#ifdef DEBUG
		UART_send(&in_from);
		#endif
		
		current_callibration.in_from = in_from;
		current_callibration.out_to = getNumber(current_callibration.out_to, true);
		
		
		#ifdef DEBUG
		UART_send(&current_callibration.out_to);
		#endif
		
		flotofix(&in_to, (float)readCurrent(), 4);
		current_callibration.in_to = in_to;
		
		
		#ifdef DEBUG
		UART_send(&current_callibration.in_to);
		#endif
		
	}
	saveCallibrationData();
	_delay_ms(1000);
}