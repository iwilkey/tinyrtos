/**
  ******************************************************************************
  * @file    kernel.h
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

#ifndef _RTOSK_KERNEL_H_
#define _RTOSK_KERNEL_H_

#include <stdint.h>

/**
 * Current TinyRTOS version.
 */
#define RTOSK_KERNEL_VERSION "2026.5.20+1"

/**
 * Function handle for a RTOSK task.
 * @author Ian Wilkey
 */
typedef void (*rtosk_task_func_t)(void);

/**
 * Forward declaration of struct in task.h
 * @author Ian Wilkey
 */
typedef struct rtosk_task_info rtosk_task_info_t;

/**
 * Initializes the SYSTICK handler to properly sync the CORTEX-M's frequency to a deterministic
 * tick rate.
 * @author Ian Wilkey
 */
void rtosk_kernel_systick_init(void);

/**
 * Uses SYSTICK calibration to delay instruction for, as close to, given ms as possible. This should only be used for crude testing, as it
 * busy-waits and wastes CPU time.
 * @author Ian Wilkey
 */
void rtosk_kernel_delay_ms(uint32_t ms);

/**
 * Blocks the currently running task for the requested number of milliseconds.
 * 
 * The task is marked blocked until the global kernel tick reaches it's wake time. After blocking, this function immediatley yields so
 * another ready task can run.
 * @author Ian Wilkey
 */
void rtosk_kernel_sleep_ms(uint32_t ms);

/**
 * Returns the current system tick count.
 * @author Ian Wilkey
 */
uint32_t rtosk_kernel_get_ticks(void);

/**
 * Returns the system core clock frequency, unscaled.
 * @author Ian Wilkey
 */
uint32_t rtosk_get_cpu_freq(void);

/**
 * Builds an initial Cortex-M exception stack frame manually and loads
 * the function address to the PC.
 * @author Ian Wilkey
 */
void rtosk_kernel_create_task(rtosk_task_func_t task_func, uint32_t priority, const char * name);

/**
 * TODO: Add docs
 */
uint32_t rtosk_kernel_get_task_info(uint32_t index, rtosk_task_info_t * info);

/**
 * Called by a task to voluntarily give up it's current execution to allow other tasks to fire.
 * @author Ian Wilkey
 */
void rtosk_kernel_yield(void);

/**
 * Enters kernel into a CRITICAL section.
 * @author Ian Wilkey
 */
void rtosk_kernel_enter_critical(void);

/**
 * Exits kernel from a CRITICAL section.
 * @author Ian Wilkey
 */
void rtosk_kernel_exit_critical(void);

/**
 * Initializes the kernel to tell the system to use the process stack pointer
 * for application/thread execution and the main stack pointer for interrupts.
 * @author Ian Wilkey
 */
void rtosk_kernel_start(void);

#endif /// _RTOSK_KERNEL_H_
