/*
 * IncFile1.h
 *
 * Created: 9/20/2013 12:41:56 AM
 *  Author: Nick Cox, adapted from code by Tim Martin
 */ 

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef USART_H_
#define USART_H_

#define bufferSize 64

typedef struct{
	uint8_t *buf_start;
	uint8_t *buf_end;
	uint8_t *buf_head;
	uint8_t *buf_tail;
	uint8_t data[bufferSize];
}buf_t;

buf_t RXC0_buf;
buf_t TXC0_buf;

FILE usartC0_stream;

void buf_write(buf_t* buf, char data);
uint8_t buf_read(buf_t *buf);
uint8_t data_in_buf(buf_t *buf);
void usart_putc(char c);
char usart_getc();
void usart_init();
#endif /* USART_H_ */