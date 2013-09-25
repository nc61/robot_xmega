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
	TC_init();
	set_motors(50, 0);
	char cmd_byte = 0;
    
	while(1)
    {
		
        if(data_in_buf(&RXC0_buf))
        {
	        //read in a character from the USB buffer
	        cmd_byte = usart_getc();
		
			if (cmd_byte == 'm')
			{
				volatile uint8_t duty_cycle, dir;
				
				duty_cycle = usart_getc();
				
				if (duty_cycle <=0 || duty_cycle > 100)
				{
					duty_cycle = 0;
				}
				
				dir = usart_getc();
				set_motors(duty_cycle, dir);
			}
			
			cmd_byte = 0;
        }
    }
}
