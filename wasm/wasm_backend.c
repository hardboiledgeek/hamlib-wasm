/*
 * Hamlib WebAssembly Wrapper Backend - implementation
 * Copyright (c) 2024 by hardboiledgeek
 * 
 * This backend acts as a proxy/wrapper that intercepts I/O operations
 * and routes them to JavaScript callbacks while forwarding all radio
 * commands to the actual radio backend.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <hamlib/rig.h>
#include "wasm_backend.h"

// Global callback storage
wasm_callbacks_t g_wasm_callbacks = {0};

// Function called from JavaScript to register I/O callbacks
void hamlib_set_callbacks(
    js_radio_open_func_t open_cb,
    js_radio_close_func_t close_cb, 
    js_radio_write_func_t write_cb,
    js_radio_read_func_t read_cb
) {
    g_wasm_callbacks.open = open_cb;
    g_wasm_callbacks.close = close_cb;
    g_wasm_callbacks.write = write_cb;
    g_wasm_callbacks.read = read_cb;
}

/*
 * I/O Override Functions
 * These replace the standard write_block/read_block calls with JavaScript callbacks
 */

int wasm_write_block_override(hamlib_port_t *p, const unsigned char *txbuffer, size_t count)
{
    rig_debug(RIG_DEBUG_VERBOSE, "%s: writing %zu bytes via JavaScript\n", __func__, count);
    
    if (!g_wasm_callbacks.write) {
        rig_debug(RIG_DEBUG_ERR, "%s: JavaScript write callback not set\n", __func__);
        return -RIG_ECONF;
    }
    
    int result = g_wasm_callbacks.write(txbuffer, count);
    if (result < 0) {
        rig_debug(RIG_DEBUG_ERR, "%s: JavaScript write callback failed: %d\n",
                  __func__, result);
        return -RIG_EIO;
    }
    
    return result;
}

int wasm_read_block_override(hamlib_port_t *p, unsigned char *rxbuffer, size_t count)
{
    rig_debug(RIG_DEBUG_VERBOSE, "%s: reading up to %zu bytes via JavaScript\n", __func__, count);
    
    if (!g_wasm_callbacks.read) {
        rig_debug(RIG_DEBUG_ERR, "%s: JavaScript read callback not set\n", __func__);
        return -RIG_ECONF;
    }
    
    int result = g_wasm_callbacks.read(rxbuffer, count);
    if (result < 0) {
        rig_debug(RIG_DEBUG_ERR, "%s: JavaScript read callback failed: %d\n",
                  __func__, result);  
        return -RIG_EIO;
    }
    
    return result;
}

/*
 * Link-time I/O Function Overrides
 * These replace the original iofunc.c functions when building for WebAssembly
 */

// Override the internal port_write function from iofunc.c
// This gets called by write_block() and routes to JavaScript
ssize_t port_write(hamlib_port_t *p, const void *buf, size_t count)
{
    rig_debug(RIG_DEBUG_VERBOSE, "%s: intercepted write of %zu bytes\n", __func__, count);
    
    if (!g_wasm_callbacks.write) {
        rig_debug(RIG_DEBUG_ERR, "%s: JavaScript write callback not set\n", __func__);
        return -1;
    }
    
    int result = g_wasm_callbacks.write((const unsigned char*)buf, count);
    if (result < 0) {
        rig_debug(RIG_DEBUG_ERR, "%s: JavaScript write callback failed: %d\n",
                  __func__, result);
        return -1;
    }
    
    return result;
}

// We also need to override the read functions, but the read path is more complex
// Let's start with the basic port_read_generic function override
ssize_t port_read_generic(hamlib_port_t *p, void *buf, size_t count, int direct)
{
    rig_debug(RIG_DEBUG_VERBOSE, "%s: intercepted read of up to %zu bytes\n", __func__, count);
    
    if (!g_wasm_callbacks.read) {
        rig_debug(RIG_DEBUG_ERR, "%s: JavaScript read callback not set\n", __func__);
        return -1;
    }
    
    int result = g_wasm_callbacks.read((unsigned char*)buf, count);
    if (result < 0) {
        rig_debug(RIG_DEBUG_ERR, "%s: JavaScript read callback failed: %d\n",
                  __func__, result);
        return -1;
    }
    
    return result;
}

/*
 * WASM Wrapper Rig Creation Function
 * This creates a rig instance for any radio model
 * I/O is automatically intercepted via link-time function replacement
 */

RIG *hamlib_wasm_rig_init(rig_model_t real_radio_model)
{
    rig_debug(RIG_DEBUG_VERBOSE, "%s: creating WASM wrapper for model %d\n", 
              __func__, real_radio_model);
    
    // Validate callbacks are set
    if (!g_wasm_callbacks.open || !g_wasm_callbacks.close ||
        !g_wasm_callbacks.write || !g_wasm_callbacks.read) {
        rig_debug(RIG_DEBUG_ERR, "%s: JavaScript callbacks not set\n", __func__);
        return NULL;
    }
    
    // Create the real radio instance - I/O will be automatically intercepted
    RIG *real_rig = rig_init(real_radio_model);
    if (!real_rig) {
        rig_debug(RIG_DEBUG_ERR, "%s: failed to create real rig model %d\n", 
                  __func__, real_radio_model);
        return NULL;
    }
    
    rig_debug(RIG_DEBUG_VERBOSE, "%s: WASM wrapper created successfully\n", __func__);
    return real_rig;
}