/**
  ******************************************************************************
  * @file    serial.h
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

#ifndef TINYRTOS_GUI_SERIAL_H
#define TINYRTOS_GUI_SERIAL_H

#include <stdint.h>

typedef struct {
    void * port;
    uint32_t baud;
    char port_name[256];
} gui_serial_t;

uint32_t gui_serial_open_auto(gui_serial_t * serial, uint32_t baud);

int32_t gui_serial_read(gui_serial_t * serial, uint8_t * buffer, uint32_t len);

int32_t gui_serial_write(gui_serial_t * serial, const uint8_t * buffer, uint32_t len);

void gui_serial_close(gui_serial_t * serial);

#endif
