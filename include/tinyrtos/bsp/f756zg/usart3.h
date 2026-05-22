/**
  ******************************************************************************
  * @file    usart3.h
  * @author  Ian Wilkey
  * @brief   A compact, preemptive priority RTOS kernel for ARM Cortex-M, 
  *          written from scratch in C.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Ian Wilkey
  *
  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is
  * furnished to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in all
  * copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  * SOFTWARE.
  ******************************************************************************
  */

#ifndef _RTOSK_BSP_F756ZG_USART3_H_
#define _RTOSK_BSP_F756ZG_USART3_H_

#include <stdint.h>
#include <stm32f7xx.h>

/**
 * This file strictly concerns GPIO on Nucleo-F756ZG's PORT D, where the TX and
 * RX lines on USART3 are located.
 */
#define BSP_PORT_D GPIOD

/**
 * CMSIS's macro for locating the USART3 peripheral on the target board.
 */
#define BSP_USART3 USART3

/**
 * USART3 peripheral TX on the Nucleo-F756ZG is located here on PORT D.
 */
#define BSP_USART3_GPIO_TX 8UL

/**
 * USART3 peripheral RX on the Nucleo-F756ZG is located here on PORT D.
 */
#define BSP_USART3_GPIO_RX 9UL

/**
 * Initializes USART3 on Nucleo-F756ZG over ST-LINK virtual COM port at given BAUD.
 * @author Ian Wilkey
 */
void rtosk_bsp_f756zg_usart3_init(uint32_t baud);

/**
 * Writes one character out through TX USART3 on Nucleo-F756ZG.
 * @author Ian Wilkey
 */
void rtosk_bsp_f756zg_usart3_write_char(char c);

/**
 * Writes one string out through TX USART3 on Nucleo-F756ZG.
 * @author Ian Wilkey
 */
void rtosk_bsp_f756zg_usart3_write_string(const char *s);

/**
 * Enables USART3 on Nucleo-F756ZG RX interrupt.
 * @author Ian Wilkey
 */
void rtosk_bsp_f756zg_uart_enable_rx_interrupt(void);

/**
 * Weak ISR hook called when USART3 on Nucleo-F756ZG recieves a byte.
 * 
 * Override this in application code to pass recieved data into the RTOS, usually by using a queue from the ISR.
 * @author Ian Wilkey
 */
void rtosk_bsp_f756zg_usart3_rx_callback_from_isr(uint8_t c);

#endif /// _RTOSK_BSP_F756ZG_USART3_H_
