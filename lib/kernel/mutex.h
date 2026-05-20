/**
  ******************************************************************************
  * @file    mutex.h
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

#ifndef _RTOSK_MUTEX_H_
#define _RTOSK_MUTEX_H_

#include <stdint.h>

/**
 * the value indicating that no task currently owns the mutex.
 */
#define RTOSK_MUTEX_NO_OWNER  0xFFFFFFFFUL

/**
 * value indicating that no task is currently blocked waiting for the mutex.
 */
#define RTOSK_MUTEX_NO_WAITER 0xFFFFFFFFUL

/**
 * Simple mutex used to protect exclusive access to a shared resource.
 * @author Ian Wilkey
 */
typedef struct {
    uint32_t owner;
    uint32_t waiting_task;
} rtosk_mutex_t;

/**
 * Initializes a mutex with no owner and no waiting task.
 * @author Ian Wilkey
 */
void rtosk_mutex_init(rtosk_mutex_t * mutex);

/**
 * Aquires the mutex, blocking the current task if another task owns it.
 * @author Ian Wilkey
 */
void rtosk_mutex_lock(rtosk_mutex_t * mutex);

/**
 * Releases the mutex and wakes a waiting task, if one exists.
 * @author Ian Wilkey
 */
void rtosk_mutex_unlock(rtosk_mutex_t * mutex);

#endif
