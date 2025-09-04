/*
 * Hamlib WebAssembly Bindings
 * Copyright (c) 2024 by Hardboiled Geek, LLC
 * 
 * Emscripten bindings to export Hamlib functions to JavaScript
 * This file handles the interface between WebAssembly and JavaScript
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

#include <emscripten.h>
#include <emscripten/bind.h>

#include <hamlib/rig.h>
#include "wasm_backend.h"

/*
 * Core Hamlib Function Exports
 * These expose the main Hamlib API functions to JavaScript
 */

// Initialize a WASM-wrapped rig for the specified model
EMSCRIPTEN_KEEPALIVE
RIG *wasm_rig_init(int model_id) {
    return hamlib_wasm_rig_init((rig_model_t)model_id);
}

// Open the rig connection (will call JavaScript open callback)
EMSCRIPTEN_KEEPALIVE
int wasm_rig_open(RIG *rig) {
    if (!rig) return -RIG_EINVAL;
    return rig_open(rig);
}

// Close the rig connection (will call JavaScript close callback)
EMSCRIPTEN_KEEPALIVE
int wasm_rig_close(RIG *rig) {
    if (!rig) return -RIG_EINVAL;
    return rig_close(rig);
}

// Cleanup and free rig resources
EMSCRIPTEN_KEEPALIVE
int wasm_rig_cleanup(RIG *rig) {
    if (!rig) return -RIG_EINVAL;
    return rig_cleanup(rig);
}

// Set rig frequency
EMSCRIPTEN_KEEPALIVE
int wasm_rig_set_freq(RIG *rig, int vfo, double freq) {
    if (!rig) return -RIG_EINVAL;
    return rig_set_freq(rig, (vfo_t)vfo, (freq_t)freq);
}

// Get rig frequency  
EMSCRIPTEN_KEEPALIVE
int wasm_rig_get_freq(RIG *rig, int vfo, double *freq) {
    if (!rig || !freq) return -RIG_EINVAL;
    freq_t f;
    int ret = rig_get_freq(rig, (vfo_t)vfo, &f);
    *freq = (double)f;
    return ret;
}

// Set rig mode and passband
EMSCRIPTEN_KEEPALIVE
int wasm_rig_set_mode(RIG *rig, int vfo, int mode, int width) {
    if (!rig) return -RIG_EINVAL;
    return rig_set_mode(rig, (vfo_t)vfo, (rmode_t)mode, (pbwidth_t)width);
}

// Get rig mode and passband
EMSCRIPTEN_KEEPALIVE
int wasm_rig_get_mode(RIG *rig, int vfo, int *mode, int *width) {
    if (!rig || !mode || !width) return -RIG_EINVAL;
    rmode_t m;
    pbwidth_t w;
    int ret = rig_get_mode(rig, (vfo_t)vfo, &m, &w);
    *mode = (int)m;
    *width = (int)w;
    return ret;
}

// Set PTT (Push-to-Talk)
EMSCRIPTEN_KEEPALIVE
int wasm_rig_set_ptt(RIG *rig, int vfo, int ptt) {
    if (!rig) return -RIG_EINVAL;
    return rig_set_ptt(rig, (vfo_t)vfo, (ptt_t)ptt);
}

// Get PTT status
EMSCRIPTEN_KEEPALIVE
int wasm_rig_get_ptt(RIG *rig, int vfo, int *ptt) {
    if (!rig || !ptt) return -RIG_EINVAL;
    ptt_t p;
    int ret = rig_get_ptt(rig, (vfo_t)vfo, &p);
    *ptt = (int)p;
    return ret;
}

// Set connection parameters (serial port path, baud rate)
EMSCRIPTEN_KEEPALIVE
void wasm_rig_set_conf(RIG *rig, const char *pathname, int baud_rate) {
    if (!rig || !pathname) return;
    
    hamlib_port_t *rp = &rig->state.rigport;
    strncpy(rp->pathname, pathname, HAMLIB_FILPATHLEN - 1);
    rp->pathname[HAMLIB_FILPATHLEN - 1] = '\0';
    
    rp->parm.serial.rate = baud_rate;
    rp->parm.serial.data_bits = 8;
    rp->parm.serial.stop_bits = 1;
    rp->parm.serial.parity = RIG_PARITY_NONE;
    rp->parm.serial.handshake = RIG_HANDSHAKE_NONE;
}

/*
 * JavaScript Callback Registration
 * These functions are called from JavaScript to register I/O callbacks
 */

// This function is already defined in wasm_backend.c, just make sure it's exported
// EMSCRIPTEN_KEEPALIVE is in the header

/*
 * Utility Functions
 */

// Get rig model name by ID (useful for JavaScript)
EMSCRIPTEN_KEEPALIVE
const char *wasm_rig_get_model_name(int model_id) {
    const struct rig_caps *caps = rig_get_caps((rig_model_t)model_id);
    return caps ? caps->model_name : "Unknown";
}

// Get available rig models count (for enumeration)
EMSCRIPTEN_KEEPALIVE
int wasm_rig_get_model_count(void) {
    // This is a simplified count - in reality you'd iterate through rig_list
    // For now, return a reasonable number for testing
    return 100; // Placeholder
}

/*
 * Memory Management Helpers
 * WebAssembly needs explicit memory management
 */

// Allocate memory for frequency value (JavaScript can't directly handle pointers)
EMSCRIPTEN_KEEPALIVE
double *wasm_alloc_double(void) {
    return (double *)malloc(sizeof(double));
}

// Allocate memory for integer value
EMSCRIPTEN_KEEPALIVE
int *wasm_alloc_int(void) {
    return (int *)malloc(sizeof(int));
}

// Free allocated memory
EMSCRIPTEN_KEEPALIVE
void wasm_free(void *ptr) {
    if (ptr) free(ptr);
}