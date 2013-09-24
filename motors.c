/*
 * motors.c
 *
 * Created: 9/20/2013 1:21:33 AM
 *  Author: Nick
 */ 

#include <avr/io.h>
#include <math.h>

#define F_CPU 32000000
#define MOTOR_FWD 0
#define MOTOR_BACK 1
#define MOTOR_LEFT 2
#define MOTOR_RIGHT 3

uint8_t frequency;

void TC_init(){
	PORTD.DIRSET = 0xF;	//CCA, dir1, CCC, dir2 pins
	TCD0.CTRLB = 0x50 | TC_WGMODE_SINGLESLOPE_gc;	//Enable CCA, CCC and single slope mode
	TCD0.PER = floor(F_CPU/(2*frequency) + .5);
}

void set_motors(uint16_t CCA, uint8_t dir)
{
	TCD0.CCA = CCA;
	switch (dir)
	{
	case MOTOR_FWD:
		PORTD.OUTSET = PIN1_bm | PIN3_bm; //Set left motor fwd and right motor fwd
		break;
	case MOTOR_BACK:
		PORTD.OUTCLR = PIN1_bm | PIN3_bm; //Set both motors backwards
		break;
	case MOTOR_LEFT:
		PORTD.OUTSET = PIN3_bm;	//Set left motor backwards and right motor forward
		PORTD.OUTCLR = PIN1_bm;
		break;
	case MOTOR_RIGHT:
		PORTD.OUTSET = PIN1_bm;	//Set right motor backwards and left motor forward
		PORTD.OUTCLR = PIN3_bm; 
		break;
	
	TCD0.CTRLA = TC_CLKSEL_DIV64_gc; //Start the motors
	}
}

void stop_motors(void)
{
	TCD0.CTRLA = TC_CLKSEL_OFF_gc;
}