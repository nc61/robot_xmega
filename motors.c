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


void TC_init(){
	PORTD.DIRSET = 0x30;	//CCA,CCB
	TCD1.CTRLA = TC_CLKSEL_DIV1_gc;
	TCD1.CTRLB = 0x30 | TC_WGMODE_DSTOP_gc;	//Enable CCA, CCB and dual slope mode
	TCD1.PER = floor((F_CPU)/(2*12000) + .5);
	PORTB.DIRSET = PIN0_bm | PIN1_bm;
}

void set_motors(uint8_t duty_cycle, uint8_t dir)
{
	switch (dir)
	{
		case MOTOR_FWD:
			PORTB.OUTSET = PIN0_bm | PIN1_bm; //Set left motor fwd and right motor fwd
			break;
		case MOTOR_BACK:
			PORTB.OUTCLR = PIN0_bm | PIN1_bm; //Set both motors backwards
			break;
		case MOTOR_LEFT:
			PORTB.OUTSET = PIN1_bm;	//Set left motor backwards and right motor forward
			PORTB.OUTCLR = PIN0_bm;
			break;
		case MOTOR_RIGHT:
			PORTB.OUTSET = PIN0_bm;	//Set right motor backwards and left motor forward
			PORTB.OUTCLR = PIN1_bm; 
			break;
	}
	TCD1.CCA = floor(TCD1.PER*((float)duty_cycle/100) + .5);
}

