# Hamlib WebAssembly Port

A WebAssembly port of [Hamlib](https://github.com/Hamlib/Hamlib) that enables amateur radio control directly from web browsers using the Web Serial API.

## Overview

This project creates a WebAssembly module that exposes Hamlib's radio control functionality to JavaScript applications. Instead of directly accessing serial ports, it uses JavaScript callback functions that can be implemented using the Web Serial API, allowing radio control from browsers without requiring native applications.

## Architecture

```
JavaScript Application
    ↓ (Web Serial API)
WebAssembly Hamlib Module
    ↓ (JavaScript callbacks)
Real Radio Backend (FT-991A, IC-7300, etc.)
```

### Key Components

- **WASM Backend** (`wasm/wasm_backend.c`): Intercepts Hamlib's I/O operations and routes them to JavaScript callbacks
- **WASM Bindings** (`wasm/wasm_bindings.c`): Emscripten interface that exports Hamlib functions to JavaScript
- **Build System** (`wasm/Makefile`): Emscripten-based build configuration
- **Demo Application** (`wasm/example.html`): Example HTML page showing usage

## Features

### Supported Operations
- ✅ Radio initialization for any Hamlib-supported model
- ✅ Frequency control (get/set)
- ✅ Mode control (get/set)  
- ✅ PTT (Push-to-Talk) control
- ✅ Serial port configuration
- ✅ JavaScript I/O callback registration

### Supported Radios
Currently includes popular models:
- Yaesu FT-991A
- Icom IC-7300  
- Kenwood TS-590S
- (Any other Hamlib-supported radio can be added)

## Building

### Prerequisites
- [Docker](https://www.docker.com/) installed
- Docker Compose (included with Docker Desktop)

### Docker Build (Recommended)

The easiest way to build is using Docker, which provides a consistent build environment across all platforms:

```bash
# Build the WebAssembly module
docker-compose build hamlib-wasm-build
docker-compose run --rm hamlib-wasm-build

# Or combine both steps
docker-compose up hamlib-wasm-build
```

This produces:
- `wasm/dist/hamlib.js` - JavaScript glue code
- `wasm/dist/hamlib.wasm` - WebAssembly module

### Testing the Build

Serve the example application:
```bash
# Start web server on http://localhost:8080
docker-compose up hamlib-wasm-serve
```

Then open http://localhost:8080/example.html in your browser.

### Development Mode

For interactive development and debugging:
```bash
# Start development container with shell access
docker-compose run --rm hamlib-wasm-dev
```

### Local Build (Alternative)

If you prefer to install tools locally:

#### Prerequisites
- [Emscripten](https://emscripten.org/) toolchain installed
- Make

#### Build Steps
```bash
cd wasm
make all
```

## Usage

### Basic Setup

```javascript
// Load the WebAssembly module
const HamlibWasm = await import('./dist/hamlib.js');

// Set up I/O callbacks for Web Serial API
HamlibWasm.ccall('hamlib_set_callbacks', null, ['number', 'number', 'number', 'number'],
    [openCallback, closeCallback, writeCallback, readCallback]);

// Initialize radio (e.g., Yaesu FT-991A)
const rig = HamlibWasm.ccall('wasm_rig_init', 'number', ['number'], [2020]);

// Configure serial port
HamlibWasm.ccall('wasm_rig_set_conf', null, ['number', 'string', 'number'],
    [rig, '/dev/ttyUSB0', 38400]);

// Open connection
HamlibWasm.ccall('wasm_rig_open', 'number', ['number'], [rig]);

// Set frequency to 14.200 MHz
HamlibWasm.ccall('wasm_rig_set_freq', 'number', ['number', 'number', 'number'],
    [rig, 1, 14200000]); // VFO A, 14.200 MHz
```

### I/O Callbacks

Implement these JavaScript functions to handle serial communication:

```javascript
function openCallback(pathname, baudRate) {
    // Open serial port using Web Serial API
    return 0; // Success
}

function closeCallback() {
    // Close serial port
    return 0;
}

function writeCallback(bufferPtr, count) {
    // Write data to serial port
    const data = new Uint8Array(HamlibWasm.HEAPU8.buffer, bufferPtr, count);
    // Send 'data' via Web Serial API
    return count;
}

function readCallback(bufferPtr, maxCount) {
    // Read data from serial port and copy to WASM memory
    // Return number of bytes read
    return bytesRead;
}
```

## Example Application

Run the demo:

1. Build the WebAssembly module
2. Serve `wasm/example.html` from a web server (required for WASM loading)
3. Open in a browser that supports Web Serial API (Chrome/Edge)
4. Click "Request Serial Port" and select your radio's serial interface
5. Choose your radio model and connect

## Project Structure

```
hamlibdotwasm/
├── hamlib/                 # Hamlib source (git subtree)
├── wasm/                   # WebAssembly port code
│   ├── wasm_backend.c      # I/O interception backend
│   ├── wasm_backend.h      # Header file
│   ├── wasm_bindings.c     # Emscripten JavaScript bindings  
│   ├── Makefile            # Build system
│   └── example.html        # Demo application
├── README.md               # This file
└── .gitignore              # Git ignore rules
```

## Technical Details

### I/O Interception Method

The WebAssembly port uses **link-time function replacement** to intercept Hamlib's I/O operations:

1. Hamlib backends call standard I/O functions (`write_block`, `read_block`)
2. These functions call internal `port_write`/`port_read_generic` functions
3. Our WASM backend provides replacement implementations that route to JavaScript callbacks
4. JavaScript implements actual I/O using Web Serial API

This approach is clean and requires minimal changes to Hamlib core.

### Browser Compatibility

- **Chrome/Chromium 89+**: Full Web Serial API support
- **Edge 89+**: Full support  
- **Firefox**: Web Serial API behind flag (experimental)
- **Safari**: No Web Serial API support yet

### Limitations

- **Synchronous I/O**: Current implementation assumes synchronous read/write operations
- **Error Handling**: Basic error handling - could be enhanced
- **Limited Backends**: Currently includes only popular radio backends
- **Browser Only**: Designed specifically for browser environment

## Contributing

This project maintains compatibility with upstream Hamlib via git subtree:

```bash
# Update Hamlib source to latest version
git subtree pull --prefix=hamlib git@github.com:hardboiledgeek/Hamlib.git 4.6.4 --squash
```

## License

This project inherits Hamlib's GPL-2.1+ license since it's derived from Hamlib source code. The WebAssembly-specific code (`wasm/` directory) is also GPL-2.1+.

## Future Enhancements

- **TCP/Network Radios**: Support radios with Ethernet interfaces via WebSocket proxy
- **Asynchronous I/O**: Better handling of async operations
- **More Backends**: Add support for more radio models
- **TypeScript Definitions**: Provide .d.ts files for better JavaScript integration
- **NPM Package**: Package as installable npm module
- **Digital Modes**: Integrate digital signal processing for modes like FT8