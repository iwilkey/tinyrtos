/**
  ******************************************************************************
  * @file    kernel.c
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

#include "kernel.h"
#include "task.h"
#include "port.h"
#include "stm32f7xx.h"

/**
 * Current systicks since boot.
 */
static volatile uint32_t RTOSK_SYSTICKS = 0UL;

/**
 * Whether or not the RTOSK kernel SysTick has been configured this runtime.
 */
static uint8_t RTOSK_KERNEL_SYSTICK_INIT = 0U;

/**
 * Whether or not the RTOSK kernel has been initialized this runtime.
 */
static uint8_t RTOSK_KERNEL_INIT = 0U;

/**
 * Tracks nested critical sections so interrupts are only restored after the outermost
 * critical section exits.
 */
static uint32_t RTOSK_CRITICAL_NESTING = 0UL;

/**
 * Stores the interrupt state that existed before entered the outermost critical section so
 * it can be restored correctly.
 */
static uint32_t RTOSK_CRITICAL_PRIMASK = 0UL;

void rtosk_kernel_systick_init(void) {
    if(RTOSK_KERNEL_SYSTICK_INIT) return;
    rtosk_port_configure_exceptions();
    SysTick->LOAD = (SystemCoreClock / 1000UL) - 1UL;
    SysTick->VAL = 0UL;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk   |
                    SysTick_CTRL_ENABLE_Msk;
    RTOSK_KERNEL_SYSTICK_INIT = 1U;
}

void rtosk_kernel_start(void) {
    if(RTOSK_KERNEL_INIT) return;
    if(rtosk_task_get_count() == 0UL) {
        while(1) {}
    }
    rtosk_task_init_idle();
    RTOSK_KERNEL_INIT = 1U;
    rtosk_port_start_first_task();
}

void rtosk_kernel_delay_ms(uint32_t ms) {
    uint32_t start = RTOSK_SYSTICKS;
    while((RTOSK_SYSTICKS - start) < ms); /// busy-wait for now.
}

void rtosk_kernel_sleep_ms(uint32_t ms) {
    /// blocking current task modifies task state and wake time...
    rtosk_kernel_enter_critical();
    uint32_t wake_tick = RTOSK_SYSTICKS + ms;
    rtosk_task_block_current_until(wake_tick);
    rtosk_kernel_exit_critical();
    rtosk_kernel_yield();
}

uint32_t rtosk_kernel_get_ticks(void) {
    return RTOSK_SYSTICKS;
}

uint32_t rtosk_get_cpu_freq(void) {
    return SystemCoreClock;
}

void rtosk_kernel_create_task(rtosk_task_func_t task_func, uint32_t priority, const char * name) {
    rtosk_task_create(task_func, priority, name);
}

uint32_t rtosk_kernel_get_task_info(uint32_t index, rtosk_task_info_t * info) {
    return rtosk_task_get_info(index, info);
}

void rtosk_kernel_yield(void) {
    rtosk_port_yield();
}

void rtosk_kernel_enter_critical(void) {
    uint32_t primask = rtosk_port_irq_save();
    if(RTOSK_CRITICAL_NESTING == 0UL) {
        RTOSK_CRITICAL_PRIMASK = primask;
    }
    RTOSK_CRITICAL_NESTING++;
}

void rtosk_kernel_exit_critical(void) {
    if(RTOSK_CRITICAL_NESTING == 0UL) {
        return;
    }
    RTOSK_CRITICAL_NESTING--;
    if(RTOSK_CRITICAL_NESTING == 0UL) {
        rtosk_port_irq_restore(RTOSK_CRITICAL_PRIMASK);
    }
}

void rtosk_kernel_tick(void) {
    RTOSK_SYSTICKS++;
    rtosk_task_update_blocked(RTOSK_SYSTICKS);
    rtosk_port_yield();
}
