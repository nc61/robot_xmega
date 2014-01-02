/*
 * robot_xmega.c
 *
 * Created: 9/19/2013 9:12:26 PM
 *  Author: Nick Cox
 *	Description: Interpret and act upon serial commands
 */ 



#include "robot_xmega.h"
#include <avr/io.h>

//global variables from motors.c
extern uint8_t acceleration;
extern uint8_t deceleration;
extern uint16_t right_motor_set_point;
extern uint16_t left_motor_set_point;
extern char stop_reply;

int main(void)
{
	clock_init();
	usart_init();
	TC_init();
	sei();
	PORTB.DIRSET = 0xF;
	PORTB.OUT = 0xA;
	char cmd_byte = 0;
    
	while(1)
    {
		if(data_in_buf(&RXC0_buf))
        {
	        //read in a character from the USB buffer
	        cmd_byte = usart_getc();
			PORTB.OUT = (cmd_byte & 0x0F);
			
			//Command 'd' - Debug
			if (cmd_byte == '!')
			{
				usart_putc('r');
			}
			//Command 'p' - Pivot
			if (cmd_byte == 'p')
			{
				uint8_t direction = usart_getc();
				uint8_t duty_cycle = usart_getc();
				uint8_t immediate = usart_getc();
				uint8_t imm = 0;
				if (immediate == 'y') imm = 1;
				if (direction == 'l'){
					pivot_left(duty_cycle, imm);
				}
				else if (direction == 'r'){
					pivot_right(duty_cycle, imm);
				}
			}
			
			//Command 'm' - Motor
			else if (cmd_byte == 'm')
			{
				char motor = usart_getc();
				uint8_t direction = usart_getc();
				uint8_t duty_cycle = usart_getc();
				uint8_t immediate = usart_getc();
				uint8_t imm;
				if (immediate == 'y') imm = 1;
				set_motor(motor, duty_cycle, direction, imm);
			}
			
			//Command 's' - Stop motors
			else if (cmd_byte == 's')
			{
				//Description of stop_reply found in motors.c
				stop_reply = usart_getc();
				stop_motors();
			}
			
			//Command 'a' - Set acceleration value (1-255)
			else if (cmd_byte == 'a')
			{
				uint8_t acc = usart_getc();
				if ((acc <= 255) && (acc > 0))
				{
					acceleration = acc;
				}
			}
			
			//Command 'd' - Set deceleration value (1-255)
			else if (cmd_byte == 'd')
			{
				uint8_t dec = usart_getc();
				if ((dec <= 255) && (dec > 0))
				{
					deceleration = dec;
				}
			}
			else if (cmd_byte == 'i')
			{
				right_motor_set_point = 0;
				left_motor_set_point = 0;
				TCD1.CCA = 0;
				TCD1.CCB = 0;
			}
			else if (cmd_byte == 'c')
			{
				uint8_t left_duty_cycle = usart_getc();
				uint8_t left_direction = usart_getc();
				uint8_t right_duty_cycle = usart_getc();
				uint8_t right_direction = usart_getc();
				
				set_motor('l', left_duty_cycle, left_direction, 1);
				set_motor('r', right_duty_cycle, right_direction, 1);
			}
		//Reset to prepare for next command
		cmd_byte = 0;	
		}
	}
}