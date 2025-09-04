/*
 * Hamlib WebAssembly Wrapper Backend - header
 * Copyright (c) 2024 by Hardboiled Geek, LLC
 * 
 * This backend acts as a proxy/wrapper around real radio backends,
 * intercepting I/O operations and routing them to JavaScript callbacks.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _WASM_BACKEND_H
#define _WASM_BACKEND_H 1

#include <hamlib/rig.h>

// JavaScript callback function types
typedef int (*js_radio_open_func_t)(const char *pathname, int baud_rate);
typedef int (*js_radio_close_func_t)(void);
typedef int (*js_radio_write_func_t)(const unsigned char *buffer, size_t count);
typedef int (*js_radio_read_func_t)(unsigned char *buffer, size_t max_count);

// Callback registration structure
typedef struct {
    js_radio_open_func_t  open;
    js_radio_close_func_t close;
    js_radio_write_func_t write;
    js_radio_read_func_t  read;
} wasm_callbacks_t;

// WASM wrapper private data - stores the real radio instance
typedef struct {
    RIG *real_rig;              // The actual radio backend instance
    rig_model_t real_model;     // Model ID of the real radio
} wasm_priv_data_t;

// Global callback storage
extern wasm_callbacks_t g_wasm_callbacks;

// Functions called from JavaScript
extern HAMLIB_EXPORT(void) hamlib_set_callbacks(
    js_radio_open_func_t open_cb,
    js_radio_close_func_t close_cb,
    js_radio_write_func_t write_cb,
    js_radio_read_func_t read_cb
);

// Create a WASM wrapper for any radio model
extern HAMLIB_EXPORT(RIG *) hamlib_wasm_rig_init(rig_model_t real_radio_model);

// Override functions for intercepting I/O
extern int wasm_write_block_override(hamlib_port_t *p, const unsigned char *txbuffer, size_t count);
extern int wasm_read_block_override(hamlib_port_t *p, unsigned char *rxbuffer, size_t count);

#endif /* _WASM_BACKEND_H */