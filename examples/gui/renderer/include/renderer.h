/**
  ******************************************************************************
  * @file    renderer.h
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

#ifndef _TINYRTOS_GUI_RENDERER_H_
#define _TINYRTOS_GUI_RENDERER_H_

#include <stdint.h>

typedef struct {
    void * window;
    void * renderer;
    uint32_t width;
    uint32_t height;
    uint32_t scale;
} gui_renderer_t;

uint32_t gui_renderer_init(
    gui_renderer_t * gui, 
    const char * title, 
    uint32_t width, 
    uint32_t height, 
    uint32_t scale
);

void gui_renderer_clear(gui_renderer_t * gui);

void gui_renderer_present(gui_renderer_t * gui);

void gui_renderer_draw_pixel(gui_renderer_t * gui, uint32_t x, uint32_t y, uint8_t on);

void gui_renderer_clear_color(gui_renderer_t *gui, uint8_t color);

void gui_renderer_draw_line(gui_renderer_t *gui, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint8_t color);

void gui_renderer_destroy(gui_renderer_t * gui);

uint32_t gui_renderer_poll_quit(void);

#endif /// _TINYRTOS_GUI_RENDERER_H_
