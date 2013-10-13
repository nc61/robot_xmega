/*
 * motor_test.c
 *
 * Created: 10/12/2013 9:07:07 PM
 *  Author: Nick
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "robot_xmega.h"

int main(void){
	clock_init();
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;
	sei();
	TC_init();
	pivot_left(50);
	while(1){
		asm("NOP");
	}
}