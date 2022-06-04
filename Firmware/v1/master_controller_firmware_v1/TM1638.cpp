/*
 * TM1638.cpp
 *
 * Created: 4/23/2021 1:59:14 PM
 * Author : sanmveg saini
 * Company: ADVANCE INSTRUMENTATION 'n' CONTROL SYSTEMS
 * Website: www.aics.co.in
 */ 

#include "main.h"

#define SET(A,B,C,D) do{A |= (1<<B);}while((C & (1<<D) ) == 0)
#define RESET(A,B,C,D) do{A &= ~(1<<B);}while((C & (1<<D) ) != 0)

const static uint8_t Segment_digits[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f };

uint8_t SPI_MasterTransmit(char cData)
{
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)))
	;
	return SPDR;
}

void TM1638_Init(){
	DDRB |= (1<<DDB4) | (1<<DDB5) | (1<<DDB7);
	PORTB |= 1<<PORTB4;
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPR1)|(1<<DORD);
}

void TM1638_SendCommand(char data){
	_delay_us(10);
	PORTB &= ~(1<<PORTB4);
	_delay_us(10);
	SPDR = data;
	while(!(SPSR & (1<<SPIF)))
	;
	_delay_us(10);
	PORTB |= 1<<PORTB4;
	_delay_us(10);
}

void TM1638_DisplayNumber(uint8_t index, uint8_t num, bool decimal){
	if(num > 9) num = 9;
	TM1638_SendCommand(0x40);
	uint8_t address = 0xc0 | ( ( (7 - index) * 2 ) & 0x0f );
	PORTB &= ~(1<<PORTB4);
	_delay_us(10);
	SPDR = address;
	while(!(SPSR & (1<<SPIF)));
	SPDR = Segment_digits[num] | (decimal ? 0x80 : 0);
	//SPDR = num;
	while(!(SPSR & (1<<SPIF)));
	_delay_us(10);
	PORTB |= 1<<PORTB4;
	_delay_us(10);
}

void TM1638_DisplayNumber_raw(uint8_t index, uint8_t num){
	TM1638_SendCommand(0x40);
	uint8_t address = 0xc0 | ( ( (7 - index) * 2 ) & 0x0f );
	PORTB &= ~(1<<PORTB4);
	_delay_us(10);
	SPDR = address;
	while(!(SPSR & (1<<SPIF)));
	//SPDR = Segment_digits[num];
	SPDR = num;
	while(!(SPSR & (1<<SPIF)));
	_delay_us(10);
	PORTB |= 1<<PORTB4;
	_delay_us(10);
	
}

void TM1638_reset(){
	TM1638_SendCommand(0x40);
	PORTB &= ~(1<<PORTB4);
	SPDR = 0xc0;
	while(!(SPSR & (1<<SPIF)));
	_delay_us(10);
	for(uint8_t i =0;i<16;i++){
		SPDR = 0x5f;
		while(!(SPSR & (1<<SPIF)));
		_delay_us(10);
	}
	PORTB |= 1<<PORTB4;
	_delay_us(10);
	TM1638_SendCommand(0x8f);
}

void TM1638_DisplayNumber(uint32_t num){
	
	for(uint8_t i = 0; i < 8 ; i++){
		uint8_t n = num % 10;
		if(num)
			TM1638_DisplayNumber(i,n);
		else
			TM1638_DisplayNumber_raw(i,0);
			
		num /= 10;
	}
	
}

void TM1638_DisplayNumber(fixedPoint_t num, uint8_t offset, uint8_t len, uint8_t dec_cnt){
	if(num.decPos > dec_cnt){
		num.number /= pow(10, num.decPos - dec_cnt);
		num.decPos = dec_cnt;
	}
	for(uint8_t i = 0; i < len; i++){
		if(num.number || i <= num.decPos)
			TM1638_DisplayNumber(offset + i, num.number % 10, i == num.decPos);	
		else
			TM1638_DisplayNumber_raw(offset + i, i == num.decPos ? 0x80 : 0);
		
		num.number /= 10;
	}
}
