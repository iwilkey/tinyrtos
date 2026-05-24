/**
  ******************************************************************************
  * @file    primitives.c
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

#include <primitives.h>
#include "../common.h"

static gui_write_fn_t gui_write = 0;

static uint8_t checksum(const uint8_t * buf, uint16_t len) {
    uint8_t c = 0;
    for(uint16_t i = 0; i < len; i++) {
        c ^= buf[i];
    }
    return c;
}

static void send_cmd(uint8_t cmd, const uint8_t * payload, uint8_t len) {
    uint8_t packet[32];
    uint8_t n = 0;
    packet[n++] = TINYRTOS_GUI_SOF0;
    packet[n++] = TINYRTOS_GUI_SOF1;
    packet[n++] = cmd;
    packet[n++] = len;
    for(uint8_t i = 0; i < len; i++) {
        packet[n++] = payload[i];
    }
    packet[n++] = checksum(&packet[2], (uint16_t)(2U + len));
    if(gui_write) {
        gui_write(packet, n);
    }
}

void gui_primitives_init(gui_write_fn_t write_fn) {
    gui_write = write_fn;
}

void gui_clear(uint8_t color) {
    uint8_t payload[1] = {color};
    send_cmd(TINYRTOS_GUI_CMD_CLEAR, payload, 1);
}

void gui_draw_pixel(uint8_t x, uint8_t y, uint8_t color) {
    uint8_t payload[3] = {x, y, color};
    send_cmd(TINYRTOS_GUI_CMD_PIXEL, payload, 3);
}

void gui_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color) {
    uint8_t payload[5] = {x0, y0, x1, y1, color};
    send_cmd(TINYRTOS_GUI_CMD_LINE, payload, 5);
}

void gui_present(void) {
    send_cmd(TINYRTOS_GUI_CMD_PRESENT, 0, 0);
}
