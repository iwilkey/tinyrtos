/**
  ******************************************************************************
  * @file    semaphore.c
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

#include "semaphore.h"
#include "kernel.h"
#include "task.h"
#include "port.h"

void rtosk_semaphore_init(rtosk_semaphore_t * sem, uint32_t initial_count) {
    if(sem == 0) {
        return;
    }
    sem->count = initial_count;
    sem->waiting_task = RTOSK_SEMAPHORE_NO_WAITER;
}

void rtosk_semaphore_take(rtosk_semaphore_t * sem) {
    if(sem == 0) {
        return;
    }
    rtosk_kernel_enter_critical();
    if(sem->count > 0UL) {
        sem->count--;
        rtosk_kernel_exit_critical();
        return;
    }
    sem->waiting_task = rtosk_task_get_current_index();
    rtosk_task_block_current_on_semaphore();
    rtosk_kernel_exit_critical();
    rtosk_kernel_yield();
}

void rtosk_semaphore_give(rtosk_semaphore_t * sem) {
    if(sem == 0) {
        return;
    }
    rtosk_kernel_enter_critical();
    if(sem->waiting_task != RTOSK_SEMAPHORE_NO_WAITER) {
        rtosk_task_set_ready(sem->waiting_task);
        sem->waiting_task = RTOSK_SEMAPHORE_NO_WAITER;
    } else {
        sem->count = 1UL;
    }
    rtosk_kernel_exit_critical();
    rtosk_kernel_yield();
}

void rtosk_semaphore_give_from_isr(rtosk_semaphore_t * sem) {
    if(sem == 0) {
        return;
    }
    if(sem->waiting_task != RTOSK_SEMAPHORE_NO_WAITER) {
        rtosk_task_set_ready(sem->waiting_task);
        sem->waiting_task = RTOSK_SEMAPHORE_NO_WAITER;
        rtosk_port_yield();
    } else {
        sem->count = 1UL;
    }
}
