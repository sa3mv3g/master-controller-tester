/*
 * UART.cpp
 *
 * Created: 5/1/2021 7:08:08 AM
 * Author : sanmveg saini
 * Company: ADVANCE INSTRUMENTATION 'n' CONTROL SYSTEMS
 * Website: www.aics.co.in
 */ 

#include "main.h"

void UART_init(){
	// baud = 9600
	// 1 stop bit
	// 8 data bits
	// no parity
	UBRRH = (unsigned char)(12>>8);
	UBRRL = (unsigned char)12;
	UCSRA = 1<<U2X;
	UCSRB = (1<<RXEN)|(1<<TXEN);
	UCSRC = (1<<URSEL)|(3<<UCSZ0);
}

void UART_send(uint8_t byte){
	while ( !( UCSRA & (1<<UDRE)) );
	UDR = byte;
}

void UART_send(const char *str){
	for(uint8_t i = 0; str[i] != '\0'; i++) UART_send(str[i]);
}

void UART_send(const fixedPoint_t * const n){
	UART_send((uint8_t)((n->number)>>24));
	UART_send((uint8_t)((n->number)>>16));
	UART_send((uint8_t)((n->number)>>8));
	UART_send((uint8_t)(n->number));
	UART_send((uint8_t)n->decPos);
}
