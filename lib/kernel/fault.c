/**
  ******************************************************************************
  * @file    fault.c
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

#include "fault.h"
#include "f756zg/usart3.h"
#include "stm32f7xx.h"

/**
 * Writes a string directly over USART3 during fault handling.
 * @author Ian Wilkey
 */
static void fault_write(const char *s) {
    rtosk_bsp_f756zg_usart3_write_string(s);
}

/**
 * Writes a 32-bit value as hex.
 * @author Ian Wilkey
 */
static void fault_write_hex32(uint32_t value) {
    static const char hex[] = "0123456789ABCDEF";
    fault_write("0x");
    for(int32_t i = 28; i >= 0; i -= 4) {
        rtosk_bsp_f756zg_usart3_write_char(hex[(value >> i) & 0xFUL]);
    }
}

/**
 * Writes a named register value in hex.
 * @author Ian Wilkey
 */
static void fault_write_reg(const char *name, uint32_t value) {
    fault_write(name);
    fault_write(" : ");
    fault_write_hex32(value);
    fault_write("\r\n");
}

/**
 * Writes a decoded fault flag if the given bit is set.
 * @author Ian Wilkey
 */
static void fault_write_flag(uint32_t reg, uint32_t mask, const char *name) {
    if((reg & mask) != 0UL) {
        fault_write("  - ");
        fault_write(name);
    }
}

/**
 * Decodes EXC_RETURN to show stack, mode, and frame type.
 * @author Ian Wilkey
 */
static void fault_decode_exc_return(uint32_t exc_return) {
    fault_write("EXC_RETURN DECODE:\r\n");
    if((exc_return & (1UL << 2)) != 0UL) {
        fault_write("  - stack used: PSP\r\n");
    } else {
        fault_write("  - stack used: MSP\r\n");
    }
    if((exc_return & (1UL << 3)) != 0UL) {
        fault_write("  - return mode: THREAD\r\n");
    } else {
        fault_write("  - return mode: HANDLER\r\n");
    }
    if((exc_return & (1UL << 4)) != 0UL) {
        fault_write("  - frame type: BASIC\r\n");
    } else {
        fault_write("  - frame type: EXTENDED/FPU\r\n");
    }
}

/**
 * Decodes Cortex-M configurable fault status bits.
 * @author Ian Wilkey
 */
static void fault_decode_cfsr(uint32_t cfsr) {
    fault_write("CFSR DECODE:\r\n");
    fault_write_flag(cfsr, (1UL << 0),  "IACCVIOL: instruction access violation");
    fault_write_flag(cfsr, (1UL << 1),  "DACCVIOL: data access violation");
    fault_write_flag(cfsr, (1UL << 3),  "MUNSTKERR: fault during exception unstacking");
    fault_write_flag(cfsr, (1UL << 4),  "MSTKERR: fault during exception stacking");
    fault_write_flag(cfsr, (1UL << 5),  "MLSPERR: lazy FP stacking fault");
    fault_write_flag(cfsr, (1UL << 7),  "MMARVALID: MMFAR contains valid address");
    fault_write_flag(cfsr, (1UL << 8),  "IBUSERR: instruction bus error");
    fault_write_flag(cfsr, (1UL << 9),  "PRECISERR: precise data bus error");
    fault_write_flag(cfsr, (1UL << 10), "IMPRECISERR: imprecise data bus error");
    fault_write_flag(cfsr, (1UL << 11), "UNSTKERR: bus fault during exception unstacking");
    fault_write_flag(cfsr, (1UL << 12), "STKERR: bus fault during exception stacking");
    fault_write_flag(cfsr, (1UL << 13), "LSPERR: lazy FP bus fault");
    fault_write_flag(cfsr, (1UL << 15), "BFARVALID: BFAR contains valid address");
    fault_write_flag(cfsr, (1UL << 16), "UNDEFINSTR: undefined instruction");
    fault_write_flag(cfsr, (1UL << 17), "INVSTATE: invalid EPSR/Thumb state");
    fault_write_flag(cfsr, (1UL << 18), "INVPC: invalid exception return");
    fault_write_flag(cfsr, (1UL << 19), "NOCP: coprocessor access fault");
    fault_write_flag(cfsr, (1UL << 24), "UNALIGNED: unaligned access");
    fault_write_flag(cfsr, (1UL << 25), "DIVBYZERO: divide by zero");
}

