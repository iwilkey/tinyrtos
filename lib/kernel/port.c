/**
  ******************************************************************************
  * @file    port.c
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

#include "port.h"
#include "task.h"
#include "scheduler.h"
#include "stm32f7xx.h"

uint32_t * rtosk_port_align_stack_pointer(uint32_t *sp) {
    return (uint32_t *)((uint32_t)sp & ~0x7UL);
}

void SysTick_Handler(void) {
    rtosk_kernel_tick();
}

__attribute__((naked)) void SVC_Handler(void) {
    __asm volatile (
        /* load and call the address of the C function that returns the current task's saved SP */
        "ldr r0, =rtosk_task_get_stack_pointer \n"
        "blx r0                                \n"
        /* restore the software-saved registers R4-R11 from the task stack */
        "ldmia r0!, {r4-r11}                   \n"
        /* after popping R4-R11, r0 now points at the hardware exception frame; set PSP to it */
        "msr psp, r0                           \n"
        /* read the CONTROL register */
        "mrs r0, control                       \n"
        /* set CONTROL.SPSEL = 1, meaning Thread mode uses PSP instead of MSP */
        "orr r0, r0, #2                        \n"
        /* write the updated CONTROL value back */
        "msr control, r0                       \n"
        /* flush the instruction pipeline so the CONTROL change takes effect immediately */
        "isb                                   \n"
        /* load the Cortex-M exception-return code; return to Thread mode using PSP */
        "ldr lr, =0xFFFFFFFD                   \n"
        /* exception return; CPU restores R0-R3, R12, LR, PC, xPSR from PSP and starts task */
        "bx lr                                 \n"
    );
}

__attribute__((naked)) void PendSV_Handler(void) {
    __asm volatile (
        /* read the current Process Stack Pointer; this points to the hardware-saved frame */
        "mrs r0, psp                            \n"
        /* push R4-R11 onto the current task's stack... r0 is decremented and becomes saved SP */
        "stmdb r0!, {r4-r11}                    \n"
        /* save LR while calling C functions; push r4 too to preserve 8-byte stack alignment */
        "push {r4, lr}                          \n"
        /* load and call address of function that stores the current task's saved SP */
        "ldr r1, =rtosk_task_save_stack_pointer \n"
        "blx r1                                 \n"
        /* load and call address of scheduler function */
        "ldr r1, =rtosk_scheduler_select_next   \n"
        "blx r1                                 \n"
        /* load and call address of function that returns the selected task's saved SP */
        "ldr r1, =rtosk_task_get_stack_pointer  \n"
        "blx r1                                 \n"
        /* restore LR. r4 is only popped to balance the earlier push */
        "pop {r4, lr}                           \n"
        /* restore R4-R11 for the selected task. r0 advances to its hardware frame */
        "ldmia r0!, {r4-r11}                    \n"
        /* set PSP to the selected task's hardware exception frame */
        "msr psp, r0                            \n"
        /* exception return into selected task. CPU restores remaining registers from PSP */
        "bx lr                                  \n"
    );
}

void rtosk_port_yield(void) {
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    __DSB();
    __ISB();
}

uint32_t rtosk_port_irq_save(void) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    __DSB();
    __ISB();
    return primask;
}

void rtosk_port_irq_restore(uint32_t primask) {
    if((primask & 1UL) == 0UL) {
        __enable_irq();
    }
}

void rtosk_port_configure_exceptions(void) {
    /// PendSV should be the lowest-priority exception
    NVIC_SetPriority(PendSV_IRQn, 0xFFU);
    /// SysTick should be able to request a context switch but PendSV should run only after higher-priority interrupt work finishes
    NVIC_SetPriority(SysTick_IRQn, 0xFEU);
}

void rtosk_port_start_first_task(void) {
    __asm volatile ("svc #0");
}
