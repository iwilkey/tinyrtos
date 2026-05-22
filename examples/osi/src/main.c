/**
  ******************************************************************************
  * @file    main.c
  * @author  Ian Wilkey
  * @brief   A simple use case of TinyRTOS running on a Nucleo-F756ZG board that
  *          blinks two on-board LEDs and serves OSI over serial when monitored.
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

#include <stdio.h>
#include <string.h>

#include <tinyrtos/kernel/kernel.h>
#include <tinyrtos/osi/osi.h>
#include <tinyrtos/bsp/f756zg/led.h>

/**
 * If non-zero, the system will try to deref a null pointer in TASK0 to show how TinyRTOS handles a
 * HardFault.
 */
#define SIMULATE_HARD_FAULT 0U

/**
 * Synced task to toggle the on-board GREEN LED every second.
 * @author Ian Wilkey
 */
static void led0_task(void) {
    while(1) {
        rtosk_bsp_f756zg_toggle_on_board_led(LED_0);
        rtosk_kernel_sleep_ms(1000UL);
        if(SIMULATE_HARD_FAULT) {
            *(volatile uint32_t *)0x00000000UL = 1234UL;
        }
    }
}

/**
 * Purposefuly non-yeilding task to prove preemption works. Toggles on-board BLUE LED.
 * @author Ian Wilkey
 */
static void led1_task(void) {
    while(1) {
        rtosk_bsp_f756zg_toggle_on_board_led(LED_1);
        /// never yields; if task0 still runs this prove preemption works as designed.
        for(volatile uint32_t i = 0UL; i < 3000000UL; i++) {}
    }
}

int main(void) {
    rtosk_bsp_f756zg_on_board_led_init();
    rtosk_osi_init(115200UL, 3UL);
    rtosk_kernel_systick_init();
    /// NOTE: higher-priority tasks that never yield will cause CPU starvation.
    rtosk_kernel_create_task(led0_task, 2UL, "led0");
    rtosk_kernel_create_task(led1_task, 1UL, "led1");
    rtosk_kernel_start();
    for(;;) {}
    return 0;
}
