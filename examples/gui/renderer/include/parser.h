/**
  ******************************************************************************
  * @file    parser.h
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

#ifndef _TINYRTOS_GUI_PARSER_H_
#define _TINYRTOS_GUI_PARSER_H_

#include <renderer.h>
#include <stdint.h>

typedef enum {
    RX_SOF0,
    RX_SOF1,
    RX_CMD,
    RX_LEN0,
    RX_LEN1,
    RX_PAYLOAD,
    RX_CHECKSUM
} rx_state_t;

void gui_protocol_set_renderer(gui_renderer_t *gui);

void gui_protocol_feed(uint8_t byte);

#endif /// _TINYRTOS_GUI_PARSER_H_
