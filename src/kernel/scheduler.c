/**
  ******************************************************************************
  * @file    scheduler.c
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

#include <tinyrtos/kernel/scheduler.h>
#include <tinyrtos/kernel/task.h>

void rtosk_scheduler_select_next(void) {
    uint32_t task_count = rtosk_task_get_count();
    if(task_count == 0UL) {
        return;
    }
    uint32_t current = rtosk_task_get_current_index();
    uint32_t best_index = RTOSK_IDLE_TASK_INDEX;
    uint32_t best_priority = 0UL;
    uint32_t found_ready = 0UL;
    for(uint32_t offset = 1UL; offset <= task_count; offset++) {
        uint32_t index = current + offset;
        if(index >= task_count) {
            index -= task_count;
        }
        rtosk_task_t *task = rtosk_task_get(index);
        if(task == 0) {
            continue;
        }
        if(task->state != RTOSK_TASK_READY) {
            continue;
        }
        if(found_ready == 0UL || task->priority > best_priority) {
            best_index = index;
            best_priority = task->priority;
            found_ready = 1UL;
        }
    }
    if(found_ready != 0UL) {
        rtosk_task_set_current_index(best_index);
        return;
    }
    if(rtosk_task_is_idle_ready() != 0UL) {
        rtosk_task_set_current_index(RTOSK_IDLE_TASK_INDEX);
    }
}
