# Hamlib WebAssembly Build System
# Copyright (c) 2024 by hardboiledgeek
#
# This Makefile builds Hamlib as a WebAssembly module with JavaScript I/O callbacks

# Configuration
HAMLIB_DIR = ../hamlib
WASM_DIR = .
BUILD_DIR = build
DIST_DIR = dist

# Emscripten toolchain
CC = emcc
AR = emar
RANLIB = emranlib

# Emscripten flags for WASM build
EMCC_FLAGS = \
	-O2 \
	-s WASM=1 \
	-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap','getValue','setValue','UTF8ToString','stringToUTF8'] \
	-s EXPORTED_FUNCTIONS=['_malloc','_free'] \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s MODULARIZE=1 \
	-s EXPORT_NAME='HamlibWasm' \
	-s USE_PTHREADS=0 \
	-s ENVIRONMENT='web,webview,worker' \
	-s SINGLE_FILE=0

# Include paths
INCLUDES = \
	-I$(HAMLIB_DIR)/include \
	-I$(HAMLIB_DIR)/src \
	-I$(WASM_DIR)

# Preprocessor defines for WebAssembly build
DEFINES = \
	-DHAVE_CONFIG_H \
	-DWASM_BUILD \
	-D_POSIX_C_SOURCE=200809L \
	-D_GNU_SOURCE

# Source files
WASM_SOURCES = \
	wasm_backend.c \
	wasm_bindings.c

# Hamlib core sources (we'll add these as needed)
HAMLIB_CORE_SOURCES = \
	$(HAMLIB_DIR)/src/rig.c \
	$(HAMLIB_DIR)/src/serial.c \
	$(HAMLIB_DIR)/src/misc.c \
	$(HAMLIB_DIR)/src/register.c \
	$(HAMLIB_DIR)/src/event.c \
	$(HAMLIB_DIR)/src/cal.c \
	$(HAMLIB_DIR)/src/conf.c \
	$(HAMLIB_DIR)/src/tones.c \
	$(HAMLIB_DIR)/src/rotator.c \
	$(HAMLIB_DIR)/src/locator.c \
	$(HAMLIB_DIR)/src/split.c \
	$(HAMLIB_DIR)/src/settings.c \
	$(HAMLIB_DIR)/src/network.c

# We'll exclude iofunc.c since we're providing our own I/O functions
# $(HAMLIB_DIR)/src/iofunc.c

# Essential radio backends (start with a few popular ones)
HAMLIB_BACKENDS = \
	$(HAMLIB_DIR)/rigs/yaesu/yaesu.c \
	$(HAMLIB_DIR)/rigs/yaesu/newcat.c \
	$(HAMLIB_DIR)/rigs/yaesu/ft991.c \
	$(HAMLIB_DIR)/rigs/icom/icom.c \
	$(HAMLIB_DIR)/rigs/icom/ic7300.c \
	$(HAMLIB_DIR)/rigs/icom/frame.c \
	$(HAMLIB_DIR)/rigs/kenwood/kenwood.c \
	$(HAMLIB_DIR)/rigs/kenwood/ts590.c \
	$(HAMLIB_DIR)/rigs/dummy/dummy.c

# All sources
ALL_SOURCES = $(WASM_SOURCES) $(HAMLIB_CORE_SOURCES) $(HAMLIB_BACKENDS)

# Object files
OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(ALL_SOURCES)))

# Targets
all: $(DIST_DIR)/hamlib.js $(DIST_DIR)/hamlib.wasm

# Create directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(DIST_DIR):
	mkdir -p $(DIST_DIR)

# Compile individual source files
$(BUILD_DIR)/%.o: $(WASM_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(EMCC_FLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(BUILD_DIR)/%.o: $(HAMLIB_DIR)/src/%.c | $(BUILD_DIR)
	$(CC) $(EMCC_FLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

# Backend object files (need special handling for different directories)
$(BUILD_DIR)/yaesu.o: $(HAMLIB_DIR)/rigs/yaesu/yaesu.c | $(BUILD_DIR)
	$(CC) $(EMCC_FLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(BUILD_DIR)/newcat.o: $(HAMLIB_DIR)/rigs/yaesu/newcat.c | $(BUILD_DIR)
	$(CC) $(EMCC_FLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(BUILD_DIR)/ft991.o: $(HAMLIB_DIR)/rigs/yaesu/ft991.c | $(BUILD_DIR)
	$(CC) $(EMCC_FLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(BUILD_DIR)/icom.o: $(HAMLIB_DIR)/rigs/icom/icom.c | $(BUILD_DIR)
	$(CC) $(EMCC_FLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(BUILD_DIR)/ic7300.o: $(HAMLIB_DIR)/rigs/icom/ic7300.c | $(BUILD_DIR)
	$(CC) $(EMCC_FLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(BUILD_DIR)/frame.o: $(HAMLIB_DIR)/rigs/icom/frame.c | $(BUILD_DIR)
	$(CC) $(EMCC_FLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(BUILD_DIR)/kenwood.o: $(HAMLIB_DIR)/rigs/kenwood/kenwood.c | $(BUILD_DIR)
	$(CC) $(EMCC_FLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(BUILD_DIR)/ts590.o: $(HAMLIB_DIR)/rigs/kenwood/ts590.c | $(BUILD_DIR)
	$(CC) $(EMCC_FLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(BUILD_DIR)/dummy.o: $(HAMLIB_DIR)/rigs/dummy/dummy.c | $(BUILD_DIR)
	$(CC) $(EMCC_FLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

# Link WebAssembly module
$(DIST_DIR)/hamlib.js: $(OBJECTS) | $(DIST_DIR)
	$(CC) $(EMCC_FLAGS) $(OBJECTS) -o $@ \
		--pre-js pre.js \
		--post-js post.js

# The .wasm file is automatically generated alongside the .js file
$(DIST_DIR)/hamlib.wasm: $(DIST_DIR)/hamlib.js

# JavaScript wrapper files
pre.js:
	echo '// Hamlib WASM Pre-JavaScript' > pre.js
	echo 'var HamlibWasm = HamlibWasm || {};' >> pre.js

post.js:
	echo '// Hamlib WASM Post-JavaScript' > post.js
	echo 'if (typeof module !== "undefined") module.exports = HamlibWasm;' >> post.js

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR) pre.js post.js

# Development target - build with debug info
debug: EMCC_FLAGS += -g -s ASSERTIONS=1 -s SAFE_HEAP=1
debug: all

# Test target - build a simple test
test: $(DIST_DIR)/hamlib.js
	echo "WASM module built successfully!"

.PHONY: all clean debug test