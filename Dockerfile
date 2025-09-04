# Hamlib WebAssembly Build Environment
# Based on official Emscripten Docker image
FROM emscripten/emsdk:3.1.48

# Set working directory
WORKDIR /src

# Install additional dependencies for Hamlib build
RUN apt-get update && apt-get install -y \
    autoconf \
    automake \
    libtool \
    pkg-config \
    make \
    git \
    && rm -rf /var/lib/apt/lists/*

# Copy project files
COPY . .

# Set up build environment
ENV CC=emcc
ENV CXX=em++
ENV AR=emar
ENV RANLIB=emranlib

# Create build directories
RUN mkdir -p wasm/build wasm/dist

# Default command
CMD ["make", "-C", "wasm", "all"]