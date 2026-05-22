/**
  ******************************************************************************
  * @file    mutex.c
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

#include <tinyrtos/kernel/mutex.h>
#include <tinyrtos/kernel/kernel.h>
#include <tinyrtos/kernel/task.h>

void rtosk_mutex_init(rtosk_mutex_t * mutex) {
    if(mutex == 0) {
        return;
    }
    mutex->owner = RTOSK_MUTEX_NO_OWNER;
    mutex->waiting_task = RTOSK_MUTEX_NO_WAITER;
}

void rtosk_mutex_lock(rtosk_mutex_t * mutex) {
    if(mutex == 0) {
        return;
    }
    for(;;) {
        rtosk_kernel_enter_critical();
        uint32_t current = rtosk_task_get_current_index();
        if(mutex->owner == RTOSK_MUTEX_NO_OWNER) {
            mutex->owner = current;
            rtosk_kernel_exit_critical();
            return;
        }
        if(mutex->owner == current) {
            rtosk_kernel_exit_critical();
            return;
        }
        mutex->waiting_task = current;
        rtosk_task_block_current_on_mutex();
        rtosk_kernel_exit_critical();
        rtosk_kernel_yield();
    }
}

void rtosk_mutex_unlock(rtosk_mutex_t * mutex) {
    if(mutex == 0) {
        return;
    }
    rtosk_kernel_enter_critical();
    uint32_t current = rtosk_task_get_current_index();
    if(mutex->owner != current) {
        rtosk_kernel_exit_critical();
        return;
    }
    mutex->owner = RTOSK_MUTEX_NO_OWNER;
    if(mutex->waiting_task != RTOSK_MUTEX_NO_WAITER) {
        rtosk_task_set_ready(mutex->waiting_task);
        mutex->waiting_task = RTOSK_MUTEX_NO_WAITER;
    }
    rtosk_kernel_exit_critical();
    rtosk_kernel_yield();
}
