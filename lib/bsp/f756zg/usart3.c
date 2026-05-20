/**
  ******************************************************************************
  * @file    usart3.c
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

#include "usart3.h"

/**
 * Whether or not the BSP USART3 system has been initialized already this runtime.
 */
static uint8_t RTOSK_BSP_F756ZG_USART3_INIT = 0U;

void rtosk_bsp_f756zg_usart3_init(uint32_t baud) {
    if(RTOSK_BSP_F756ZG_USART3_INIT) return;
    /// enable GPIOD and USART3 peripheral clock and wait a couple cycles
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    (void)RCC->AHB1ENR;
    (void)RCC->APB1ENR;
    /// per data sheet USART3 initialization...
    BSP_PORT_D->MODER &= ~(0x3UL << (BSP_USART3_GPIO_TX * 2UL));
    BSP_PORT_D->MODER |=  (0x2UL << (BSP_USART3_GPIO_TX * 2UL));
    BSP_PORT_D->MODER &= ~(0x3UL << (BSP_USART3_GPIO_RX * 2UL));
    BSP_PORT_D->MODER |=  (0x2UL << (BSP_USART3_GPIO_RX * 2UL));
    BSP_PORT_D->AFR[1] &= ~(0xFUL << ((BSP_USART3_GPIO_TX - 8UL) * 4UL));
    BSP_PORT_D->AFR[1] |=  (0x7UL << ((BSP_USART3_GPIO_TX - 8UL) * 4UL));
    BSP_PORT_D->AFR[1] &= ~(0xFUL << ((BSP_USART3_GPIO_RX - 8UL) * 4UL));
    BSP_PORT_D->AFR[1] |=  (0x7UL << ((BSP_USART3_GPIO_RX - 8UL) * 4UL));
    BSP_USART3->CR1 = 0UL;
    BSP_USART3->CR2 = 0UL;
    BSP_USART3->CR3 = 0UL;
    BSP_USART3->BRR = SystemCoreClock / baud;
    BSP_USART3->CR1 |= USART_CR1_TE;
    BSP_USART3->CR1 |= USART_CR1_RE;
    BSP_USART3->CR1 |= USART_CR1_UE;
    RTOSK_BSP_F756ZG_USART3_INIT = 1U;
}

void rtosk_bsp_f756zg_uart_enable_rx_interrupt(void) {
    USART3->CR1 |= USART_CR1_RXNEIE;
    NVIC_EnableIRQ(USART3_IRQn);
}

void rtosk_bsp_f756zg_usart3_write_char(char c) {
    while((USART3->ISR & USART_ISR_TXE) == 0UL) {}
    USART3->TDR = (uint8_t)c;
}

void rtosk_bsp_f756zg_usart3_write_string(const char * s) {
    while(*s != '\0') {
        if(*s == '\n') {
            rtosk_bsp_f756zg_usart3_write_char('\r');
        }
        rtosk_bsp_f756zg_usart3_write_char(*s);
        s++;
    }
}

__attribute__((weak)) void rtosk_bsp_f756zg_usart3_rx_callback_from_isr(uint8_t c) {
    (void)c;
}

/**
 * ISR handler for USART3 RX data recieved.
 * @author Ian Wilkey
 */
void USART3_IRQHandler(void) {
    if((USART3->ISR & USART_ISR_RXNE) != 0UL) {
        uint8_t c = (uint8_t)USART3->RDR;
        rtosk_bsp_f756zg_usart3_rx_callback_from_isr(c);
    }
}
