/*
 * Bit bang a UART
 * Copyright (C) 2023 Adam Williams <broadcast at earthling dot net>
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


// this runs on the STM32F407 Discovery

// build with make sendtty.hex

// program with 
// cd /amazon/root/openocd-0.9.0/tcl/board
// openocd -f stm32f4discovery.cfg&
// telnet localhost 4444
// program /amazon/root/stm32stuff/sendtty.hex;reset run



#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_spi.h"
#include "linux.h"
#include "math.h"
#include "misc.h"

// generate a square wave to calibrate the timing
//#define DO_SQUARE
#define THE_PIN GPIO_Pin_9

// the test data
const uint8_t test_data[] = 
{
    'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\n', 
    'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\n', 
    'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\n', 
    'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\n', 
    'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\n', 
    'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\n', 
    'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\n', 
    'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\n', 
};

const uint8_t test_data_[] = 
{ 'a', 'b', 'c', '\r' };
#define TEST_SIZE sizeof(test_data)

#define REPEATS 10


#define SQUARE_SIZE 256

void main()
{
// CCM RAM
    uint16_t *waveform = (uint16_t*)0x10000000;
    uint16_t *start = waveform;
    uint16_t *ptr = waveform;
    uint8_t rand[] = { 
        0xcf, 0x8f, 0x35, 0xa5, 0xaf, 0x28, 0x4c, 0xdc, 
        0x84, 0x27, 0x36, 0x83, 0x57, 0x7c, 0xc4, 0xe5,
        0x00, 0x4c, 0x72, 0x7d, 0x53, 0x9f, 0x73, 0x46,
        0xc8, 0x89, 0x54, 0x2c, 0x6d, 0xd5, 0x63, 0x82,
        0x3b, 0x00
    };
    int i, j, k;


#ifndef DO_SQUARE
// generate waveform for test data
    for(k = 0; k < REPEATS; k++)
    {
        for(i = 0; i < TEST_SIZE; i++)
        {
// start bit
            *ptr++ = 0;
            *ptr++ = 0;
            for(j = 0; j < 8; j++)
            {
                if((test_data[i] & (1 << j)))
                {
                    *ptr++ = THE_PIN;
                    *ptr++ = THE_PIN;
                }
                else
                {
                    *ptr++ = 0;
                    *ptr++ = 0;
                }
            }

// 1 stop bit
            uint16_t *ptr1 = ptr;
            *ptr++ = THE_PIN;
            *ptr++ = THE_PIN;

// 2 stop bits
//           *ptr++ = THE_PIN;
//           *ptr++ = THE_PIN;

// randomize stop bit length from 1.5-2
//           int x = rand[(i + TEST_SIZE * k) % sizeof(rand)];
//           if((x & 0x1))
//               *ptr++ = THE_PIN;
//           if((x & 0x2))
//               *ptr++ = THE_PIN;
        }


// pad end of packet with 1
        for(i = 0; i < 32; i++)
            *ptr++ = THE_PIN;
    }
    uint16_t *end = ptr;
    
#else
// generate square wave
    for(i = 0; i < SQUARE_SIZE; i++)
    {
        if((i & 0x1))
            waveform[i] = THE_PIN;
        else
            waveform[i] = 0;
    }
    uint16_t *end = waveform + SQUARE_SIZE;
#endif




	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE |
            RCC_AHB1Periph_CCMDATARAMEN, 
		ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = 
        GPIO_Pin_9;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    ptr = waveform;
    while(1)
    {
        GPIOE->ODR = *ptr;
// 1 megbaud
        asm("nop");
// 500 kbaud
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
// 400 kbaud
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
#if 0
// 250 kbaud
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
// 230400 baud
        asm("nop");
        asm("nop");
        asm("nop");
// 115200 baud
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
#endif

        ptr++;
        if(ptr >= end) ptr = start;
    }
}
























