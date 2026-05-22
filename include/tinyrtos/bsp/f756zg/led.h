/**
  ******************************************************************************
  * @file    led.h
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

#ifndef _RTOSK_BSP_F756ZG_GPIO_PORT_B_H_
#define _RTOSK_BSP_F756ZG_GPIO_PORT_B_H_

#include <stdint.h>
#include <stm32f7xx.h>

/**
 * This file strictly concerns GPIO on Nucleo-F756ZG's PORT B, where the on-board
 * LEDs are located.
 */
#define BSP_PORT_B GPIOB

/**
 * All possible on-board LEDs on the Nucleo-F756ZG.
 * 
 * LED 0 = GREEN
 * 
 * LED 1 = BLUE
 * 
 * LED 2 = RED
 * @author Ian Wilkey
 */
typedef enum {
    LED_0 = 0UL,
    LED_1,
    LED_2
} bsp_led_t;

/**
 * Initializes the on-board LEDs for the Nucleo-F756ZG.
 * @author Ian Wilkey
 */
void rtosk_bsp_f756zg_on_board_led_init(void);

/**
 * Sets the state of some F756ZG on-board LED to the given state, which should be one of 0U or 1U.
 * @author Ian Wilkey
 */
void rtosk_bsp_f756zg_set_on_board_led(const bsp_led_t led, const uint8_t state);

/**
 * Toggles the current state of some F756ZG on-board LED.
 * @author Ian Wilkey
 */
void rtosk_bsp_f756zg_toggle_on_board_led(const bsp_led_t led);

#endif /// _RTOSK_BSP_F756ZG_GPIO_PORT_B_H_
