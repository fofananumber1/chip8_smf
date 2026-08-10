#include "emu.hpp"
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <random>
#include <stdexcept>

constexpr std::size_t FONTSET_SIZE = 80;

constexpr uint8_t fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

constexpr uint16_t FONT_START = 0x50;

/* Constructor. Simpley sets the program counter to the start address, 0x200. */
Chip8::Chip8() {
  reset();
}

/* Function to load a ROM file. Works by opening the specified file as a binary stream
and filling a buffer to load the ROM's contents into memory. */

std::streamsize Chip8::LoadROM(char const* filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);

  if (file.is_open()) {
    std::streampos size = file.tellg();
    if (size > MAX_ROM_SIZE) {
      throw std::runtime_error("ROM is too large");
    }
    char* buffer = new char[size];

    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();

    for (long i = 0; i < size; i++) {
      memory[START_ADDRESS + i] = buffer[i];
    }

    delete[] buffer;
    return size;
  }

  return 0;
}

/* Reset the CHIP-8 state */
void Chip8::reset() {
  std::memset(registers, 0, sizeof(registers));
  std::memset(memory, 0, sizeof(memory));

  for (std::size_t i = 0; i < FONTSET_SIZE; i++) {
    memory[FONT_START + i] = fontset[i];
  }
  
  index = 0;
  pc = START_ADDRESS; // reset program counter back to start address (0x200)
  std::memset(stack, 0, sizeof(stack));
  sp = 0;
  delayTimer = 0;
  soundTimer = 0;
  std::memset(keypad, 0, sizeof(keypad));
  std::memset(video, 0, sizeof(video));
  opcode = 0;
};

/* Fetch/decode/execute loop */
void Chip8::cycle() {
  // CHIP-8 opcodes contain two bytes each
  uint8_t byte_0 = memory[pc];
  uint8_t byte_1 = memory[pc + 1];

  opcode = (byte_0 << 8) | byte_1;
  pc += 2;

  decode_and_exec(opcode);
}

void Chip8::updateTimers() {
  if (delayTimer > 0) {
    delayTimer--;
  }

  if (soundTimer > 0) {
    soundTimer--;
  }
}

/* Testing function for display */
void Chip8::setPixel(uint32_t x, uint32_t y) {
  if (x >= 64 || y >= 32) {
    return;
  }

  video[y * 64 + x] = 1;
}

/* Testing function for instructions */
void Chip8::loadInstruction(uint16_t ins) {
  memory[pc] = 
    static_cast<uint8_t>((ins>> 8) & 0xFF);
  
  memory[pc + 1] =
    static_cast<uint8_t>(ins & 0xFF);
}

uint32_t* Chip8::getVideo() {
  return video;
}

uint8_t* Chip8::getMemory() {
  return memory;
}

uint8_t Chip8::getRegister(int index) {
  return registers[index];
}

uint16_t Chip8::getPC() {
  return pc;
}

void Chip8::setPC(uint16_t address) {
  pc = address;
}

uint16_t Chip8::getIndexRegister() {
  return index;
}

void Chip8::setRegister(int n, int v) {
  registers[n] = v;
}

void Chip8::setIndexRegister(int i) {
  index = i;
}

uint8_t Chip8::getDelayTimer() {
  return delayTimer;
}

uint8_t Chip8::getSoundTimer() {
  return soundTimer;
}

void Chip8::setDelayTimer(uint8_t value) {
  delayTimer = value;
}

void Chip8::setSoundTimer(uint8_t value) {
  soundTimer = value;
}

void Chip8::setKey(uint8_t key, uint16_t value) {
  keypad[key] = value;
}

void Chip8::printFramebuffer() {

  for (uint32_t y = 0; y < 32; y++) {
    for (uint32_t x = 0; x < 64; x++) {
      const uint32_t pixel = video[y * 64 + x];

      std::cout << (pixel != 0 ? '#' : '.');
    }

    std::cout << '\n';
  }
}

bool Chip8::loadROMFromBuffer(const uint8_t* data, std::size_t size) {
  if (size > MAX_ROM_SIZE) {
    return false;
  }

  for (std::size_t i = 0; i < size; i++) {
    memory[START_ADDRESS + i] = data[i];
  }

  return true;
}

