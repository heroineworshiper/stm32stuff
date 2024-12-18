/*
 * STM32F4 Wifi flight controller
 * Copyright (C) 2013 Adam Williams <broadcast at earthling dot net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

// You must define ENABLE_PRINT to use it



#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "settings.h"
#include "stm32f4xx_usart.h"

#define TRACE trace(__FILE__, __FUNCTION__, __LINE__, 1);
#define TRACE2 trace(__FILE__, __FUNCTION__, __LINE__, 0);
#define UART_BUFFER_SIZE 4096


extern const char hex_table[];

typedef struct
{
	int uart_read_ptr;
    int uart_write_ptr;
	int uart_size;
	unsigned char uart_buffer[UART_BUFFER_SIZE];

	unsigned char input;
	int got_input;
	int need_lf;
} uart_t;

extern uart_t uart;

void init_uart();
// after corruption by interrupts
void reset_uart();

#ifdef ENABLE_PRINT

#define HANDLE_UART_OUT \
	if((USART6->SR & USART_FLAG_TC) != 0 && \
		uart.uart_size > 0) \
	{ \
		USART6->DR = uart.uart_buffer[uart.uart_read_ptr++]; \
        if(uart.uart_read_ptr >= UART_BUFFER_SIZE) \
            uart.uart_read_ptr = 0; \
        uart.uart_size--; \
	}

void send_uart(unsigned char c);
void print_text(const char *text);
int sprint_number(unsigned char *dst, int number, int maxlen);
void print_float(float number);
void print_fixed(int number);
void print_fixed_nospace(int number);
void print_number(int number);
void print_number_nospace(int number);
void print_hex(uint32_t number);
void print_hex1(unsigned char number);
void print_hex2(unsigned char number);
void print_buffer(const unsigned char *buf, int len);
void print_buffer16(const uint16_t *buf, int len);
void flush_uart();
void print_lf();

#else

#define HANDLE_UART_OUT {}

#define send_uart(c) {}
#define print_text(text) {}
#define sprint_number(dst, number, maxlen) {}
#define print_float(number) {}
#define print_fixed(number) {}
#define print_fixed_nospace(number) {}
#define print_number(number) {}
#define print_number_nospace(number) {}
#define print_hex(number) {}
#define print_hex1(number) {}
#define print_hex2(number) {}
#define print_buffer(buf, len) {}
#define print_buffer16(buf, len) {}
#define flush_uart() {}
#define print_lf() {}

#endif



//void handle_uart();




#define UART_EMPTY \
	(uart.uart_offset >= uart.uart_size)

#define TRY_UART(x, result) \
	if((USART6->SR & USART_FLAG_TC) != 0) \
	{ \
		USART6->DR = (x); \
		result = 1; \
	} \
	else \
	{ \
		result = 0; \
	}


#define uart_got_input() (uart.got_input)


#define uart_get_input() \
({ \
	uart.got_input = 0; \
	uart.input; \
})


void trace(const char *file, const char *function, int line, int flush_it);
// Read byte with blocking
unsigned char read_char();
void send_uart_binary(const unsigned char *text, int size);



#endif




