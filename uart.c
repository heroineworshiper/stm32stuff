/*
 * STM32F4 Wifi flight controller
 * Copyright (C) 2012 Adam Williams <broadcast at earthling dot net>
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

#include "linux.h"
#include "uart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "arm_math2.h"
#include "misc.h"

const char hex_table[] = 
{
	'0', '1', '2', '3', '4', '5', '6', '7', 
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

uart_t uart;

void reset_uart()
{
    bzero(&uart, sizeof(uart_t));
}

void init_uart()
{
	USART_InitTypeDef USART_InitStructure;
	
	bzero(&uart, sizeof(uart_t));

	uart.need_lf = 1;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
	
/* Connect PXx to USARTx_Tx*/
  	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);

/* Connect PXx to USARTx_Rx*/
  	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

/* Configure USART Tx as alternate function  */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

/* Configure USART Rx as alternate function  */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);


	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
/* USART configuration */
  	USART_Init(USART6, &USART_InitStructure);
/* Enable USART */
  	USART_Cmd(USART6, ENABLE);

/* Enable the UART Interrupt */
#if defined(USE_OUTBOARD_IMU) || \
	defined(USE_OUTBOARD_GYRO) || \
	defined(BLUETOOTH_PASSTHROUGH)
 	NVIC_InitTypeDef NVIC_InitStructure;
 	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 	NVIC_Init(&NVIC_InitStructure);
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
#endif // defined(USE_OUTBOARD_IMU) || defined(USE_OUTBOARD_GYRO) || defined(BLUETOOTH_PASSTHROUGH)

}


#ifdef ENABLE_PRINT
void send_uart(unsigned char c)
{
	if(uart.uart_size < UART_BUFFER_SIZE)
    {
		uart.uart_buffer[uart.uart_write_ptr++] = c;
        if(uart.uart_write_ptr >= UART_BUFFER_SIZE)
            uart.uart_write_ptr = 0;
        uart.uart_size++;
	    if(c == '\n')
		    uart.need_lf = 0;
    }
}


#ifdef BLUETOOTH_PASSTHROUGH
void USART6_IRQHandler(void)
{
	unsigned char c = USART6->DR;
	uart.input = c;
	uart.got_input = 1;
}
#endif // BLUETOOTH_PASSTHROUGH


void print_text(const char *text)
{
	int i = 0;
	while(text[i] != 0) send_uart(text[i++]);

}

void print_digit(int *number, 
	int *force, 
	unsigned char **ptr, 
	unsigned char *dst, 
	int maxlen, 
	int x)
{
	if(*number >= x || *force) 
	{ 
		*force = 1; 
		if(*ptr - dst < maxlen - 1) 
		{ 
			*(*ptr)++ = '0' + *number / x; 
		} 
		(*number) %= x; 
	}
}

int sprint_number(unsigned char *dst, int number, int maxlen)
{
	unsigned char *ptr = dst;
	int force = 0;

	if(number < 0)
	{
		if(ptr - dst < maxlen - 1)
		{
			*ptr++ = '-';
		}
		number = -number;
	}

	
	print_digit(&number, &force, &ptr, dst, maxlen, 1000000000);
	print_digit(&number, &force, &ptr, dst, maxlen, 100000000);
	print_digit(&number, &force, &ptr, dst, maxlen, 10000000);
	print_digit(&number, &force, &ptr, dst, maxlen, 1000000);
	print_digit(&number, &force, &ptr, dst, maxlen, 100000);
	print_digit(&number, &force, &ptr, dst, maxlen, 10000);
	print_digit(&number, &force, &ptr, dst, maxlen, 1000);
	print_digit(&number, &force, &ptr, dst, maxlen, 100);
	print_digit(&number, &force, &ptr, dst, maxlen, 10);
	
	if(ptr - dst < maxlen - 1)
	{
		*ptr++ = '0' + number % 10;
	}

	*ptr = 0;
	return ptr - dst;
}

void print_number_nospace(int number)
{
	char buffer[16];
	int len = sprint_number(buffer, number, sizeof(buffer));
	print_text(buffer);
}

void print_number(int number)
{
	print_number_nospace(number);
	send_uart(' ');
}

void print_float(float number)
{
	unsigned char buffer[16];
	int maxlen = sizeof(buffer);
	unsigned char *ptr = buffer;

	int whole = (int)number;
	if(whole == 0 && number < 0) send_uart('-');
	print_number_nospace(whole);

	
	if(ABS(number - whole) > 0)
	{
		print_text(".");
		int remainder = (int)(ABS(number - whole) * 1000000);
		
		int force = 1;
		print_digit(&remainder, &force, &ptr, buffer, maxlen, 100000);
		print_digit(&remainder, &force, &ptr, buffer, maxlen, 10000);
		print_digit(&remainder, &force, &ptr, buffer, maxlen, 1000);
		print_digit(&remainder, &force, &ptr, buffer, maxlen, 100);
		print_digit(&remainder, &force, &ptr, buffer, maxlen, 10);

		if(ptr - buffer < maxlen - 1)
		{
			*ptr++ = '0' + remainder % 10;
		}
        *ptr++ = 0;
		print_text(buffer);
	}
	send_uart(' ');
}

