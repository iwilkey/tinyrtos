/**
  ******************************************************************************
  * @file    serial.c
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

#include <serial.h>
#include <libserialport.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
static void reconnect_sleep(void) {
    Sleep(500);
}
#else
#include <unistd.h>
static void reconnect_sleep(void) {
    usleep(500000);
}
#endif

static uint32_t gui_port_name_matches(const char * name) {
    if(name == 0) {
        return 0;
    }
    return strstr(name, "usbmodem") != 0 ||
           strstr(name, "ttyACM") != 0 ||
           strstr(name, "ttyUSB") != 0 ||
           strstr(name, "COM3") == name;
}

static uint32_t gui_port_still_exists(const char * target_name) {
    struct sp_port ** ports;
    if(target_name == 0 || target_name[0] == 0) {
        return 0;
    }
    if(sp_list_ports(&ports) != SP_OK) {
        return 0;
    }
    for(uint32_t i = 0; ports[i] != 0; i++) {
        const char * name = sp_get_port_name(ports[i]);
        if(name != 0 && strcmp(name, target_name) == 0) {
            sp_free_port_list(ports);
            return 1;
        }
    }
    sp_free_port_list(ports);
    return 0;
}

static uint32_t gui_port_is_healthy(gui_serial_t * serial) {
    enum sp_signal signals;
    if(serial == 0 || serial->port == 0) {
        return 0;
    }
    if(gui_port_still_exists(serial->port_name) == 0) {
        return 0;
    }
    if(sp_get_signals((struct sp_port *)serial->port, &signals) != SP_OK) {
        return 0;
    }
    return 1;
}

static void gui_serial_reconnect(gui_serial_t * serial) {
    uint32_t baud = serial->baud;
    gui_serial_close(serial);
    while(gui_serial_open_auto(serial, baud) == 0) {
        reconnect_sleep();
    }
}

uint32_t gui_serial_open_auto(gui_serial_t * serial, uint32_t baud) {
    struct sp_port ** ports;
    if(serial == 0) {
        return 0;
    }
    serial->port = 0;
    serial->baud = baud;
    if(sp_list_ports(&ports) != SP_OK) {
        return 0;
    }
    for(uint32_t i = 0; ports[i] != 0; i++) {
        const char * name = sp_get_port_name(ports[i]);
        if(gui_port_name_matches(name)) {
            struct sp_port * port;
            if(sp_copy_port(ports[i], &port) != SP_OK) {
                continue;
            }
            if(sp_open(port, SP_MODE_READ_WRITE) == SP_OK) {
                sp_set_baudrate(port, (int)baud);
                sp_set_bits(port, 8);
                sp_set_parity(port, SP_PARITY_NONE);
                sp_set_stopbits(port, 1);
                sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);
                serial->port = port;
                strncpy(serial->port_name, name, sizeof(serial->port_name) - 1);
                serial->port_name[sizeof(serial->port_name) - 1] = 0;
                sp_free_port_list(ports);
                return 1;
            }
            sp_free_port(port);
        }
    }
    sp_free_port_list(ports);
    return 0;
}

int32_t gui_serial_read(gui_serial_t * serial, uint8_t * buffer, uint32_t len) {
    if(serial == 0 || buffer == 0) {
        return -1;
    }
    while(1) {
        if(serial->port == 0) {
            gui_serial_reconnect(serial);
        }
        int32_t result = sp_nonblocking_read((struct sp_port *)serial->port, buffer, len);
        if(result > 0) {
            return result;
        }
        if(result == 0) {
            if(gui_port_is_healthy(serial) == 0) {
                gui_serial_reconnect(serial);
                continue;
            }

            return 0;
        }
        gui_serial_reconnect(serial);
    }
}

int32_t gui_serial_write(gui_serial_t * serial, const uint8_t * buffer, uint32_t len) {
    if(serial == 0 || serial->port == 0 || buffer == 0) {
        return -1;
    }
    return sp_blocking_write((struct sp_port *)serial->port, buffer, len, 100);
}

void gui_serial_close(gui_serial_t * serial) {
    if(serial == 0) {
        return;
    }
    if(serial->port != 0) {
        sp_close((struct sp_port *)serial->port);
        sp_free_port((struct sp_port *)serial->port);
        serial->port = 0;
    }
    serial->port_name[0] = 0;
}
