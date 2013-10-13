/*
 * clock.c
 *
 * Created: 5/21/2012 5:28:55 PM
 *  Author: tmartin
 */ 


#include "clock.h"

void clock_init()
{	
	OSC.CTRL |= OSC_RC32MEN_bm | OSC_RC32KEN_bm;
	while(!(OSC.STATUS & OSC_RC32MRDY_bm) || !(OSC.STATUS & OSC_RC32KRDY_bm));

	CCP = CCP_IOREG_gc;
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
}

