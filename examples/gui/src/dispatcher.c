/**
  ******************************************************************************
  * @file    dispatcher.c
  * @author  Ian Wilkey
  * @brief   A framebuffer sent over serial to a reciever application that's capable of
  *          rendering the image in real-time.
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

#include <dispatcher.h>
#include <primitives.h>
#include <scene.h>

#include <tinyrtos/kernel/kernel.h>
#include <tinyrtos/kernel/mutex.h>
#include <tinyrtos/kernel/queue.h>

#include <tinyrtos/bsp/f756zg/led.h>
#include <tinyrtos/bsp/f756zg/usart3.h>

#include "../common.h"

static uint8_t       DISPATCHER_INIT = 0U;
static rtosk_queue_t DISPATCHER_USART3_RX_QUEUE;
static rtosk_mutex_t DISPATCHER_USART3_TX_MUTEX;
static uint8_t       DISPATCHER_USART3_RX_QUEUE_STORAGE[TINYRTOS_GUI_DISPATCHER_CMD_BUFFER_SIZE];

void rtosk_bsp_f756zg_usart3_rx_callback_from_isr(uint8_t c) {
    rtosk_queue_send_from_isr(&DISPATCHER_USART3_RX_QUEUE, &c);
}

static void dispatch_bytes(const uint8_t *data, uint16_t len) {
    rtosk_mutex_lock(&DISPATCHER_USART3_TX_MUTEX);
    for(uint16_t i = 0; i < len; i++) {
        rtosk_bsp_f756zg_usart3_write_char((char)data[i]);
    }
    rtosk_mutex_unlock(&DISPATCHER_USART3_TX_MUTEX);
}



static void gui_dispatcher_task(void) {
    for(;;) {
        rtosk_bsp_f756zg_toggle_on_board_led(LED_0);
        scene_render();
        rtosk_kernel_sleep_ms(16UL);
    }
}

void gui_dispatcher_init(uint32_t baud, uint32_t priority) {
    if(DISPATCHER_INIT) return;
    rtosk_bsp_f756zg_on_board_led_init();
    rtosk_bsp_f756zg_usart3_init(baud);
    rtosk_queue_init(
        &DISPATCHER_USART3_RX_QUEUE, 
        DISPATCHER_USART3_RX_QUEUE_STORAGE, 
        sizeof(uint8_t), 
        TINYRTOS_GUI_DISPATCHER_CMD_BUFFER_SIZE
    );
    rtosk_mutex_init(&DISPATCHER_USART3_TX_MUTEX);
    rtosk_bsp_f756zg_uart_enable_rx_interrupt();
    gui_primitives_init(dispatch_bytes);
    rtosk_kernel_create_task(gui_dispatcher_task, priority, "dispatcher");
    DISPATCHER_INIT = 1U;
}
