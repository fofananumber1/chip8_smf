#include "emu.hpp"

constexpr int CYCLES_PER_FRAME = 10;

static Chip8 chip8;

extern "C" {

  void reset() {
    chip8.reset();
  }

  void runFrame() {
    for (int i = 0; i < CYCLES_PER_FRAME; i++) {
      chip8.cycle();
    }

    chip8.updateTimers();
  }

  uint32_t* getVideoBuffer() {
    return chip8.getVideo();
  }

  bool loadROM(const uint8_t* data, int size) {
    if (size <= 0) {
      return false;
    }

    chip8.reset();
    return chip8.loadROMFromBuffer(data, size);
  }

  void setKey(int key, int pressed) {
    if (key < 0 || key > 15) {
      return;
    }

    chip8.setKey(static_cast<uint8_t>(key), pressed ? 1 : 0);
    
  }
}