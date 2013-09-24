/*
 * robot_xmega.c
 *
 * Created: 9/19/2013 9:12:26 PM
 *  Author: Nick
 */ 



#include "robot_xmega.h"
#include <avr/io.h>

int main(void)
{
	clock_init();
	usart_init();
	sei();
	uint8_t usb_data = 0;
    
	while(1)
    {
         if(data_in_buf(&RXC0_buf))
         {
	         //read in a character from the USB buffer
	         fscanf(&usartC0_stream,"%c",&usb_data);
	         
			//print out the received character to the screen
			fprintf(&usartC0_stream,"%c",usb_data);
			 
         }
    }
}
