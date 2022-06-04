/*
 * main.h
 *
 * Created: 4/23/2021 1:58:53 PM
 * Author : sanmveg saini
 * Company: ADVANCE INSTRUMENTATION 'n' CONTROL SYSTEMS
 * Website: www.aics.co.in
 */ 


#ifndef MAIN_H_
#define MAIN_H_


#define F_CPU 1000000U

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <avr/sfr_defs.h>


#define RISING_EDGE(a) ((a.curr != a.prev) && (a.curr == 1))
#define FALLING_EDGE(a) ((a.curr != a.prev) && (a.curr == 0))
#define VOLTAGE ((uint8_t)0)
#define CURRENT ((uint8_t)1)

struct fixedPoint_t{
	uint32_t number;
	int8_t decPos;
};

struct callibration_data_t{
	fixedPoint_t in_from, in_to;
	fixedPoint_t out_from, out_to;
};

extern callibration_data_t voltage_callibration, current_callibration;

extern volatile bool do_callibrate_current;
extern volatile bool do_callibrate_voltage;

/*******************************************************
				LED related functions
*******************************************************/
void LEDInit();
void LEDon(uint8_t pos);
void LEDAllon();
void LEDAlloff();
void sw_dbd_test();

/*******************************************************
				EEPROM related functions
*******************************************************/
void EEPROM_write(unsigned int uiAddress, unsigned char ucData);
unsigned char EEPROM_read(unsigned int uiAddress);


/************************************************************************/
/* UART related functions                                               */
/************************************************************************/
void UART_init();
void UART_send(uint8_t byte);
void UART_send(const char *str);
void UART_send(const fixedPoint_t * const n);


/*******************************************************
				TM1638 related functions
*******************************************************/
void conuting();
void TM1638_Init();
uint8_t SPI_MasterTransmit(char cData);
void TM1638_SendCommand(char data);
void TM1638_DisplayNumber(uint8_t index, uint8_t num, bool decimal = false );
void TM1638_DisplayNumber_raw(uint8_t index, uint8_t num);
void TM1638_DisplayNumber(uint32_t num);
void TM1638_DisplayNumber(fixedPoint_t num, uint8_t offset, uint8_t len,uint8_t dec_cnt);
void TM1638_reset();
void showdata(uint16_t voltage, uint16_t current);


/*******************************************************
			   Timer 0 related functions
*******************************************************/
void TMR0_Init();
uint32_t millis();
extern volatile uint8_t did_tick;




/*******************************************************
			  Buttons related functions
*******************************************************/
struct button{
	bool curr, prev;
};
extern volatile button b1, b2, b3, b4;
void Buttons_init();
void Buttons_scan();// 104,39,99;110,64,92;116,67,95;115,62,89;113,63,92

/*****************************************************
           Analog Read related functions
******************************************************/
void analogInit();
uint16_t readVoltage(); // return voltage in volts
uint16_t readCurrent(); // return current in mA


/*****************************************************
           Fixed Point related functions
******************************************************/
// Note num >= 0
uint8_t flotofix(fixedPoint_t * res, float num, uint8_t total_len);
float fixtoflo(fixedPoint_t fix);

/*****************************************************
           calibration related functions
******************************************************/
fixedPoint_t getNumber(fixedPoint_t initval, uint8_t hl);
void loadCallibrationData();
void callibrate(uint8_t what);


#endif /* MAIN_H_ */