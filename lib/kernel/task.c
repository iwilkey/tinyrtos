/**
  ******************************************************************************
  * @file    task.c
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

#include "task.h"
#include "port.h"

/**
 * All RTOSK tasks, including idle task.
 */
static rtosk_task_t RTOSK_TASKS[RTOSK_TOTAL_TASK_SLOTS];

static uint32_t RTOSK_TASK_COUNT   = 0UL;
static uint32_t RTOSK_CURRENT_TASK = 0UL;
static uint32_t RTOSK_IDLE_READY   = 0UL;

/**
 * Idle task that runs only when no user task is ready.
 * @author Ian Wilkey
 */
static void rtosk_idle_task(void) {
    for(;;) {
        __asm volatile ("wfi");
    }
}

/**
 * guard to ensure that a task never returns, loaded into LR of the task stack frame.
 * @author Ian Wilkey
 */
static void rtosk_task_exit_trap(void) {
    for(;;) {}
}

/**
 * Fills a task stack with a known pattern (DEADBEEF!) so unused stack space can be
 * measured later.
 * @author Ian Wilkey
 */
static void rtosk_task_watermark_stack(rtosk_task_t * task) {
    for(uint32_t i = 0UL; i < RTOSK_TASK_STACK_WORDS; i++) {
        task->stack[i] = RTOSK_STACK_WATERMARK;
    }
}

/**
 * Counts how many stack works still contain DEADBEEF.
 * @author Ian Wilkey
 */
static uint32_t rtosk_task_get_stack_free_words(rtosk_task_t * task) {
    uint32_t free_words = 0UL;
    while(free_words < RTOSK_TASK_STACK_WORDS && task->stack[free_words] == RTOSK_STACK_WATERMARK) {
        free_words++;
    }
    return free_words;
}

/**
 * Manually builds a Cortex-M exception stack frame for given task.
 * @author Ian Wilkey
 */
static uint32_t * rtosk_task_build_stack(rtosk_task_t * task, rtosk_task_func_t task_func) {
    /// on ARM Cortex-M, stacks grow downward, so the pointer starts at the first
    /// address of the task stack.
    uint32_t * sp = &task->stack[RTOSK_TASK_STACK_WORDS];
    sp = rtosk_port_align_stack_pointer(sp);
    /*
    then, I build the first 8-words: the hardware exception frame
    SP -> R0
          R1
          R2
          R3
          R12
          LR
          PC   = task_func
          xPSR = Thumb bit set
    */
    *(--sp) = RTOSK_XPSR_T_BIT;
    *(--sp) = (uint32_t)task_func;
    *(--sp) = (uint32_t)rtosk_task_exit_trap; // LR
    *(--sp) = 0x00000000UL; // R12
    *(--sp) = 0x00000000UL; // R3
    *(--sp) = 0x00000000UL; // R2
    *(--sp) = 0x00000000UL; // R1
    *(--sp) = 0x00000000UL; // R0
    /// Next, build 8 more words, the software-saved frame.
    /// The final stack frame will look like this:
    /*
    task->sp -> R4
            R5
            R6
            R7
            R8
            R9
            R10
            R11
            R0
            R1
            R2
            R3
            R12
            LR
            PC
            xPSR
    */
    *(--sp) = 0x00000000UL;
    *(--sp) = 0x00000000UL;
    *(--sp) = 0x00000000UL;
    *(--sp) = 0x00000000UL;
    *(--sp) = 0x00000000UL;
    *(--sp) = 0x00000000UL;
    *(--sp) = 0x00000000UL;
    *(--sp) = 0x00000000UL;
    /// returns the first software-saved register slot.
    return sp;
}

void rtosk_task_init_idle(void) {
    if(RTOSK_IDLE_READY) return;
    rtosk_task_t * task = &RTOSK_TASKS[RTOSK_IDLE_TASK_INDEX];
    rtosk_task_watermark_stack(task);
    task->sp = rtosk_task_build_stack(task, rtosk_idle_task);
    task->state = RTOSK_TASK_IDLE;
    task->wake_tick = 0UL;
    task->priority = 0UL;
    task->name = "idle";
    RTOSK_IDLE_READY = 1UL;
}

void rtosk_task_create(rtosk_task_func_t task_func, uint32_t priority, const char * name) {
    if(task_func == 0) {
        return;
    }
    if(RTOSK_TASK_COUNT >= RTOSK_MAX_TASKS) {
        return;
    }
    rtosk_task_t * task = &RTOSK_TASKS[RTOSK_TASK_COUNT];
    rtosk_task_watermark_stack(task);
    task->sp = rtosk_task_build_stack(task, task_func);
    task->state = RTOSK_TASK_READY;
    task->wake_tick = 0UL;
    task->priority = priority;
    task->name = name;
    RTOSK_TASK_COUNT++;
}

