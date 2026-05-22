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

static uint32_t port_name_matches(const char * name) {
    if(name == 0) {
        return 0;
    }
    return strstr(name, "usbmodem") != 0 ||
           strstr(name, "ttyACM") != 0 ||
           strstr(name, "ttyUSB") != 0 ||
           strstr(name, "COM") == name;
}

uint32_t gui_serial_open_auto(gui_serial_t * serial, uint32_t baud) {
    struct sp_port ** ports;
    if(serial == 0) {
        return 0;
    }
    serial->port = 0;
    if(sp_list_ports(&ports) != SP_OK) {
        return 0;
    }
    for(uint32_t i = 0; ports[i] != 0; i++) {
        const char * name = sp_get_port_name(ports[i]);
        if(port_name_matches(name)) {
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
    if(serial == 0 || serial->port == 0 || buffer == 0) {
        return -1;
    }
    return sp_nonblocking_read((struct sp_port *)serial->port, buffer, len);
}

int32_t gui_serial_write(gui_serial_t * serial, const uint8_t * buffer, uint32_t len) {
    if(serial == 0 || serial->port == 0 || buffer == 0) {
        return -1;
    }
    return sp_blocking_write((struct sp_port *)serial->port, buffer, len, 100);
}

void gui_serial_close(gui_serial_t * serial) {
    if(serial == 0 || serial->port == 0) {
        return;
    }
    sp_close((struct sp_port *)serial->port);
    sp_free_port((struct sp_port *)serial->port);
    serial->port = 0;
}