void Chip8::decode_and_exec(uint16_t opcode) {
  uint8_t x = (opcode >> 8) & 0x0F; // one of the 16 registers V0 through VF, an INDEX
  uint8_t y = (opcode >> 4) & 0x0F; // also one of the 16 registers V0 through VF, an INDEX
  uint8_t n = opcode & 0x0F; // 4th nibble, a 4-bit number
  uint8_t nn = opcode & 0xFF; // second byte, 8-bit immediate number
  uint16_t nnn = opcode & 0xFFF; // 2nd + 3rd + 4th nibbles, 12-bit immediate address

  switch (opcode & 0xF000) {
    case 0x0000:
      switch (opcode & 0x00FF) {
        case 0x00E0: // 0x00E0
          std::memset(video, 0, sizeof(video));
          break;

        case 0x00EE: // 0x00EE
          if (sp == 0) {
            throw std::runtime_error("Stack underflow");
          }

          sp--;
          pc = stack[sp];
          break;
      }
      break;

    case 0x1000: // 1NNN
      pc = nnn;
      break;

    case 0x2000: // 2NNN
      stack[sp] = pc;
      sp++;
      pc = nnn;
      break;

    case 0x3000: // 3XNN
      // 3XNN: skip one instruction if VX == NN
      if (registers[x] == nn) {
        pc += 2;
      }
      break;

    case 0x4000: // 4XNN
      // 4XNN: skip one instruction if VX != NN
      if (registers[x] != nn) {
        pc += 2;
      }
      break;
    
    case 0x5000: // 5XY0
      // 5XY0: skip if VX == VY
      if (registers[x] == registers[y]) {
        pc += 2;
      }
      break;

    case 0x6000: // 6XNN
      registers[x] = nn;   
      break;

    case 0x7000: // 7XNN
      registers[x] = registers[x] + nn;
      break;
    
    case 0x8000: // 0x8XY# instructions
      switch (opcode & 0x000F) { 
        case 0x0000: // 8XY0
          registers[x] = registers[y];
          break;

        case 0x0001: // 8XY1
          registers[x] = registers[x] | registers[y];
          break;
        
        case 0x0002: // 8XY2
          registers[x] = registers[x] & registers[y];
          break;

        case 0x0003: // 8XY3
          registers[x] = registers[x] ^ registers[y];
          break;

        case 0x0004: { // 8XY4
          uint16_t sum = registers[x] + registers[y];
          if (sum > 255) {
            registers[0xF] = 1;
          }
          else {
            registers[0xF] = 0;
          }
          registers[x] = sum & 0xFF;
          break;
        }

        case 0x0005: // 8XY5
          registers[0xF] = (registers[x] >= registers[y]) ? 1 : 0;
          registers[x] = registers[x] - registers[y];
          break;

        case 0x0006: // 8XY6 ** AMBIGUOUS
          registers[x] = registers[y];
          registers[0xF] = registers[x] & 0x1;
          registers[x] = (registers[x] >> 1);
          break;

        case 0x0007: // 8XY7
          registers[0xF] = (registers[y] >= registers[x]) ? 1 : 0;
          registers[x] = registers[y] - registers[x];
          break;

        case 0x000E: // 8XYE ** AMBIGUOUS
          registers[x] = registers[y];
          registers[0xF] = (registers[x] >> 7) & 0x1;
          registers[x] = (registers[x] << 1);
          break;
        
      }
      break;

    case 0x9000: // 9XY0
      if (registers[x] != registers[y]) {
        pc += 2;
      }
      break;

    case 0xA000: // A000
      index = nnn;
      break;

    case 0xB000: // BNNN ** AMBIGUOUS
      pc = nnn + registers[0];
      break;
    
    case 0xC000: { // CXNN
      int random = std::rand() % 256;
      registers[x] = random & nn;
      break;
    }

    case 0xD000: { // DXYN
      uint8_t regX = registers[x] & 63;
      uint8_t regY = registers[y] & 31;

      registers[0xF] = 0;

      for (uint8_t row = 0; row < n; row++) {
        uint32_t screenY = regY + row;

        if (screenY >= 32) {
          break;
        }

        uint8_t spriteData = memory[index + row]; // 8-bit row of the sprite

        for (uint8_t column = 0; column < 8; column++) {
          uint32_t screenX = regX + column;

          if (screenX >= 64) {
            break;
          }

          uint8_t mask = 0x80 >> column;

          if ((spriteData & mask) != 0) {
            uint32_t& pixel = video[screenY * 64 + screenX];

            if (pixel != 0) {
              registers[0xF] = 1;
            }

            pixel ^= 1;
          }
        }
      }
      break;
    }
      
    case 0xE000:
      switch (opcode & 0x00FF) {
        case 0x009E: // EX9E
          if (keypad[registers[x]] != 0) {
            pc += 2;
          }
          break;
        
        case 0x00A1: // EXA1
          if (keypad[registers[x]] == 0) {
            pc += 2;
          }
          break;

      }
      break;
    
    case 0xF000:
      switch (opcode & 0x00FF) {
        case 0x0007: // FX07
          registers[x] = delayTimer;
          break;

        case 0x0015: // FX15
          delayTimer = registers[x];
          break;

        case 0x0018: // FX18
          soundTimer = registers[x];
          break;

        case 0x001E: { // FX1E
          uint16_t sum = index + registers[x];

          registers[0xF] = (sum > 0x0FFF) ? 1 : 0;
          index = sum;
          break;
        }
        case 0x000A: { // FX0A
          bool keyPressed = false;

          for (uint8_t key = 0; key < 16; key++) {
            if (keypad[key] != 0) {
              registers[x] = key;
              keyPressed = true;
              break;
            }
          }

          if (!keyPressed) {
            pc -= 2;
          }
          break;
        }
        case 0x0029: // FX29
          index = FONT_START + (registers[x] * 5);
          break;

        case 0x0033: { // FX33 
          int v = registers[x];
          int a = v / 100;
          int b = (v / 10) % 10;
          int c = v % 10;
          memory[index] = a;
          memory[index + 1] = b;
          memory[index + 2] = c;
          break;
        }

        case 0x0055: { // FX55 ** AMBIGUOUS
          for (uint8_t reg = 0; reg <= x; reg++) {
            memory[index + reg] = registers[reg];
          }
          break;
        }

        case 0x0065: { // FX65 ** AMBIGUOUS
          for (uint8_t reg = 0; reg <= x; reg++) {
            registers[reg] = memory[index + reg];
          }
          break;
        }
      }
      break;
  }
}
