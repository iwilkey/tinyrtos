/**
  ******************************************************************************
  * @file    renderer.c
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

#include <renderer.h>
#include <SDL2/SDL.h>

uint32_t gui_renderer_init(gui_renderer_t * gui, const char * title, uint32_t width, uint32_t height, uint32_t scale) {
    if(gui == 0) {
        return 0;
    }
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 0;
    }
    gui->width = width;
    gui->height = height;
    gui->scale = scale;
    gui->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        (int)(width * scale),
        (int)(height * scale),
        SDL_WINDOW_SHOWN
    );
    if(gui->window == 0) {
        SDL_Quit();
        return 0;
    }
    gui->renderer = SDL_CreateRenderer(
        (SDL_Window *)gui->window,
        -1,
        SDL_RENDERER_ACCELERATED
    );
    if(gui->renderer == 0) {
        SDL_DestroyWindow((SDL_Window *)gui->window);
        SDL_Quit();
        return 0;
    }
    return 1;
}

void gui_renderer_clear(gui_renderer_t * gui) {
    SDL_SetRenderDrawColor((SDL_Renderer *)gui->renderer, 0, 0, 0, 255);
    SDL_RenderClear((SDL_Renderer *)gui->renderer);
}

void gui_renderer_present(gui_renderer_t * gui) {
    SDL_RenderPresent((SDL_Renderer *)gui->renderer);
}

void gui_renderer_draw_pixel(gui_renderer_t * gui, uint32_t x, uint32_t y, uint8_t on) {
    SDL_Rect rect;
    rect.x = (int)(x * gui->scale);
    rect.y = (int)(y * gui->scale);
    rect.w = (int)gui->scale;
    rect.h = (int)gui->scale;
    if(on) {
        SDL_SetRenderDrawColor((SDL_Renderer *)gui->renderer, 255, 255, 255, 255);
    } else {
        SDL_SetRenderDrawColor((SDL_Renderer *)gui->renderer, 0, 0, 0, 255);
    }
    SDL_RenderFillRect((SDL_Renderer *)gui->renderer, &rect);
}

void gui_renderer_destroy(gui_renderer_t * gui) {
    if(gui == 0) {
        return;
    }
    if(gui->renderer != 0) {
        SDL_DestroyRenderer((SDL_Renderer *)gui->renderer);
    }
    if(gui->window != 0) {
        SDL_DestroyWindow((SDL_Window *)gui->window);
    }
    SDL_Quit();
}

uint32_t gui_renderer_poll_quit(void) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            return 1;
        }
    }
    return 0;
}
