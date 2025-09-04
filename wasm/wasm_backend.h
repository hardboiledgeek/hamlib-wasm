/*
 * Hamlib WebAssembly Wrapper Backend - header
 * Copyright (c) 2024 by hardboiledgeek
 * 
 * This backend acts as a proxy/wrapper around real radio backends,
 * intercepting I/O operations and routing them to JavaScript callbacks.
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