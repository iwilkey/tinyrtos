/**
  ******************************************************************************
  * @file    semaphore.h
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

#ifndef _RTOSK_SEMAPHORE_H_
#define _RTOSK_SEMAPHORE_H_

#include <stdint.h>

/**
 * Value indicating that no task is currently blocked waiting for
 * the semaphore.
 */
#define RTOSK_SEMAPHORE_NO_WAITER 0xFFFFFFFFUL

/**
 * Counting semaphore used to signal events between tasks or from ISR to task.
 * @author Ian Wilkey
 */
typedef struct {
    uint32_t count;
    uint32_t waiting_task;
} rtosk_semaphore_t;

/**
 * Initializes a semaphore with the given starting count.
 * @author Ian Wilkey
 */
void rtosk_semaphore_init(rtosk_semaphore_t * sem, uint32_t initial_count);

/**
 * Takes the semaphore, blocking if count is zero.
 * @author Ian Wilkey
 */
void rtosk_semaphore_take(rtosk_semaphore_t * sem);

/**
 * Gives the semaphore from task context and wakes a waiting task if needed.
 * @author Ian Wilkey
 */
void rtosk_semaphore_give(rtosk_semaphore_t * sem);

/**
 * Gives the semaphore from interrupt context and wakes a waiting task if needed.
 * @author Ian Wilkey
 */
void rtosk_semaphore_give_from_isr(rtosk_semaphore_t * sem);

#endif
