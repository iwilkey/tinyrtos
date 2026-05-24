/**
  ******************************************************************************
  * @file    primitives.h
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

#ifndef _FREERTOS_GUI_PRIMITIVES_H_
#define _FREERTOS_GUI_PRIMITIVES_H_

#include <stdint.h>

#define RGB332(r,g,b) (uint8_t)((((r) & 0x07U) << 5) | (((g) & 0x07U) << 2) | ((b) & 0x03U))

typedef void (*gui_write_fn_t)(const uint8_t * data, uint16_t len);

void gui_primitives_init(gui_write_fn_t write_fn);

void gui_clear(uint8_t color);

void gui_draw_pixel(uint8_t x, uint8_t y, uint8_t color);

void gui_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);

void gui_present(void);

#endif /// _FREERTOS_GUI_PRIMITIVES_H_
