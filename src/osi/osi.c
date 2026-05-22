/**
  ******************************************************************************
  * @file    osi.c
  * @author  Ian Wilkey
  * @brief   A compact, preemptive priority RTOS kernel for ARM Cortex-M, 
  *          written from scratch in C.
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

#include <tinyrtos/osi/osi.h>
#include <tinyrtos/kernel/task.h>

#include <stm32f7xx.h>

/**
 * Whether or not the RTOSK OSI has been initialized this runtime.
 */
static uint8_t OSI_INIT = 0U;

/**
 * Queue for incoming RX data over USART3 line; pushed from ISR, popped from task.
 */
static rtosk_queue_t OSI_USART3_RX_QUEUE;

/**
 * Mutex for outgoing USART3 TX's so that only one task is allowed to write at a time.
 */
static rtosk_mutex_t OSI_USART3_TX_MUTEX;

/**
 * Buffer for incoming RX data from client-side OSI interface.
 */
static uint8_t OSI_USART3_RX_QUEUE_STORAGE[RTOSK_OSI_CMD_BUFFER_SIZE];

/**
 * Called from USART3 RX ISR; uses queue primitive to save data and gets out.
 * @author Ian Wilkey
 */
void rtosk_bsp_f756zg_usart3_rx_callback_from_isr(uint8_t c) {
    rtosk_queue_send_from_isr(&OSI_USART3_RX_QUEUE, &c);
}

/**
 * Writes a null terminted string through USART3 TX to client machine using a mutex 
 * to ensure no other tasks can write out until the operation has finished.
 * @author Ian Wilkey
 */
static void rtosk_osi_usart3_write_locked(const char *s) {
    rtosk_mutex_lock(&OSI_USART3_TX_MUTEX);
    rtosk_bsp_f756zg_usart3_write_string(s);
    rtosk_mutex_unlock(&OSI_USART3_TX_MUTEX);
}

/**
 * Invokes given OSI command.
 * @author Ian Wilkey
 */
static void osi_invoke(osi_cmd_t cmd, const char * cmd_literal) {
    switch(cmd) {
        case OSI_CMD_HELP:
            rtosk_osi_usart3_write_locked("\rcommands:\r\n");
            for(uint32_t i = 1; i < OSI_CMD_COUNT; i++) {
                rtosk_osi_usart3_write_locked("  ");
                rtosk_osi_usart3_write_locked(OSI_COMMAND_LITERALS[i]);
                rtosk_osi_usart3_write_locked("\r\n");
            }
            break;
        case OSI_CMD_TASKS: {
            char line[128];
            rtosk_task_info_t info;
            rtosk_osi_usart3_write_locked("\rID  PRI  STATE   USED  FREE  NAME\r\n");
            rtosk_osi_usart3_write_locked("-----------------------------------\r\n");
            for(uint32_t i = 0UL; i < RTOSK_TOTAL_TASK_SLOTS; i++) {
                if(rtosk_kernel_get_task_info(i, &info) != 0UL) {
                    snprintf(
                        line,
                        sizeof(line),
                        "%lu   %lu    %-6s  %luB   %luB   %s\r\n",
                        (unsigned long)info.index,
                        (unsigned long)info.priority,
                        rtosk_task_state_to_string(info.state),
                        (unsigned long)(info.stack_used_words * 4UL),
                        (unsigned long)(info.stack_free_words * 4UL),
                        info.name
                    );
                    rtosk_osi_usart3_write_locked(line);
                }
            }
            break;
        }
        case OSI_CMD_TICKS: {
            char out[48];
            snprintf(
                out,
                sizeof(out),
                "\rticks: %lu\r\n",
                (unsigned long)rtosk_kernel_get_ticks()
            );
            rtosk_osi_usart3_write_locked(out);
            break;
        }
        case OSI_CMD_UPTIME: {
            char out[64];
            uint32_t ticks = rtosk_kernel_get_ticks();
            uint32_t seconds = ticks / 1000UL;
            uint32_t ms = ticks % 1000UL;
            snprintf(
                out,
                sizeof(out),
                "\ruptime: %lu.%03lu sec\r\n",
                (unsigned long)seconds,
                (unsigned long)ms
            );
            rtosk_osi_usart3_write_locked(out);
            break;
        }
        case OSI_CMD_FREQ: {
            char out[64];
            snprintf(
                out,
                sizeof(out),
                "\rcpu: %lu Hz\r\n",
                (unsigned long)rtosk_get_cpu_freq()
            );
            rtosk_osi_usart3_write_locked(out);
            break;
        }
        case OSI_CMD_ABOUT:
            rtosk_osi_usart3_write_locked(
                "\rtinyrtos\r\n"
                "Version : " RTOSK_KERNEL_VERSION "\r\n"
                "Author  : Ian Wilkey\r\n"
                "Target  : STM32F756ZG\r\n"
                "Mode    : Preemptive Priority RTOS\r\n"
                "License : MIT\r\n"
            );
            break;
        case OSI_CMD_FAULT:
            rtosk_osi_usart3_write_locked("\rtriggering HardFault...");
            __asm volatile ("udf #0");
            break;
        case OSI_CMD_REBOOT:
            rtosk_osi_usart3_write_locked("\rrebooting...\r\n");
            rtosk_kernel_delay_ms(250);
            NVIC_SystemReset();
            break;
        case OSI_CMD_UNKNOWN:
        default:
            rtosk_osi_usart3_write_locked("\runknown command: ");
            rtosk_osi_usart3_write_locked(cmd_literal);
            rtosk_osi_usart3_write_locked("\r\n");
    }
}

