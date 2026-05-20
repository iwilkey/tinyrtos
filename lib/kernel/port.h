/**
  ******************************************************************************
  * @file    port.h
  * @author  Ian Wilkey
  * @brief   A compact, preemptive priority RTOS kernel for ARM Cortex-M, 
  *          written from scratch in C on STM32.
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

#ifndef _RTOSK_PORT_H_
#define _RTOSK_PORT_H_

#include <stdint.h>

/**
 * Thumb bit for stack frame.
 */
#define RTOSK_XPSR_T_BIT 0x01000000UL

/**
 * Code to return to Thread mode, use PSP, and restore registers
 * from the PSP stack frame.
 */
#define RTOSK_EXC_RETURN_PSP 0xFFFFFFFDUL

/**
 * aligns the given stack pointer to an 8-byte boundary which Cortex-M expects for
 * exception return.
 * @author Ian Wilkey
 */
uint32_t * rtosk_port_align_stack_pointer(uint32_t * sp);

/**
 * Triggers a software interrupt to call SVC handler to load in and use
 * the stack frame of the first task.
 * @author Ian Wilkey
 */
void rtosk_port_start_first_task(void);

/**
 * Calls PendSV to save the state of the current executing task and call the scheduler to figure out what
 * task needs to be executed next (if none, IDLE.)
 * @author Ian Wilkey
 */
void rtosk_port_yield(void);

/**
 * Saves the current interrupt enable state and disables interrupts.
 * @author Ian Wilkey
 */
uint32_t rtosk_port_irq_save(void);

/**
 * Restores interrupts to a previously saved state.
 * @author Ian Wilkey
 */
void rtosk_port_irq_restore(uint32_t primask);

/**
 * Sets kernel exception priorities for SysTick and PendSV.
 * @author Ian Wilkey
 */
void rtosk_port_configure_exceptions(void);

/**
 * Called every configured SysTick interrupt.
 * @author Ian Wilkey
 */
void rtosk_kernel_tick(void);

#endif /// _RTOSK_PORT_H_
