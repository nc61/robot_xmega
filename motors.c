/*
 * motors.c
 *
 * Created: 9/20/2013 1:21:33 AM
 *  Author: Nick Cox
 *	Description: Motor controller for MC33929 motor driver.
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include "motors.h"
#include "usart.h"
#define F_CPU 32000000

//Set points for motors
volatile uint16_t left_motor_set_point;
volatile uint16_t right_motor_set_point;

//Acceleration value with which to approach set points
volatile uint8_t acceleration = 25;
volatile uint8_t deceleration = 50;

//When stop reply is 'y' (yes), the Xmega will send a serial 
//	signal to the computer when motors are stopped. This is convenient
//	because it avoids unpredictable delays.
volatile char stop_reply = 'n';

void TC_init()
{
	/*
		Pins used for speed control:
		PD4 ---- /M1D2
		PD5 ---- /M2D2
	*/
	PORTD.DIRSET = 0x30;	//CCA,CCB
	TCD1.CTRLA = TC_CLKSEL_DIV1_gc;
	
	//CCA will control the left motor and CCB will control the right motor
	TCD1.CTRLB = 0x30 | TC_WGMODE_SINGLESLOPE_gc;
	TCD1.PER = floor((F_CPU)/(10000) + .5);
	/*
		Pins used for direction control:
		PB0 ---- M1IN1
		PB1 ---- M1IN2
		PB2 ---- M2IN1
		PB2 ---- M2In2
		
		Pin 1:0 control left motor direction as follows:
		pin0 = 1 && pin1 = 0 ==> Forward
		pin0 = 0 && pin1 = 1 ==> Backward
		Pin 3:2 control right motor in the same way
	*/
	PORTB.DIRSET = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm;
	
	//Initialize timer used for acceleration
	TCE0.CTRLB = TC_WGMODE_NORMAL_gc;
	TCE0.PER = 5000;
	TCE0.INTCTRLA = TC_OVFINTLVL_MED_gc;
	TCE0.CTRLA = TC_CLKSEL_DIV64_gc;
}

void set_motor(char motor, uint8_t duty_cycle, uint8_t dir)
{
	uint16_t set_point = floor(TCD1.PER*((float)duty_cycle/100) + .5);
	
	if (motor == 'l')
	{
		if (dir == 'f')
		{
			PORTB.OUTSET = PIN0_bm;
			PORTB.OUTCLR = PIN1_bm;
			left_motor_set_point = set_point;
		}
		else if (dir == 'b')
		{
			PORTB.OUTSET = PIN1_bm;
			PORTB.OUTCLR = PIN0_bm;
			left_motor_set_point = set_point;
		}
		else left_motor_set_point = 0;
	}
	
	else if (motor == 'r')
	{
		if (dir == 'f')
		{
			PORTB.OUTSET = PIN2_bm;
			PORTB.OUTCLR = PIN3_bm;
			right_motor_set_point = set_point;
		}
		else if (dir == 'b')
		{
			PORTB.OUTSET = PIN3_bm;
			PORTB.OUTCLR = PIN2_bm;
			right_motor_set_point = set_point;
		}
		else right_motor_set_point = 0;
	}
	
	else if (motor == 'b')
	{
		if (dir == 'f')
		{
			PORTB.OUTSET = PIN0_bm | PIN2_bm;
			PORTB.OUTCLR = PIN1_bm | PIN3_bm;
			right_motor_set_point = set_point;
			left_motor_set_point = set_point;
		}
		else if (dir == 'b')
		{
			PORTB.OUTSET = PIN1_bm | PIN3_bm;
			PORTB.OUTCLR = PIN0_bm | PIN2_bm;
			right_motor_set_point = set_point;
			left_motor_set_point = set_point;
		}
		else
		{
			right_motor_set_point = 0;
			left_motor_set_point = 0;
		}
	}
}

void pivot_right(uint8_t duty_cycle)
{
	uint16_t set_point = floor(TCD1.PER*((float)duty_cycle/100) + .5);
	
	//See pin descriptions in TC_init
	PORTB.OUTSET = PIN0_bm | PIN3_bm;
	PORTB.OUTCLR = PIN1_bm | PIN2_bm;
	
	left_motor_set_point = set_point;
	right_motor_set_point = set_point;
}

void pivot_left(uint8_t duty_cycle)
{
	uint16_t set_point = floor(TCD1.PER*((float)duty_cycle/100) + .5);
	
	//See pin descriptions in TC_init
	PORTB.OUTSET = PIN1_bm | PIN2_bm;
	PORTB.OUTCLR = PIN0_bm | PIN3_bm;
	
	left_motor_set_point = set_point;
	right_motor_set_point = set_point;
}

void stop_motors()
{
	left_motor_set_point = 0;
	right_motor_set_point = 0;
}

//This ISR accelerates motors toward their respective set points
ISR(TCE0_OVF_vect)
{
	volatile uint16_t speed;
		
		if (TCD1.CCA < left_motor_set_point)
		{
			speed = TCD1.CCA + acceleration;
			TCD1.CCA = (speed >= left_motor_set_point) || (speed > TCD1.PER) ? left_motor_set_point : speed;
		}
		
		else if (TCD1.CCA > left_motor_set_point){
			speed = TCD1.CCA - deceleration;
			TCD1.CCA = (speed <= left_motor_set_point) || (speed > TCD1.PER) ? left_motor_set_point : speed;
		}
		
		if (TCD1.CCB < right_motor_set_point)
		{
			speed = TCD1.CCB + acceleration;
			TCD1.CCB = (speed >= right_motor_set_point) || (speed > TCD1.PER) ? right_motor_set_point : speed;
		}
		else if (TCD1.CCB > right_motor_set_point){
			speed = TCD1.CCB - deceleration;
			TCD1.CCB = (speed <= right_motor_set_point) || (speed > TCD1.PER) ? right_motor_set_point : speed;
		}
		
		//This alerts the computer when the motors are stopped
		if (stop_reply == 'y'){
			if (!(TCD1.CCA || TCD1.CCB))
			{
				usart_putc(0xAA);
				stop_reply = 'n';
			}
		}
}