/**
 * Handles simple RTOSK commands coming in from USART RX (sent from client machine.)
 * @author Ian Wilkey
 */
static void rtosk_osi_dispatch_command(const char * cmd) {
    for(uint32_t i = 1; i < OSI_CMD_COUNT; i++) {
        if(!strcmp(cmd, OSI_COMMAND_LITERALS[i])) {
            osi_invoke((osi_cmd_t)i, cmd);
            return;
        }
    }
    osi_invoke(OSI_CMD_UNKNOWN, cmd);
}

/**
 * Task to wait on USART3 RX queue data, buffer it, and handle commands when they are terminated.
 * @author Ian Wilkey
 */
static void rtosk_osi_task(void) {
    uint8_t c;
    char cmd[RTOSK_OSI_CMD_BUFFER_SIZE];
    uint32_t len = 0UL;
    uint32_t saw_cr = 0UL;
    rtosk_kernel_delay_ms(1000);
    rtosk_osi_usart3_write_locked(
        "\r"
        "========================================\r\n"
        "              TinyRTOS BOOT             \r\n"
        "========================================\r\n"
        "  Author : Ian Wilkey\r\n"
        "  Target : STM32F756ZG\r\n"
        "  Mode   : Preemptive Priority RTOS\r\n"
        "----------------------------------------\r\n"
        "  This is the built-in OSI for simple   \r\n"
        "  commands and OS exploration.          \r\n"
        "  Type 'help' for available commands.   \r\n"
        "========================================\r\n"
        "\r"
    );
    rtosk_osi_usart3_write_locked("> ");
    for(;;) {
        rtosk_queue_receive(&OSI_USART3_RX_QUEUE, &c);
        if(c == '\r') {
            saw_cr = 1UL;
            rtosk_osi_usart3_write_locked("\r\n");
            if(len > 0UL) {
                cmd[len] = '\0';
                rtosk_osi_dispatch_command(cmd);
                len = 0UL;
            }
            rtosk_osi_usart3_write_locked("> ");
        } else if(c == '\n') {
            if(saw_cr != 0UL) {
                saw_cr = 0UL;
                continue;
            }
            rtosk_osi_usart3_write_locked("\r\n");
            if(len > 0UL) {
                cmd[len] = '\0';
                rtosk_osi_dispatch_command(cmd);
                len = 0UL;
            }
            rtosk_osi_usart3_write_locked("> ");
        } else {
            saw_cr = 0UL;
            if(c == 0x08U || c == 0x7FU) {
                if(len > 0UL) {
                    len--;
                    rtosk_osi_usart3_write_locked("\b \b");
                }
            } else {
                if(len < (RTOSK_OSI_CMD_BUFFER_SIZE - 1UL)) {
                    cmd[len++] = (char)c;
                    /// echo out!
                    rtosk_mutex_lock(&OSI_USART3_TX_MUTEX);
                    rtosk_bsp_f756zg_usart3_write_char((char)c);
                    rtosk_mutex_unlock(&OSI_USART3_TX_MUTEX);
                }
            }
        }
    }
}

void rtosk_osi_init(uint32_t baud, uint32_t priority) {
    if(OSI_INIT) return;
    rtosk_bsp_f756zg_usart3_init(baud);
    rtosk_queue_init(
        &OSI_USART3_RX_QUEUE, 
        OSI_USART3_RX_QUEUE_STORAGE, 
        sizeof(uint8_t), 
        RTOSK_OSI_CMD_BUFFER_SIZE
    );
    rtosk_mutex_init(&OSI_USART3_TX_MUTEX);
    rtosk_bsp_f756zg_uart_enable_rx_interrupt();
    rtosk_kernel_create_task(rtosk_osi_task, priority, "osi");
    OSI_INIT = 1U;
}
