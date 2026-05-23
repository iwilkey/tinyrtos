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

static uint8_t TINYRTOS_FRAMEBUFFER[TINYRTOS_FRAMEBUFFER_SIZE];

static rx_state_t state = RX_SOF0;
static uint8_t    cmd = 0;
static uint16_t   len = 0;
static uint16_t   index = 0;
static uint8_t    checksum = 0;

int gui_protocol_feed(uint8_t byte) {
    switch(state) {
        case RX_SOF0:
            if(byte == TINYRTOS_GUI_SOF0) state = RX_SOF1;
            break;

        case RX_SOF1:
            state = (byte == TINYRTOS_GUI_SOF1) ? RX_CMD : RX_SOF0;
            break;

        case RX_CMD:
            cmd = byte;
            state = RX_LEN0;
            break;

        case RX_LEN0:
            len = byte;
            state = RX_LEN1;
            break;

        case RX_LEN1:
            len |= ((uint16_t)byte << 8);
            if(cmd != TINYRTOS_GUI_CMD_FRAME || len != TINYRTOS_FRAMEBUFFER_SIZE) {
                state = RX_SOF0;
                break;
            }
            index = 0;
            checksum = 0;
            state = RX_PAYLOAD;
            break;
        case RX_PAYLOAD:
            TINYRTOS_FRAMEBUFFER[index++] = byte;
            checksum ^= byte;
            if(index >= len) {
                state = RX_CHECKSUM;
            }
            break;
        case RX_CHECKSUM:
            state = RX_SOF0;
            return byte == checksum;
        default:
            state = RX_SOF0;
            break;
    }
    return 0;
}

uint8_t * get_fb(void) {
    return TINYRTOS_FRAMEBUFFER;
}
