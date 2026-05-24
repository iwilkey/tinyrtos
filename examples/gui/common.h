/**
  ******************************************************************************
  * @file    common.h
  * @author  Ian Wilkey
  * @brief   A shared constants file for both the TinyRTOS framebuffer tasks and
  *          the companion renderer.
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

#ifndef _TINYRTOS_COMMON_H_
#define _TINYRTOS_COMMON_H_

#define TINYRTOS_FRAMEBUFFER_WIDTH  0xffU
#define TINYRTOS_FRAMEBUFFER_HEIGHT ((TINYRTOS_FRAMEBUFFER_WIDTH * 9U) / 16U)
#define TINYRTOS_RENDER_SCALE       4U

#define TINYRTOS_GUI_SOF0 0xA5
#define TINYRTOS_GUI_SOF1 0x5A

#define TINYRTOS_GUI_CMD_CLEAR 0x01
#define TINYRTOS_GUI_CMD_PIXEL 0x02
#define TINYRTOS_GUI_CMD_LINE  0x03
#define TINYRTOS_GUI_CMD_PRESENT 0x04

#endif /// _TINYRTOS_COMMON_H_
