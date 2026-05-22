/**
  ******************************************************************************
  * @file    main.c
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

#include <stdint.h>
#include <stdio.h>
#include <serial.h>
#include <renderer.h>

#define FB_WIDTH  128UL
#define FB_HEIGHT 64UL
#define FB_SCALE  6UL

int main(void) {
    gui_serial_t serial;
    if(!gui_serial_open_auto(&serial, 115200UL)) {
        printf("failed to connect to target device.\n");
        return 1;
    }
    printf("target connected.\n");
    gui_renderer_t gui;
    if(!gui_renderer_init(&gui, "TinyRTOS GUI", FB_WIDTH, FB_HEIGHT, FB_SCALE)) {
        return 1;
    }
    uint32_t x = 0UL;
    while(!gui_renderer_poll_quit()) {
        uint8_t rx;
        int32_t n = gui_serial_read(&serial, &rx, 1U);
        if(n == 1) {
            x = (uint32_t)(rx % FB_WIDTH);
            gui_renderer_clear(&gui);
            for(uint32_t y = 0; y < FB_HEIGHT; y++) {
                gui_renderer_draw_pixel(&gui, x, y, 1U);
            }
            gui_renderer_present(&gui);
        }
    }
    gui_renderer_destroy(&gui);
    return 0;
}
