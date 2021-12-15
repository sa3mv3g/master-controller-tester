/*
 * fixedpoint.cpp
 *
 * Created: 4/29/2021 1:52:53 PM
 * Author : sanmveg saini
 * Company: ADVANCE INSTRUMENTATION 'n' CONTROL SYSTEMS
 * Website: www.aics.co.in
 */ 

#include "main.h"


uint8_t flotofix(fixedPoint_t * res, float num, uint8_t total_len){
	if(num < 0) return 0;
	
	uint32_t nn = num;
	uint32_t nnlen = 0;
	int8_t d = 0;
	for(uint32_t nt = nn; nt > 0 && nnlen < total_len; nnlen++, nt /= 10);
	for(uint8_t i =0; i < (total_len - nnlen);i++, d++){
		num *= 10;
		nn *= 10;
		nn += (num - nn);  
	}
	res->decPos = d;
	res->number = nn;
	return 1;
}

float fixtoflo(fixedPoint_t fix){
	float res = fix.number;
	for (uint8_t i =0;i < fix.decPos; i++)res /= 10;
	return res;
}