void rtosk_task_save_stack_pointer(uint32_t * sp) {
    if(RTOSK_TASK_COUNT == 0UL) {
        return;
    }
    RTOSK_TASKS[RTOSK_CURRENT_TASK].sp = sp;
}

void rtosk_task_block_current_until(uint32_t wake_tick) {
    RTOSK_TASKS[RTOSK_CURRENT_TASK].state = RTOSK_TASK_BLOCKED;
    RTOSK_TASKS[RTOSK_CURRENT_TASK].wake_tick = wake_tick;
}

uint32_t rtosk_task_update_blocked(uint32_t current_tick) {
    uint32_t woke_task = 0UL;
    for(uint32_t i = 0UL; i < RTOSK_TASK_COUNT; i++) {
        if(RTOSK_TASKS[i].state == RTOSK_TASK_BLOCKED) {
            if((int32_t)(current_tick - RTOSK_TASKS[i].wake_tick) >= 0) {
                RTOSK_TASKS[i].state = RTOSK_TASK_READY;
                woke_task = 1UL;
            }
        }
    }
    return woke_task;
}

void rtosk_task_block_current_on_semaphore(void) {
    RTOSK_TASKS[RTOSK_CURRENT_TASK].state = RTOSK_TASK_BLOCKED_ON_SEMAPHORE;
}

void rtosk_task_block_current_on_queue(void) {
    RTOSK_TASKS[RTOSK_CURRENT_TASK].state = RTOSK_TASK_BLOCKED_ON_QUEUE;
}

void rtosk_task_block_current_on_mutex(void) {
    RTOSK_TASKS[RTOSK_CURRENT_TASK].state = RTOSK_TASK_BLOCKED_ON_MUTEX;
}

void rtosk_task_set_ready(uint32_t index) {
    if(index >= RTOSK_TASK_COUNT) {
        return;
    }
    RTOSK_TASKS[index].state = RTOSK_TASK_READY;
}

uint32_t * rtosk_task_get_stack_pointer(void) {
    if(RTOSK_CURRENT_TASK == RTOSK_IDLE_TASK_INDEX) {
        return RTOSK_TASKS[RTOSK_IDLE_TASK_INDEX].sp;
    }
    if(RTOSK_TASK_COUNT == 0UL) {
        return 0;
    }
    return RTOSK_TASKS[RTOSK_CURRENT_TASK].sp;
}

uint32_t rtosk_task_get_count(void) {
    return RTOSK_TASK_COUNT;
}

uint32_t rtosk_task_get_current_index(void) {
    return RTOSK_CURRENT_TASK;
}

void rtosk_task_set_current_index(uint32_t index) {
    if(index >= RTOSK_TOTAL_TASK_SLOTS) {
        return;
    }
    RTOSK_CURRENT_TASK = index;
}

rtosk_task_t * rtosk_task_get(uint32_t index) {
    if(index >= RTOSK_TOTAL_TASK_SLOTS) {
        return 0;
    }
    return &RTOSK_TASKS[index];
}

uint32_t rtosk_task_is_idle_ready(void) {
    return RTOSK_IDLE_READY;
}

uint32_t rtosk_task_get_info(uint32_t index, rtosk_task_info_t * info) {
    if(info == 0) {
        return 0UL;
    }
    if(index >= RTOSK_TOTAL_TASK_SLOTS) {
        return 0UL;
    }
    if(index >= RTOSK_TASK_COUNT && index != RTOSK_IDLE_TASK_INDEX) {
        return 0UL;
    }
    if(index == RTOSK_IDLE_TASK_INDEX && RTOSK_IDLE_READY == 0UL) {
        return 0UL;
    }
    rtosk_task_t * task = &RTOSK_TASKS[index];
    uint32_t free_words = rtosk_task_get_stack_free_words(task);
    info->index = index;
    info->priority = task->priority;
    info->state = task->state;
    info->wake_tick = task->wake_tick;
    info->stack_free_words = free_words;
    info->stack_used_words = RTOSK_TASK_STACK_WORDS - free_words;
    info->name = task->name;
    return 1UL;
}

const char * rtosk_task_state_to_string(rtosk_task_state_t state) {
    switch(state) {
        case RTOSK_TASK_READY:
            return "READY";
        case RTOSK_TASK_BLOCKED:
            return "SLEEP";
        case RTOSK_TASK_BLOCKED_ON_SEMAPHORE:
            return "SEM";
        case RTOSK_TASK_BLOCKED_ON_QUEUE:
            return "QUEUE";
        case RTOSK_TASK_BLOCKED_ON_MUTEX:
            return "MUTEX";
        case RTOSK_TASK_IDLE:
            return "IDLE";
        default:
            return "UNKNOWN";
    }
}

