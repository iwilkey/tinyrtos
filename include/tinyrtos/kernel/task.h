/**
  ******************************************************************************
  * @file    task.h
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

#ifndef _RTOSK_TASK_H_
#define _RTOSK_TASK_H_

#include <stdint.h>
#include <tinyrtos/kernel/kernel.h>

/**
 * The depth, in words, of a task's stack.
 */
#define RTOSK_TASK_STACK_WORDS 256UL

/**
 * Max tasks this RTOS supports.
 */
#define RTOSK_MAX_TASKS 4UL

/**
 * Index for idle task.
 */
#define RTOSK_IDLE_TASK_INDEX RTOSK_MAX_TASKS

/**
 * Total task slots (including idle task)
 */
#define RTOSK_TOTAL_TASK_SLOTS (RTOSK_MAX_TASKS + 1UL)

/**
 * A known stack value used when deciphering how much stack has been allocated over all-time. I'm using
 * the famous DEADBEEF for value dumps later.
 */
#define RTOSK_STACK_WATERMARK 0xDEADBEEFUL

/**
 * The maximum length of a TinyRTOS task name.
 */
#define RTOSK_TASK_NAME_MAX   16UL

/**
 * Current state of a RTOSK task.
 * @author Ian Wilkey
 */
typedef enum {
    RTOSK_TASK_READY = 0U,
    RTOSK_TASK_BLOCKED,
    RTOSK_TASK_BLOCKED_ON_SEMAPHORE,
    RTOSK_TASK_BLOCKED_ON_QUEUE,
    RTOSK_TASK_BLOCKED_ON_MUTEX,
    RTOSK_TASK_IDLE
} rtosk_task_state_t;

/**
 * RTOSK task information.
 * @author Ian Wilkey
 */
struct rtosk_task_info {

    /**
     * the index of this task inside of the TCB array.
     */
    uint32_t index;

    /**
     * the configured static priority of the task.
     */
    uint32_t priority;

    /**
     * the current state of the task to the scheduler.
     */
    rtosk_task_state_t state;

    /**
     * the wake tick of the task, if blocked.
     */
    uint32_t wake_tick;

    /**
     * the highest amount of stack used by the task during the runtime.
     */
    uint32_t stack_used_words;

    /**
     * the remaining stack of the task, relative to the highest stack usage of the task during the
     * runtime.
     */
    uint32_t stack_free_words;

    /**
     * the name of the task.
     */
    const char * name;

};

/**
 * RTOSK task control block.
 * @author Ian WIlkey
 */
typedef struct {

    /**
     * task stack pointer to be loaded into PSP when it's the tasks turn to run.
     */
    uint32_t * sp;

    /**
     * Stack buffer for this task. Raw memory reserved for task
     * stack.
     */
    uint32_t stack[RTOSK_TASK_STACK_WORDS];

    /**
     * Current state of the task.
     */
    rtosk_task_state_t state;

    /**
     * What tick the task should wake and continue execution.
     */
    uint32_t wake_tick;

    /**
     * The priority of this task. Higher number, higher priority.
     */
    uint32_t priority;

    /**
     * Name of the task.
     */
    const char * name;

} rtosk_task_t;

/**
 * Builds an initial Cortex-M exception stack frame manually and loads
 * the function address to the PC.
 * @author Ian Wilkey
 */
void rtosk_task_create(rtosk_task_func_t task_func, uint32_t priority, const char * name);

/**
 * Initializes the IDLE task, used for when no executing task is ready to wake, or there are no user defined tasks.
 * @author Ian Wilkey
 */
void rtosk_task_init_idle(void);

/**
 * Saves the current task's stack when it's time to switch context so it can be reloaded
 * later.
 * @author Ian Wilkey
 */
void rtosk_task_save_stack_pointer(uint32_t * sp);

/**
 * Marks the currently running task as blocked until the given absolute wake tick.
 * 
 * The scheduler should not select this task again until it's changed back to ready in the kernel tick handler (update blocked routine.)
 * @author Ian Wilkey
 */
void rtosk_task_block_current_until(uint32_t wake_tick);

/**
 * Blocks the current task while it waits for a semaphore.
 * @author Ian Wilkey
 */
void rtosk_task_block_current_on_semaphore(void);

/**
 * Blocks the current task while it waits for queue data.
 * @author Ian Wilkey
 */
void rtosk_task_block_current_on_queue(void);

/**
 * Blocks the current task task while it waits for a mutex.
 * @author Ian Wilkey
 */
void rtosk_task_block_current_on_mutex(void);

/**
 * Marks the task at the given index as ready to run.
 * @author Ian Wilkey
 */
void rtosk_task_set_ready(uint32_t index);

/**
 * CALLED BY SYSTICK! Scans all of the blocked tasks and wakes any whose wake_tick has expired, at which point the task's state is moved back to
 * ready and the scheduler can choose it again.
 * 
 * Returns 1 if at least one task was woken, 0 otherwise so the scheduler knows if it should schedule the IDLE task.
 * @author Ian Wilkey
 */
uint32_t rtosk_task_update_blocked(uint32_t current_tick);

/**
 * Returns the location of the running task's stack pointer.
 * @author Ian Wilkey
 */
uint32_t * rtosk_task_get_stack_pointer(void);

/**
 * Returns the number of currently configured RTOSK tasks.
 * @author Ian Wilkey
 */
uint32_t rtosk_task_get_count(void);

/**
 * Returns the current task index being executed.
 * @author Ian Wilkey
 */
uint32_t rtosk_task_get_current_index(void);

/**
 * Sets the index of the next/current task.
 * @author Ian Wilkey
 */
void rtosk_task_set_current_index(uint32_t index);

/**
 * Returns the state of a given RTOSK task index.
 * @author Ian Wilkey
 */
rtosk_task_t * rtosk_task_get(uint32_t index);

/**
 * Returns 1 if the IDLE task is ready.
 * @author Ian Wilkey
 */
uint32_t rtosk_task_is_idle_ready(void);

/**
 * Retrieves diagnostic information for the task at the given index.
 * @author Ian Wilkey
 */
uint32_t rtosk_task_get_info(uint32_t index, rtosk_task_info_t * info);

/**
 * Converts a task state enum into a readable string.
 * @author Ian Wilkey
 */
const char * rtosk_task_state_to_string(rtosk_task_state_t state);

#endif /// _RTOSK_TASK_H_