/**
 * Decodes Cortex-M hard fault status bits.
 * @author Ian Wilkey
 */
static void fault_decode_hfsr(uint32_t hfsr) {
    fault_write("\r\nHFSR DECODE:\r\n");
    fault_write_flag(hfsr, (1UL << 1),  "VECTTBL: fault during vector table read");
    fault_write_flag(hfsr, (1UL << 30), "FORCED: configurable fault escalated to HardFault");
    fault_write_flag(hfsr, (1UL << 31), "DEBUGEVT: debug event caused HardFault");
}

/**
 * Prints a short human readable likely hard fault cause.
 * @author Ian Wilkey
 */
static void fault_print_fault_summary(uint32_t cfsr, uint32_t hfsr) {
    fault_write("\r\nLIKELY CAUSE:\r\n");
    if((cfsr & (1UL << 16)) != 0UL) {
        fault_write("  undefined instruction executed.\r\n");
    } else if((cfsr & (1UL << 18)) != 0UL) {
        fault_write("  invalid exception return or corrupted stack frame.\r\n");
    } else if((cfsr & (1UL << 1)) != 0UL) {
        fault_write("  invalid data memory access.\r\n");
    } else if((cfsr & (1UL << 9)) != 0UL) {
        fault_write("  precise bus fault. Check BFAR if valid.\r\n");
    } else if((cfsr & (1UL << 24)) != 0UL) {
        fault_write("  unaligned memory access.\r\n");
    } else if((cfsr & (1UL << 25)) != 0UL) {
        fault_write("  divide by zero.\r\n");
    } else if((hfsr & (1UL << 30)) != 0UL) {
        fault_write("  configurable fault escalated to HardFault.\r\n");
    } else {
        fault_write("  unknown. check PC, LR, CFSR, and HFSR.\r\n");
    }
}

__attribute__((noreturn)) void rtosk_fault_hardfault_handler(uint32_t * stacked_registers, uint32_t exc_return) {
    __disable_irq();
    fault_write("\r\n");
    fault_write("========================================\r\n");
    fault_write("           TinyRTOS HARDFAULT           \r\n");
    fault_write("========================================\r\n");
    fault_write_reg("EXC_RETURN", exc_return);
    fault_write_reg("R0 ", stacked_registers[0]);
    fault_write_reg("R1 ", stacked_registers[1]);
    fault_write_reg("R2 ", stacked_registers[2]);
    fault_write_reg("R3 ", stacked_registers[3]);
    fault_write_reg("R12", stacked_registers[4]);
    fault_write_reg("LR ", stacked_registers[5]);
    fault_write_reg("PC ", stacked_registers[6]);
    fault_write_reg("xPSR", stacked_registers[7]);
    uint32_t cfsr = SCB->CFSR;
    uint32_t hfsr = SCB->HFSR;
    fault_write("----------------------------------------\r\n");
    fault_write_reg("CFSR", cfsr);
    fault_write_reg("HFSR", hfsr);
    fault_write_reg("DFSR", SCB->DFSR);
    fault_write_reg("AFSR", SCB->AFSR);
    fault_write_reg("MMFAR", SCB->MMFAR);
    fault_write_reg("BFAR", SCB->BFAR);
    fault_write("----------------------------------------\r\n");
    fault_decode_exc_return(exc_return);
    fault_decode_cfsr(cfsr);
    fault_decode_hfsr(hfsr);
    fault_print_fault_summary(cfsr, hfsr);
    fault_write("========================================\r\n");
    fault_write("SYSTEM HALTED.\r\n");
    for(;;) {
        __asm volatile ("bkpt #0");
    }
}
