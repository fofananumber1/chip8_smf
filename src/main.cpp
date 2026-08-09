#include "emu.hpp"

#include <fstream>
#include <iostream>

#define CYCLES_PER_FRAME 10

int main() {

  bool running = true;

  Chip8 chip8;

  std::streamsize romSize = chip8.LoadROM("../roms/test_opcode.ch8");

  std::cout << "Successfully read file\n";
  if (romSize == 0) {
    std::cout << "error while reading file";
    return 1;
  }

  while (running) {
    for (int i = 0; i < CYCLES_PER_FRAME; i++) {
      // std::cout
      //   <<std::hex
      //   <<std::setw(2)
      //   <<std::setfill('0')
      //   <<static_cast<int>(chip8.getMemory()[i])
      //   <<'\n';
      chip8.cycle();

      chip8.updateTimers();
    }
  }

  //chip8.printFramebuffer();

  return 0;
}
