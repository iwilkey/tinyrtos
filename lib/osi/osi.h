/**
  ******************************************************************************
  * @file    osi.h
  * @author  Ian Wilkey
  * @brief   A compact, preemptive priority RTOS kernel for ARM Cortex-M, 
  *          written from scratch in C on STM32.
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

#ifndef _RTOSK_OSI_H_
#define _RTOSK_OSI_H_

#include <stdio.h>
#include <string.h>

#include "kernel.h"
#include "queue.h"
#include "mutex.h"

#include "f756zg/led.h"
#include "f756zg/usart3.h"

/**
 * The maximum amount of data reserved for incoming OSI commands.
 */
#define RTOSK_OSI_CMD_BUFFER_SIZE 128UL

/**
 * Initializes RTOSK's tiny Operating System Interface (OSI) at given baud and task priority.
 * @author Ian Wilkey
 */
void rtosk_osi_init(uint32_t baud, uint32_t priority);

#endif /// _RTOSK_OSI_H_
