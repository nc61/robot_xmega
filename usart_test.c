/*
 * usart_test.c
 *
 * Created: 10/13/2013 7:25:25 PM
 *  Author: Nick
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "clock.h"

volatile uint8_t data = 0;

void usart_test_init()
{
	PORTC.DIR = PIN3_bm;								//Set port C pin 3 as an output
	PORTC.OUTSET = PIN3_bm;								//Set port C pin 3
	USARTC0.BAUDCTRLA = 0x6E;							//Baud rate to 57600Hz
	USARTC0.BAUDCTRLB = 0xA8;
	USARTC0.CTRLA = USART_RXCINTLVL_MED_gc;
	USARTC0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;      //Enable receiver and transmitter
	USARTC0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;
	PMIC.CTRL |= PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
}

int main(void)
{
	clock_init();
	usart_test_init();
	PORTB.DIRSET = 0xF;
	PORTB.OUTSET = 0xF1;
	sei();
	while(1)
	{
		asm("NOP");
	}
}

ISR(USARTC0_RXC_vect)
{
	PORTB.OUTSET = (data & 0xFF);
	data = USARTC0.DATA;
	data = data + 1;
	USARTC0.DATA = data;
}
