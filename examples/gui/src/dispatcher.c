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
#include <framebuffer.h>

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

static uint8_t checksum(const uint8_t * buf, uint16_t len) {
    uint8_t c = 0;
    for(uint16_t i = 0; i < len; i++) c ^= buf[i];
    return c;
}

static void send_frame(void) {
    uint8_t * fb = fb_get();
    uint16_t len = TINYRTOS_FRAMEBUFFER_SIZE;
    uint8_t cs = checksum(fb, len);
    rtosk_mutex_lock(&DISPATCHER_USART3_TX_MUTEX);
    rtosk_bsp_f756zg_usart3_write_char(TINYRTOS_GUI_SOF0);
    rtosk_bsp_f756zg_usart3_write_char(TINYRTOS_GUI_SOF1);
    rtosk_bsp_f756zg_usart3_write_char(0x01);
    rtosk_bsp_f756zg_usart3_write_char((uint8_t)(len & 0xFF));
    rtosk_bsp_f756zg_usart3_write_char((uint8_t)(len >> 8));
    for(uint16_t i = 0; i < len; i++) {
        rtosk_bsp_f756zg_usart3_write_char((char)fb[i]);
    }
    rtosk_bsp_f756zg_usart3_write_char(cs);
    rtosk_mutex_unlock(&DISPATCHER_USART3_TX_MUTEX);
}

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} vec3_t;

typedef struct {
    int16_t x;
    int16_t y;
} vec2_t;

static const vec3_t cube[8] = {
    {-16, -16, -16}, { 16, -16, -16},
    { 16,  16, -16}, {-16,  16, -16},
    {-16, -16,  16}, { 16, -16,  16},
    { 16,  16,  16}, {-16,  16,  16}
};

static const uint8_t edges[12][2] = {
    {0,1}, {1,2}, {2,3}, {3,0},
    {4,5}, {5,6}, {6,7}, {7,4},
    {0,4}, {1,5}, {2,6}, {3,7}
};

static const int8_t sin_lut[64] = {
      0,  12,  25,  37,  49,  60,  71,  81,
     90,  98, 106, 112, 118, 122, 125, 127,
    127, 127, 125, 122, 118, 112, 106,  98,
     90,  81,  71,  60,  49,  37,  25,  12,
      0, -12, -25, -37, -49, -60, -71, -81,
    -90, -98,-106,-112,-118,-122,-125,-127,
   -127,-127,-125,-122,-118,-112,-106, -98,
    -90, -81, -71, -60, -49, -37, -25, -12
};

static int16_t fsin(uint8_t a) {
    return sin_lut[a & 63];
}

static int16_t fcos(uint8_t a) {
    return sin_lut[(a + 16) & 63];
}

static void draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t v) {
    int16_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t dy = (y1 > y0) ? (y0 - y1) : (y1 - y0);
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx + dy;
    for(;;) {
        fb_set_pixel((uint16_t)x0, (uint16_t)y0, v);
        if(x0 == x1 && y0 == y1) break;
        int16_t e2 = err << 1;
        if(e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if(e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

static vec2_t project(vec3_t p, uint8_t ax, uint8_t ay) {
    int16_t sx = fsin(ax);
    int16_t cx = fcos(ax);
    int16_t sy = fsin(ay);
    int16_t cy = fcos(ay);
    int16_t y1 = (int16_t)(((int32_t)p.y * cx - (int32_t)p.z * sx) >> 7);
    int16_t z1 = (int16_t)(((int32_t)p.y * sx + (int32_t)p.z * cx) >> 7);
    int16_t x2 = (int16_t)(((int32_t)p.x * cy + (int32_t)z1 * sy) >> 7);
    int16_t z2 = (int16_t)((-(int32_t)p.x * sy + (int32_t)z1 * cy) >> 7);
    int16_t d = 96;
    int16_t s = (int16_t)((d * 64) / (z2 + d));
    vec2_t out;
    out.x = (TINYRTOS_FRAMEBUFFER_WIDTH / 2) + (int16_t)(((int32_t)x2 * s) >> 6);
    out.y = (TINYRTOS_FRAMEBUFFER_HEIGHT / 2) + (int16_t)(((int32_t)y1 * s) >> 6);
    return out;
}

static void gui_dispatcher_task(void) {
    uint8_t angle = 0;
    for(;;) {
        vec2_t projected[8];
        fb_clear();
        for(uint8_t i = 0; i < 8; i++) {
            projected[i] = project(cube[i], angle, angle + 11);
        }
        for(uint8_t i = 0; i < 12; i++) {
            uint8_t a = edges[i][0];
            uint8_t b = edges[i][1];
            draw_line(
                projected[a].x, projected[a].y,
                projected[b].x, projected[b].y,
                255U
            );
        }
        rtosk_bsp_f756zg_toggle_on_board_led(LED_0);
        send_frame();
        angle++;
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
    rtosk_kernel_create_task(gui_dispatcher_task, priority, "dispatcher");
    DISPATCHER_INIT = 1U;
}
