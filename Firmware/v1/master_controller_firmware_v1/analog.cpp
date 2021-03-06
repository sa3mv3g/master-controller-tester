/*
 * analog.cpp
 *
 * Created: 4/26/2021 5:39:27 AM
 * Author : sanmveg saini
 * Company: ADVANCE INSTRUMENTATION 'n' CONTROL SYSTEMS
 * Website: www.aics.co.in
 */ 

#include "main.h"

volatile uint16_t VolatageADCvalue = 0;
volatile uint16_t CurrentADCvalue = 0;

#define MAX_SAMPLE_COUNT 128

ISR(ADC_vect){
	static uint16_t count = 0;
	static bool flag = true;
	
	//uint8_t adcl = ADCL;
	//uint8_t adch = ADCH;
	uint16_t adc = ADCL | (ADCH << 8);
	
	count++;
	if(flag){
		static uint32_t tmp_CurrentADCvalue = 0;
		
		if((adc & 0x0200) != 0) adc = 0;
		
		if(count == MAX_SAMPLE_COUNT){
			CurrentADCvalue = tmp_CurrentADCvalue / MAX_SAMPLE_COUNT;
			tmp_CurrentADCvalue = count = 0;
			flag = false;
			ADMUX &= ~(0x1f);
			ADMUX |= 1;
		}else{
			tmp_CurrentADCvalue += adc;
		}
	}else{
		static uint32_t tmp_VolatageADCvalue = 0;
		if(count == MAX_SAMPLE_COUNT){
			count = 0;
			VolatageADCvalue = tmp_VolatageADCvalue / MAX_SAMPLE_COUNT;
			tmp_VolatageADCvalue = 0;
			ADMUX &= ~(0x1f);
			ADMUX |= 0x0d;
			flag = true;
		}else{
			tmp_VolatageADCvalue += adc;
		}
	}
	ADCSRA |= 1<<ADSC;
}

void analogInit(){
	/* 
		there are 2 channels, channel 1 is to read voltage which is a single ended input on ADC1.
		Other channel is differential channel on ADC2 and ADC3 to measure current.
		2 channels need to be sampled in every 1.28e-4 seconds and F_CPU is 1MHz. Hence,
		in every 64uS, an ADC interrupt should occurs which on odd interrupt measure channel 2 and 
		on even interrupt measure channel 1.
	*/
	// prescalar of 64 and interrupt enabled
	ADMUX |= (1 << REFS0) | 0x1b;
	ADCSRA = 0xce; 
}

uint16_t readCurrent2(){
	return CurrentADCvalue;
}

uint16_t readVoltage(){
	return VolatageADCvalue;	
}

static uint16_t readCurrent_raw(){
	cli();
	uint16_t res = 0;
	// curr	
	// PD7 - 21 - CS - O
	// PB6 -  7 - DAT - I
	// PB7 -  8 - CLK - O
	
	PORTD &= ~(1<<PORTD7);
	res = SPI_MasterTransmit(0xaa);
	res = (res << 8) | SPI_MasterTransmit(0xaa);
	PORTD |= (1<<PORTD7);
	sei();
	return (res & 0x0fff);
}

uint16_t readCurrent(){
	uint32_t data = 0;
	// change word ordering from LSB first to MSB first
	SPCR &= ~_BV(DORD);
	// increase speed 
	SPCR &= ~(_BV(SPR0)|_BV(SPR1));
	for(uint16_t i =0; i< 1024;i++){
		data += readCurrent_raw();
		_delay_us(1);
	}
	// change word ordering from MSB first to LSB first 
	// as required by TM1638
	SPCR |= _BV(DORD);
	// switch to slow speed
	SPCR |= (_BV(SPR0)|_BV(SPR1));
	data /= 1024;
	return data;
}