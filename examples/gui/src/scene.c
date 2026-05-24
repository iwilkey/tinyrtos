/**
  ******************************************************************************
  * @file    scene.c
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

#include <scene.h>
#include <primitives.h>

#include "../common.h"

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} vec3_t;

typedef struct {
    int16_t x;
    int16_t y;
} vec2_t;

static const vec3_t CUBE[8] = {
    {-16, -16, -16}, { 16, -16, -16},
    { 16,  16, -16}, {-16,  16, -16},
    {-16, -16,  16}, { 16, -16,  16},
    { 16,  16,  16}, {-16,  16,  16}
};

static const uint8_t EDGES[12][2] = {
    {0,1}, {1,2}, {2,3}, {3,0},
    {4,5}, {5,6}, {6,7}, {7,4},
    {0,4}, {1,5}, {2,6}, {3,7}
};

static const int8_t SIN_LUT[64] = {
      0,  12,  25,  37,  49,  60,  71,  81,
     90,  98, 106, 112, 118, 122, 125, 127,
    127, 127, 125, 122, 118, 112, 106,  98,
     90,  81,  71,  60,  49,  37,  25,  12,
      0, -12, -25, -37, -49, -60, -71, -81,
    -90, -98,-106,-112,-118,-122,-125,-127,
   -127,-127,-125,-122,-118,-112,-106, -98,
    -90, -81, -71, -60, -49, -37, -25, -12
};

static const uint8_t EDGE_COLORS[12] = {
    RGB332(7,0,0), RGB332(0,7,0), RGB332(0,0,3), RGB332(7,7,0),
    RGB332(7,0,3), RGB332(0,7,3), RGB332(7,3,0), RGB332(3,7,0),
    RGB332(3,0,3), RGB332(0,3,3), RGB332(7,7,7), RGB332(4,4,7)
};

static int16_t fsin(uint8_t a) {
    return SIN_LUT[a & 63U];
}

static int16_t fcos(uint8_t a) {
    return SIN_LUT[(a + 16U) & 63U];
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

static uint8_t angle = 0U;

void scene_render(void) {
    vec2_t p[8];
    for(uint8_t i = 0; i < 8; i++) {
        p[i] = project(CUBE[i], angle, angle + 11U);
    }
    gui_clear(0);
    for(uint8_t i = 0; i < 12; i++) {
        uint8_t a = EDGES[i][0];
        uint8_t b = EDGES[i][1];
        gui_draw_line(
            (uint8_t)p[a].x,
            (uint8_t)p[a].y,
            (uint8_t)p[b].x,
            (uint8_t)p[b].y,
            EDGE_COLORS[i]
        );
    }
    gui_present();
    angle++;
}
