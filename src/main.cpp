#include "emu.hpp"

#include <fstream>
#include <iostream>

int main() {
  Chip8 chip8;

  std::streamsize romSize = chip8.LoadROM("../roms/test_opcode.ch8");

  std::cout << "Successfully read file\n";
  if (romSize == 0) {
    std::cout << "error while reading file";
    return 1;
  }

  for (int i = 0; i < 200; i++) {
    // std::cout
    //   <<std::hex
    //   <<std::setw(2)
    //   <<std::setfill('0')
    //   <<static_cast<int>(chip8.getMemory()[i])
    //   <<'\n';
    chip8.cycle();
  }

  chip8.printFramebuffer();

  return 0;
}
