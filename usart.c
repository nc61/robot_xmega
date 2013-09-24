/*
 * robot_xmega.c
 *
 * Created: 9/19/2013 9:12:26 PM
 *  Author: Nick Cox, adapted from code by Tim Martin
 */ 

#include "usart.h"

buf_t RXC0_buf = {&RXC0_buf.data[0],&RXC0_buf.data[bufferSize-1],&RXC0_buf.data[0],&RXC0_buf.data[0],{0}};
buf_t TXC0_buf = {&TXC0_buf.data[0],&TXC0_buf.data[bufferSize-1],&TXC0_buf.data[0],&TXC0_buf.data[0],{0}};
	
void buf_write(buf_t* buf, char data)
{
	if (buf->buf_head == buf->buf_end)
	{
		*buf->buf_head = data;
		 buf->buf_head = buf->buf_start;
	}
	else *buf->buf_head++ = data;
}

uint8_t buf_read(buf_t *buf)
{
	if (buf->buf_tail == buf->buf_end)
	{
		char data = *buf->buf_tail;
		buf->buf_tail = buf->buf_start;
		return data;
	}
	else return *buf->buf_tail++;
}

FILE usartC0_stream = FDEV_SETUP_STREAM(usartC0_putc, usartC0_getc, _FDEV_SETUP_RW);

void usart_putc(buf_t *buf, char c)
{
	cli();
	buf_write((buf_t *)buf, c);
	USARTC0.CTRLA &= ~USART_DREINTLVL_gm;
	USARTC0.CTRLA |= USART_DREINTLVL_LO_gc;	//setting this bit should cause the DRE interrupt to be triggered
	sei();
}

char usart_getc(buf_t *buf)
{
	char data;
	cli();
	data = buf_read((buf_t *) buf);
	sei();
	return data;
}

uint8_t data_in_buf(buf_t *buf)
{
	if(buf->buf_head == buf->buf_tail) return 0;
	else return 1;
}

void usart_init()
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

ISR(USARTC0_RXC_vect)
{
	uint8_t data = USARTC0.DATA;
	buf_write(&RXC0_buf, data);
}

ISR(USARTC0_DRE_vect)
{
	cli();
	if(data_in_buf(&TXC0_buf))
	{
		USARTC0.DATA = buf_read(&TXC0_buf);
	}
	else USARTC0.CTRLA &= ~USART_DREINTLVL_gm;
	sei();
}