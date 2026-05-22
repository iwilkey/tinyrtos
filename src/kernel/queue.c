/**
  ******************************************************************************
  * @file    queue.c
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

#include <tinyrtos/kernel/queue.h>
#include <tinyrtos/kernel/kernel.h>
#include <tinyrtos/kernel/task.h>
#include <tinyrtos/kernel/port.h>

static void rtosk_queue_copy(uint8_t * dst, const uint8_t * src, uint32_t size) {
    for(uint32_t i = 0UL; i < size; i++) {
        dst[i] = src[i];
    }
}

void rtosk_queue_init(rtosk_queue_t * queue, void * buffer, uint32_t item_size, uint32_t capacity) {
    if(queue == 0 || buffer == 0 || item_size == 0UL || capacity == 0UL) {
        return;
    }
    queue->buffer = (uint8_t *)buffer;
    queue->item_size = item_size;
    queue->capacity = capacity;
    queue->head = 0UL;
    queue->tail = 0UL;
    queue->count = 0UL;
    queue->waiting_task = RTOSK_QUEUE_NO_WAITER;
}

uint32_t rtosk_queue_send(rtosk_queue_t * queue, const void * item) {
    if(queue == 0 || item == 0) {
        return 0UL;
    }
    rtosk_kernel_enter_critical();
    if(queue->count >= queue->capacity) {
        rtosk_kernel_exit_critical();
        return 0UL;
    }
    uint8_t * dst = &queue->buffer[queue->head * queue->item_size];
    rtosk_queue_copy(dst, (const uint8_t *)item, queue->item_size);
    queue->head++;
    if(queue->head >= queue->capacity) {
        queue->head = 0UL;
    }
    queue->count++;
    if(queue->waiting_task != RTOSK_QUEUE_NO_WAITER) {
        rtosk_task_set_ready(queue->waiting_task);
        queue->waiting_task = RTOSK_QUEUE_NO_WAITER;
    }
    rtosk_kernel_exit_critical();
    rtosk_kernel_yield();
    return 1UL;
}

uint32_t rtosk_queue_send_from_isr(rtosk_queue_t * queue, const void * item) {
    if(queue == 0 || item == 0) {
        return 0UL;
    }
    if(queue->count >= queue->capacity) {
        return 0UL;
    }
    uint8_t * dst = &queue->buffer[queue->head * queue->item_size];
    rtosk_queue_copy(dst, (const uint8_t *)item, queue->item_size);
    queue->head++;
    if(queue->head >= queue->capacity) {
        queue->head = 0UL;
    }
    queue->count++;
    if(queue->waiting_task != RTOSK_QUEUE_NO_WAITER) {
        rtosk_task_set_ready(queue->waiting_task);
        queue->waiting_task = RTOSK_QUEUE_NO_WAITER;
        rtosk_port_yield();
    }
    return 1UL;
}

uint32_t rtosk_queue_receive(rtosk_queue_t * queue, void * item) {
    if(queue == 0 || item == 0) {
        return 0UL;
    }
    for(;;) {
        rtosk_kernel_enter_critical();
        if(queue->count > 0UL) {
            uint8_t * src = &queue->buffer[queue->tail * queue->item_size];
            rtosk_queue_copy((uint8_t *)item, src, queue->item_size);
            queue->tail++;
            if(queue->tail >= queue->capacity) {
                queue->tail = 0UL;
            }
            queue->count--;
            rtosk_kernel_exit_critical();
            return 1UL;
        }
        queue->waiting_task = rtosk_task_get_current_index();
        rtosk_task_block_current_on_queue();
        rtosk_kernel_exit_critical();
        rtosk_kernel_yield();
    }
}
