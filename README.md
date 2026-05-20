# tinyrtos

A compact, preemptive priority RTOS kernel for ARM Cortex-M, written completely from scratch in C.

* Author: Ian Wilkey
* Start Date: May 19th, 2026
* License: MIT
* Target MCU: STM32F756ZG (ARM Cortex-M7)
* Toolchain: PlatformIO + GCC ARM
* **Approximate footprint: ~6 KB RAM, ~6 KB Flash**

---

## Table of Contents

- [Why I Built This](#why-i-built-this)
- [Current Features](#current-features)
- [Public API](#public-api)
- [OSI (Operating System Interface)](#osi-operating-system-interface)
- [Current Limitations](#current-limitations)
- [Official Planned Improvements](#official-planned-improvements)
- [Building](#building)
- [Serial Monitor / OSI Access](#serial-monitor--osi-access)
- [Contributing](#contributing)
- [Educational Use](#educational-use)

---

## Why I Built This

**tinyrtos** was built as a learning project to fully understand how a real-time operating system works internally on ARM Cortex-M hardware without relying on existing RTOS implementations such as FreeRTOS or Zephyr.

The goal of this project was to implement the core concepts manually and understand the architecture from first principles.

This project intentionally stays very close to the hardware and avoids unnecessary abstraction layers.

---

## Current Features

**tinyrtos** currently includes:

* Preemptive priority scheduler
* PendSV context switching
* SVC-based task startup
* SysTick kernel tick
* Separate PSP task stacks
* Round-robin scheduling within priorities
* Task sleep/blocking
* Idle task
* Critical sections
* Counting semaphores
* Queues
* Mutexes
* ISR-safe synchronization APIs
* UART shell / OSI (Operating System Interface)
* STM32 board support package (BSP)

---

## Public API

The public tinyrtos API is intentionally small and subject to change.

### Kernel

```C
void rtosk_kernel_systick_init(void);
void rtosk_kernel_create_task(rtosk_task_func_t task_func, uint32_t priority);
void rtosk_kernel_start(void);
void rtosk_kernel_sleep_ms(uint32_t ms);
void rtosk_kernel_yield(void);
uint32_t rtosk_kernel_get_ticks(void);
```

### Synchronization

```C
void rtosk_semaphore_init(rtosk_semaphore_t * sem, uint32_t initial_count);
void rtosk_semaphore_take(rtosk_semaphore_t * sem);
void rtosk_semaphore_give(rtosk_semaphore_t * sem);
void rtosk_semaphore_give_from_isr(rtosk_semaphore_t * sem);

void rtosk_mutex_init(rtosk_mutex_t * mutex);
void rtosk_mutex_lock(rtosk_mutex_t * mutex);
void rtosk_mutex_unlock(rtosk_mutex_t * mutex);
```

### Queues

```C
void rtosk_queue_init(rtosk_queue_t * queue, void * buffer, uint32_t item_size, uint32_t capacity);
uint32_t rtosk_queue_send(rtosk_queue_t * queue, const void * item);
uint32_t rtosk_queue_send_from_isr(rtosk_queue_t * queue, const void * item);
uint32_t rtosk_queue_receive(rtosk_queue_t * queue, void * item);
```

### Nucleo-F756ZG Board Support Package

```C
void rtosk_bsp_f756zg_on_board_led_init(void);
void rtosk_bsp_f756zg_set_on_board_led(const bsp_led_t led, const uint8_t state);
void rtosk_bsp_f756zg_toggle_on_board_led(const bsp_led_t led);

void rtosk_bsp_f756zg_usart3_init(uint32_t baud);
void rtosk_bsp_f756zg_usart3_write_char(char c);
void rtosk_bsp_f756zg_usart3_write_string(const char *s);
void rtosk_bsp_f756zg_uart_enable_rx_interrupt(void);
void rtosk_bsp_f756zg_usart3_rx_callback_from_isr(uint8_t c);
```

Any API added or changed through contributions must be included/updated here.

---

## OSI (Operating System Interface)

**tinyrtos** includes a lightweight UART shell called the OSI.

The OSI allows interaction with the RTOS at runtime through a serial terminal.

Example commands:

```text
help
ticks
red on
red off
```

The OSI is intended to become the primary diagnostics and debugging interface for the kernel.

---

## Current Limitations

**tinyrtos** is intentionally small and educational in scope.

The kernel currently does NOT include:

* Priority inheritance
* BASEPRI interrupt masking
* Dynamic memory allocation
* Stack overflow detection
* Memory protection (MPU)
* Tickless idle
* SMP or multicore support
* Userspace/process isolation
* Filesystems
* Networking
* Software timers
* Event groups
* Task deletion

That said, the absence of these features today does not mean they will never exist. **Future development may include many of the advanced capabilities listed above, either by myself or contributors.**

As it currently stands, tinyrtos is functional enough to support real embedded firmware applications, especially those that require a small footprint and deterministic behavior. 

*However*, it is still primarily an **educational and experimental RTOS kernel** and should **not yet** be considered production-ready due to the lack of advanced safety, debugging, memory protection, and fault-handling mechanisms...

*you're more than welcome to add those yourself, though* ;)

---

## Official Planned Improvements

Future planned improvements include:

* Additional OSI commands
* Fault diagnostics and crash reporting
* HardFault register dump support
* Stack watermark checking
* Logging subsystem
* Event flags / event groups
* Improved scheduler diagnostics
* Runtime task statistics
* Better BSP abstraction
* Additional board support

---

## Building

I built this project using **PlatformIO inside VSCode**, so I didn't have to mess with writing the linker script, compilation, flashing, and UART monitoring.

### Requirements

* VSCode
* PlatformIO extension
* STM32 Nucleo-F756ZG board

### Build and Flash

1. Clone the repository
2. Open the project in VSCode
3. Connect the STM32 board through USB/ST-Link
4. Build and upload through PlatformIO

---

## Serial Monitor / OSI Access

After flashing, open a serial monitor at:

```text
115200 baud
8-N-1
```

Example using PlatformIO monitor:

```bash
pio device monitor -b 115200
```

You should see the OSI boot banner and be able to interact with the system through UART commands.

You may also use my provided *run.sh* which will compile, flash, and open a monitor in one fell swoop. A Windows batch or PowerShell equivalent is not included yet, but contributions are welcome.

```bash
cd tinyrtos
./run.sh
```

PlatformIO can be installed through:

- Homebrew (macOS)
- Chocolatey (Windows)
- pip (Linux/macOS/Windows)

---

## Contributing

Contributions, ideas, critiques, and pull requests are all welcome.

This project was created primarily for learning and experimentation, so discussion around architecture tradeoffs, implementation quality, and RTOS design decisions is encouraged.

---

## Educational Use

This project may be freely used for:

* embedded systems education
* RTOS demonstrations
* experimentation
* firmware development
* commercial products (see Current Limitations section, though!)

under the terms of the MIT license.
