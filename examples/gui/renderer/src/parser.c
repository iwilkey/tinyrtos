/**
  ******************************************************************************
  * @file    parser.c
  * @author  Ian Wilkey
  * @brief   A reciever application that's capable of rendering a framebuffer
  *          from a Nucleo-F756ZG board running TinyRTOS "gui" example.
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

#include <parser.h>
#include "../../common.h"

#define GUI_MAX_PAYLOAD 32U

static gui_renderer_t * GUI_RENDERER = 0;

static rx_state_t state = RX_SOF0;
static uint8_t cmd = 0;
static uint8_t len = 0;
static uint8_t index = 0;
static uint8_t checksum = 0;
static uint8_t payload[GUI_MAX_PAYLOAD];

void gui_protocol_set_renderer(gui_renderer_t *gui) {
    GUI_RENDERER = gui;
}

static void exec_cmd(void) {
    if(GUI_RENDERER == 0) return;
    switch(cmd) {
        case TINYRTOS_GUI_CMD_CLEAR:
            if(len == 1) gui_renderer_clear_color(GUI_RENDERER, payload[0]);
            break;
        case TINYRTOS_GUI_CMD_PIXEL:
            if(len == 3) gui_renderer_draw_pixel(GUI_RENDERER, payload[0], payload[1], payload[2]);
            break;
        case TINYRTOS_GUI_CMD_LINE:
            if(len == 5) gui_renderer_draw_line(GUI_RENDERER, payload[0], payload[1], payload[2], payload[3], payload[4]);
            break;
        case TINYRTOS_GUI_CMD_PRESENT:
            if(len == 0) gui_renderer_present(GUI_RENDERER);
            break;
    }
}

void gui_protocol_feed(uint8_t byte) {
    switch(state) {
        case RX_SOF0:
            if(byte == TINYRTOS_GUI_SOF0) state = RX_SOF1;
            break;
        case RX_SOF1:
            state = (byte == TINYRTOS_GUI_SOF1) ? RX_CMD : RX_SOF0;
            break;
        case RX_CMD:
            cmd = byte;
            checksum = byte;
            state = RX_LEN0;
            break;
        case RX_LEN0:
            len = byte;
            checksum ^= byte;
            if(len > GUI_MAX_PAYLOAD) {
                state = RX_SOF0;
                break;
            }
            index = 0;
            state = (len == 0) ? RX_CHECKSUM : RX_PAYLOAD;
            break;
        case RX_LEN1:
            state = RX_SOF0;
            break;
        case RX_PAYLOAD:
            payload[index++] = byte;
            checksum ^= byte;
            if(index >= len) {
                state = RX_CHECKSUM;
            }
            break;
        case RX_CHECKSUM:
            if(byte == checksum) exec_cmd();
            state = RX_SOF0;
            break;
        default:
            state = RX_SOF0;
            break;
    }
}
