/**
  ******************************************************************************
  * @file    queue.h
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

#ifndef _RTOSK_QUEUE_H_
#define _RTOSK_QUEUE_H_

#include <stdint.h>

/**
 * value indicating that no task is currently blocked waiting
 */
#define RTOSK_QUEUE_NO_WAITER 0xFFFFFFFFUL

/**
 * Fixed size ring buffer for passing items between tasks or from ISR to task.
 * @author Ian Wilkey
 */
typedef struct {
    uint8_t * buffer;
    uint32_t item_size;
    uint32_t capacity;
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    uint32_t waiting_task;
} rtosk_queue_t;

/**
 * Initializes a queue using caller provided storage.
 * @author Ian Wilkey
 */
void rtosk_queue_init(rtosk_queue_t * queue, void * buffer, uint32_t item_size, uint32_t capacity);

/**
 * Sends one item to the queue from task context.
 * @author Ian Wilkey
 */
uint32_t rtosk_queue_send(rtosk_queue_t * queue, const void * item);

/**
 * Sends one item to queue from interrupt context.
 * @author Ian Wilkey
 */
uint32_t rtosk_queue_send_from_isr(rtosk_queue_t * queue, const void * item);

/**
 * Recieves one item from the queue, blocking until data is available.
 * @author Ian Wilkey
 */
uint32_t rtosk_queue_receive(rtosk_queue_t * queue, void * item);

#endif
