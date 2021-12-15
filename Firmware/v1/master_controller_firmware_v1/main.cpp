/*
 * master_controller_firmware_v1.cpp
 *
 * Created: 4/23/2021 10:49:02 AM
 * Author : sanmveg saini
 * Company: ADVANCE INSTRUMENTATION 'n' CONTROL SYSTEMS
 * Website: www.aics.co.in
 */ 

#include "main.h"

#define SETBIT(P,N) (P |= 1<<N)
#define UNSETBIT(P,N) (P &= ~(1<<N))

int uart_putchar(char c, FILE *stream);

FILE mystdout ;

void printfix(fixedPoint_t pt, const char s[]){
	printf("%s: (%lu, %d)", s, pt.number, pt.decPos);
}


fixedPoint_t map(callibration_data_t * what, uint16_t val){
	fixedPoint_t res;
	//printfix(what->in_from, "\nin_from");
	//printfix(what->in_to, ", in_to");
	//printfix(what->out_from, ", out_from");
	//printfix(what->out_to, ", out_to");
	float in_from = fixtoflo(what->in_from);
	float in_to = fixtoflo(what->in_to);
	float out_from = fixtoflo(what->out_from);
	float out_to = fixtoflo(what->out_to);
	float nval = (float)val;
	//printf("\nnval = %.2f", (double)nval);
	static float aa=0;
	if(nval > in_from){
		aa =  (nval - in_from);
		//printf(", aa1= %.2f", (double)aa);
		aa = aa * (out_to - out_from);
		//printf(", aa2= %.2f", (double)aa);
		aa = aa / (in_to - in_from);
		//printf(". aa3= %.2f", (double)aa);
		aa += out_from;
		//printf(", aa4= %2.f", (double)aa);
	}else{
		aa = 0.0f;
	}
	//printf(", aa = %f,", (double)aa);
	flotofix(&res, aa, 4);
	return res;
}

int main()
{
	
	mystdout.flags =_FDEV_SETUP_WRITE;
	mystdout.put = uart_putchar;
	mystdout.get = NULL;
	mystdout.udata = 0;
	
	
	#ifdef DEBUG
	DDRD |= (1<<DDD0) | (1<<DDD1) | (1<<DDD6) | (1<<DDD7);
	PORTD &= ~((1<<PORTD0)|(1<<PORTD1)|(1<<PORTD6)|(1<<PORTD7));
	#endif
	sei();
	loadCallibrationData();
	TMR0_Init();
	Buttons_init();
	analogInit();
	LEDInit();
	UART_init();
	TM1638_Init();
	TM1638_reset();
	
	LEDAllon();
	
	for(uint8_t i=0;i<3;i++)
	TM1638_DisplayNumber_raw(7,0x5f);
	TM1638_DisplayNumber_raw(6,0x06);
	TM1638_DisplayNumber_raw(5,0x58);
	TM1638_DisplayNumber_raw(4,0xed);
	TM1638_DisplayNumber_raw(3,0x58);
	TM1638_DisplayNumber_raw(2,0xdc);
	TM1638_DisplayNumber_raw(1,0x06);
	TM1638_DisplayNumber_raw(0,0x54);
	
	stdout = &mystdout;
	//#ifdef DEBUG
	UART_send("www.aics.co.in");
	//UART_send(&current_callibration.in_from);
	//UART_send(&current_callibration.in_to);
	//UART_send(&current_callibration.out_from);
	//UART_send(&current_callibration.out_to);
	//#endif
	//printf("current-calibration, in_from = %f, in_to = %f, out_from = %f, out_to = %f", (double)fixtoflo(current_callibration.in_from), (double)fixtoflo(current_callibration.in_to), (double)fixtoflo(current_callibration.out_from), (double)fixtoflo(current_callibration.out_to));

	
	_delay_ms(2000);
	
	printf("current_callibration.in_from, Number = %lu, decpos = %i\n", current_callibration.in_from.number, current_callibration.in_from.decPos);
	printf("current_callibration.in_to, Number = %lu, decpos = %i\n", current_callibration.in_to.number, current_callibration.in_to.decPos);
	printf("current_callibration.out_from, Number = %lu, decpos = %i\n", current_callibration.out_from.number, current_callibration.out_from.decPos);
	printf("current_callibration.out_to, Number = %lu, decpos = %i\n", current_callibration.out_to.number, current_callibration.out_to.decPos);
	
	do_callibrate_current = false;
	do_callibrate_voltage = false;
	
	while(1){
		
		if(do_callibrate_current){
			callibrate(CURRENT);
			do_callibrate_current = false;
			do_callibrate_voltage = false;
		}else if(do_callibrate_voltage){
			callibrate(VOLTAGE);
			do_callibrate_current = false;
			do_callibrate_voltage = false;
		}
		
	
		fixedPoint_t curr = map(&current_callibration, readCurrent());
		fixedPoint_t voll = map(&voltage_callibration, readVoltage());
		
		#ifdef DEBUG
		fixedPoint_t num = {readCurrent(),0};
		UART_send(&num);
		#endif
		
		sw_dbd_test();
		TM1638_DisplayNumber(curr, 0,4,2);
		TM1638_DisplayNumber(voll, 5,3,1);
		TM1638_DisplayNumber_raw(4,0);
		//printf("%.2f\n", (double)fixtoflo(curr));
	}
	

}


int uart_putchar(char c, FILE *stream)
{
	if (c == '\n')
	uart_putchar('\r', stream);
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = c;
	return 0;
}