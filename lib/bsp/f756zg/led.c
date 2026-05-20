/**
  ******************************************************************************
  * @file    led.c
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

#include "led.h"

/**
 * Whether or not the BSP LED system has been initialized already this runtime.
 */
static uint8_t RTOSK_BSP_F756ZG_LED_INIT = 0U;

void rtosk_bsp_f756zg_on_board_led_init(void) {
    if(RTOSK_BSP_F756ZG_LED_INIT) return;
    /// enable GPIOB peripheral clock and wait a cycle
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    (void)RCC->AHB1ENR;
    for(uint32_t i = LED_0; i <= LED_2; i++) {
        /// set as GPIO
        GPIOB->MODER   &= ~(0x3U << ((i * 7UL) * 2U));
        GPIOB->MODER   |=  (0x1U << ((i * 7UL) * 2U));
        /// set as OUTPUT
        GPIOB->OTYPER  &= ~(1U << (i * 7UL));
        /// low-speed is fine for this project
        GPIOB->OSPEEDR &= ~(0x3U << ((i * 7UL) * 2U));
        /// no pull up or pull down resistors
        GPIOB->PUPDR   &= ~(0x3U << ((i * 7UL) * 2U));
    }
    RTOSK_BSP_F756ZG_LED_INIT = 1U;
}

void rtosk_bsp_f756zg_set_on_board_led(const bsp_led_t led, const uint8_t state) {
    if(led < LED_0 || led > LED_2) return;
    if(state == 1U) {
        GPIOB->ODR |= (1U << (led * 7UL));
    } else if(state == 0U) {
        GPIOB->ODR &= ~(1U << (led * 7UL));
    } else return;
}

void rtosk_bsp_f756zg_toggle_on_board_led(const bsp_led_t led) {
    if(led < LED_0 || led > LED_2) return;
    GPIOB->ODR ^= (1U << (led * 7UL));
}