void print_fixed_nospace(int number)
{
	if(number < 0) 
	{
		send_uart('-');
		number = -number;
	}
	
	print_number_nospace(number / 256);
	int fraction = ABS(number % 256);
	char string[1];
	if(fraction)
	{
		send_uart('.');
		fraction = fraction * 1000 / 256;
		send_uart('0' + (fraction / 100));
		send_uart('0' + ((fraction / 10) % 10));
		send_uart('0' + (fraction % 10));
	}
}

void print_fixed(int number)
{
	print_fixed_nospace(number);
	send_uart(' ');
}


void print_hex(uint32_t number)
{
	char buffer[10];
	int i;
	int force = 0;
	char *dst = buffer;
	
	for(i = 0; i < 8; i++)
	{
		uint8_t code = (number >> 28) & 0xf;
		
		if(code > 0 || force || i == 7)
		{
			force = 1;
			*dst++ = hex_table[code];
		}
		
		number <<= 4;
	}

	*dst++ = ' ';
    *dst++ = 0;
	print_text(buffer);
}

void print_hex2(unsigned char number)
{
	char buffer[6];
	int i = 0;
	
//	buffer[i++] = '0';
//	buffer[i++] = 'x';
	buffer[i++] = hex_table[(number >> 4) & 0xf];
	buffer[i++] = hex_table[number & 0xf];
//    buffer[i++] = ' ';
    buffer[i++] = 0;
	print_text(buffer);
}

void print_hex1(unsigned char number)
{
	char buffer[3];
	int i = 0;
	
	buffer[i++] = hex_table[number & 0xf];
    buffer[i++] = ' ';
    buffer[i++] = 0;
	print_text(buffer);
}

void print_buffer(const unsigned char *buf, int len)
{
	char buffer[UART_BUFFER_SIZE];
	int i;
	char *ptr = buffer;
	for(i = 0; i < len && ptr - buffer < UART_BUFFER_SIZE - 1; i++)
	{
		if(i > 0)
		{
			*ptr++ = ' ';
		}
		
		*ptr++ = hex_table[(buf[i] >> 4) & 0xf];
		*ptr++ = hex_table[buf[i] & 0xf];
        *ptr++ = ' ';
	}
	
	*ptr = 0;
	print_text(buffer);
	print_lf();
}

void print_buffer16(const uint16_t *buf, int len)
{
	char buffer[UART_BUFFER_SIZE];
	int i;
	char *ptr = buffer;
	for(i = 0; i < len && ptr - buffer < UART_BUFFER_SIZE - 1; i++)
	{
		if(i > 0)
		{
			*ptr++ = ' ';
		}
		
		*ptr++ = hex_table[(buf[i] >> 12) & 0xf];
		*ptr++ = hex_table[(buf[i] >> 8) & 0xf];
		*ptr++ = hex_table[(buf[i] >> 4) & 0xf];
		*ptr++ = hex_table[buf[i] & 0xf];
        *ptr++ = ' ';
	}
	
	*ptr = 0;
	print_text(buffer);
	print_lf();
}

void flush_uart()
{
	while(uart.uart_size > 0) HANDLE_UART_OUT
}

void print_lf()
{
	send_uart('\n');
}

/*
 * void handle_uart()
 * {
 * 	if(USART_GetFlagStatus(USART6, USART_FLAG_TC) == SET &&
 * 		uart_offset < uart_size)
 * 	{
 * 		USART_SendData(USART6, uart_buffer[uart_offset++]);
 * 	}
 * 
 * 	if(USART_GetFlagStatus(USART6, USART_FLAG_RXNE) == SET)
 * 	{
 * 		got_input = 1;
 * 		input = USART_ReceiveData(USART6);
 * 	}
 * }
 */

#endif // ENABLE_PRINT

unsigned char read_char()
{
	while(!uart_got_input())
	{
		HANDLE_UART_OUT
	}
	return uart_get_input();
}


void trace(const char *file, const char *function, int line, int flush_it)
{
	if(uart.need_lf)
	{
		print_text("\n");
	}
	
	print_text(file);
	print_text(": ");
	print_text(function);
	print_text(" ");
	print_number_nospace(line);
	print_text(": ");
	if(flush_it) flush_uart();
	uart.need_lf = 1;
}


