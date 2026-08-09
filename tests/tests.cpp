#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "../src/emu.hpp"

namespace {

void execute(Chip8& chip8, uint16_t instruction) {
  chip8.loadInstruction(instruction);
  chip8.cycle();
}

void testClearScreen00E0() {
  Chip8 chip8;

  chip8.setPixel(0, 1);
  chip8.setPixel(10, 3);

  execute(chip8, 0x00E0);

  const uint32_t* video = chip8.getVideo();
  assert(std::all_of(video, video + 2048, [](uint32_t pixel) {
    return pixel == 0;
  }));
}

void testReturn00EE() {
  Chip8 chip8;

  execute(chip8, 0x2300);
  assert(chip8.getPC() == 0x300);

  execute(chip8, 0x00EE);
  assert(chip8.getPC() == 0x202);
}

void testJump1NNN() {
  Chip8 chip8;

  execute(chip8, 0x1234);

  assert(chip8.getPC() == 0x234);
}

void testCall2NNN() {
  Chip8 chip8;

  execute(chip8, 0x2456);

  assert(chip8.getPC() == 0x456);
}

void testSkipIfEqual3XNN() {
  Chip8 chip8;

  chip8.setRegister(1, 0xAB);
  execute(chip8, 0x31AB);
  assert(chip8.getPC() == 0x204);

  chip8.reset();
  chip8.setRegister(1, 0xAC);
  execute(chip8, 0x31AB);
  assert(chip8.getPC() == 0x202);
}

void testSkipIfNotEqual4XNN() {
  Chip8 chip8;

  chip8.setRegister(2, 0xAB);
  execute(chip8, 0x42AC);
  assert(chip8.getPC() == 0x204);

  chip8.reset();
  chip8.setRegister(2, 0xAC);
  execute(chip8, 0x42AC);
  assert(chip8.getPC() == 0x202);
}

void testSkipIfRegistersEqual5XY0() {
  Chip8 chip8;

  chip8.setRegister(1, 0x22);
  chip8.setRegister(2, 0x22);
  execute(chip8, 0x5120);
  assert(chip8.getPC() == 0x204);

  chip8.reset();
  chip8.setRegister(1, 0x22);
  chip8.setRegister(2, 0x33);
  execute(chip8, 0x5120);
  assert(chip8.getPC() == 0x202);
}

void testSetRegister6XNN() {
  Chip8 chip8;

  execute(chip8, 0x63FE);

  assert(chip8.getRegister(3) == 0xFE);
}

void testAddImmediate7XNN() {
  Chip8 chip8;

  chip8.setRegister(4, 0x10);
  execute(chip8, 0x7405);

  assert(chip8.getRegister(4) == 0x15);
}

void testRegisterOps8XYN() {
  Chip8 chip8;

  chip8.setRegister(1, 0x12);
  chip8.setRegister(2, 0x34);
  execute(chip8, 0x8120);
  assert(chip8.getRegister(1) == 0x34);

  chip8.reset();
  chip8.setRegister(1, 0xF0);
  chip8.setRegister(2, 0x0F);
  execute(chip8, 0x8121);
  assert(chip8.getRegister(1) == 0xFF);

  chip8.reset();
  chip8.setRegister(1, 0xF3);
  chip8.setRegister(2, 0x0F);
  execute(chip8, 0x8122);
  assert(chip8.getRegister(1) == 0x03);

  chip8.reset();
  chip8.setRegister(1, 0xF0);
  chip8.setRegister(2, 0xFF);
  execute(chip8, 0x8123);
  assert(chip8.getRegister(1) == 0x0F);

  chip8.reset();
  chip8.setRegister(1, 200);
  chip8.setRegister(2, 100);
  execute(chip8, 0x8124);
  assert(chip8.getRegister(1) == 44);
  assert(chip8.getRegister(0xF) == 1);

  chip8.reset();
  chip8.setRegister(1, 20);
  chip8.setRegister(2, 10);
  execute(chip8, 0x8124);
  assert(chip8.getRegister(1) == 30);
  assert(chip8.getRegister(0xF) == 0);

  chip8.reset();
  chip8.setRegister(1, 20);
  chip8.setRegister(2, 10);
  execute(chip8, 0x8125);
  assert(chip8.getRegister(1) == 10);
  assert(chip8.getRegister(0xF) == 1);

  chip8.reset();
  chip8.setRegister(1, 10);
  chip8.setRegister(2, 20);
  execute(chip8, 0x8127);
  assert(chip8.getRegister(1) == 10);
  assert(chip8.getRegister(0xF) == 1);

  chip8.reset();
  chip8.setRegister(2, 0x05);
  execute(chip8, 0x8126);
  assert(chip8.getRegister(1) == 0x02);
  assert(chip8.getRegister(0xF) == 1);

  chip8.reset();
  chip8.setRegister(2, 0x81);
  execute(chip8, 0x812E);
  assert(chip8.getRegister(1) == 0x02);
  assert(chip8.getRegister(0xF) == 1);
}

void testSkipIfRegistersNotEqual9XY0() {
  Chip8 chip8;

  chip8.setRegister(1, 0x22);
  chip8.setRegister(2, 0x33);
  execute(chip8, 0x9120);
  assert(chip8.getPC() == 0x204);

  chip8.reset();
  chip8.setRegister(1, 0x22);
  chip8.setRegister(2, 0x22);
  execute(chip8, 0x9120);
  assert(chip8.getPC() == 0x202);
}

void testSetIndexANNN() {
  Chip8 chip8;

  execute(chip8, 0xA345);

  assert(chip8.getIndexRegister() == 0x345);
}

void testJumpWithV0BNNN() {
  Chip8 chip8;

  chip8.setRegister(0, 0x10);
  execute(chip8, 0xB300);

  assert(chip8.getPC() == 0x310);
}

void testRandomMaskedCXNN() {
  Chip8 chip8;

  std::srand(1);
  execute(chip8, 0xC10F);

  assert((chip8.getRegister(1) & 0xF0) == 0);
}

void testDrawDXYN() {
  Chip8 chip8;
  uint8_t* memory = chip8.getMemory();
  const uint32_t* video = chip8.getVideo();

  chip8.setIndexRegister(0x300);
  memory[0x300] = 0x80;
  chip8.setRegister(1, 2);
  chip8.setRegister(2, 3);

  execute(chip8, 0xD121);
  assert(video[(3 * 64) + 2] == 1);
  assert(chip8.getRegister(0xF) == 0);

  chip8.setPC(0x200);
  execute(chip8, 0xD121);
  assert(video[(3 * 64) + 2] == 0);
  assert(chip8.getRegister(0xF) == 1);
}

void testKeySkipsEX9EAndEXA1() {
  Chip8 chip8;

  chip8.setRegister(1, 0xA);
  chip8.setKey(0xA, 1);
  execute(chip8, 0xE19E);
  assert(chip8.getPC() == 0x204);

  chip8.reset();
  chip8.setRegister(1, 0xA);
  chip8.setKey(0xA, 0);
  execute(chip8, 0xE1A1);
  assert(chip8.getPC() == 0x204);
}

void testTimersFX07FX15FX18() {
  Chip8 chip8;

  chip8.setDelayTimer(42);
  execute(chip8, 0xF107);
  assert(chip8.getRegister(1) == 42);

  chip8.reset();
  chip8.setRegister(1, 12);
  execute(chip8, 0xF115);
  assert(chip8.getDelayTimer() == 12);

  chip8.reset();
  chip8.setRegister(1, 9);
  execute(chip8, 0xF118);
  assert(chip8.getSoundTimer() == 9);
}

void testAddIndexFX1E() {
  Chip8 chip8;

  chip8.setIndexRegister(0x200);
  chip8.setRegister(1, 0x10);
  execute(chip8, 0xF11E);
  assert(chip8.getIndexRegister() == 0x210);
  assert(chip8.getRegister(0xF) == 0);

  chip8.reset();
  chip8.setIndexRegister(0xFFF);
  chip8.setRegister(1, 1);
  execute(chip8, 0xF11E);
  assert(chip8.getIndexRegister() == 0x1000);
  assert(chip8.getRegister(0xF) == 1);
}

void testWaitForKeyFX0A() {
  Chip8 chip8;

  execute(chip8, 0xF10A);
  assert(chip8.getPC() == 0x200);

  chip8.setKey(5, 1);
  execute(chip8, 0xF10A);
  assert(chip8.getRegister(1) == 5);
  assert(chip8.getPC() == 0x202);
}

void testFontAddressFX29() {
  Chip8 chip8;

  chip8.setRegister(1, 0xA);
  execute(chip8, 0xF129);

  assert(chip8.getIndexRegister() == 0x50 + (0xA * 5));
}

void testBcdFX33() {
  Chip8 chip8;
  uint8_t* memory = chip8.getMemory();

  chip8.setIndexRegister(0x300);
  chip8.setRegister(1, 234);
  execute(chip8, 0xF133);

  assert(memory[0x300] == 2);
  assert(memory[0x301] == 3);
  assert(memory[0x302] == 4);
}

void testStoreAndLoadRegistersFX55FX65() {
  Chip8 chip8;
  uint8_t* memory = chip8.getMemory();

  chip8.setIndexRegister(0x300);
  chip8.setRegister(0, 0x11);
  chip8.setRegister(1, 0x22);
  chip8.setRegister(2, 0x33);
  execute(chip8, 0xF255);

  assert(memory[0x300] == 0x11);
  assert(memory[0x301] == 0x22);
  assert(memory[0x302] == 0x33);

  chip8.reset();
  memory = chip8.getMemory();
  chip8.setIndexRegister(0x300);
  memory[0x300] = 0x44;
  memory[0x301] = 0x55;
  memory[0x302] = 0x66;
  execute(chip8, 0xF265);

  assert(chip8.getRegister(0) == 0x44);
  assert(chip8.getRegister(1) == 0x55);
  assert(chip8.getRegister(2) == 0x66);
}

} // namespace

int main() {
  testClearScreen00E0();
  testReturn00EE();
  testJump1NNN();
  testCall2NNN();
  testSkipIfEqual3XNN();
  testSkipIfNotEqual4XNN();
  testSkipIfRegistersEqual5XY0();
  testSetRegister6XNN();
  testAddImmediate7XNN();
  testRegisterOps8XYN();
  testSkipIfRegistersNotEqual9XY0();
  testSetIndexANNN();
  testJumpWithV0BNNN();
  testRandomMaskedCXNN();
  testDrawDXYN();
  testKeySkipsEX9EAndEXA1();
  testTimersFX07FX15FX18();
  testAddIndexFX1E();
  testWaitForKeyFX0A();
  testFontAddressFX29();
  testBcdFX33();
  testStoreAndLoadRegistersFX55FX65();

  std::cout << "All tests passed.\n";
  return 0;
}
