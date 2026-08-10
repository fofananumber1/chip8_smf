#!/usr/bin/env bash
set -e

mkdir -p frontend/public

em++ src/wasm.cpp src/emu.cpp \
  -std=c++17 \
  -s WASM=1 \
  -s EXPORTED_FUNCTIONS='["_reset","_runFrame","_getVideoBuffer","_loadROM","_malloc","_free","_setKey"]' \
  -s EXPORTED_RUNTIME_METHODS='["HEAPU8","HEAPU32","ccall","cwrap"]' \
  -o frontend/public/chip8.js