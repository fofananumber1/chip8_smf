/* The API for the CHIP-8. An instance of CHIP-8 contains */

#include <cstdint>
#include <cstddef>
#include <fstream>
#include <iostream>

constexpr std::size_t MEMORY_SIZE = 4096;
constexpr std::size_t ROM_START = 0x200;
constexpr std::size_t MAX_ROM_SIZE = MEMORY_SIZE - ROM_START;

// See README.md for explanations on CHIP-8 components
class Chip8
{
  public:
    const unsigned int START_ADDRESS = 0x200;

    Chip8();
    std::streamsize LoadROM(char const* filename);
    void reset();
    void cycle();
    void updateTimers();
    void setPixel(uint32_t x, uint32_t y);
    void loadInstruction(uint16_t ins);
    uint32_t* getVideo();
    uint8_t* getMemory();
    uint8_t getRegister(int index);
    uint16_t getPC();
    void setPC(uint16_t address);
    uint16_t getIndexRegister();
    void setRegister(int n, int v);
    void setIndexRegister(int i);
    uint8_t getDelayTimer();
    uint8_t getSoundTimer();
    void setDelayTimer(uint8_t value);
    void setSoundTimer(uint8_t value);
    void setKey(uint8_t key, uint16_t value);
    void printFramebuffer();
    bool loadROMFromBuffer(const uint8_t* data, std::size_t size);
    uint16_t getOpcode();
  private:
    uint8_t registers[16]{}; // 16 8-bit registers
    uint8_t memory[4096]{}; // 4KB memory
    uint16_t index{}; // index register
    uint16_t pc{}; // program counter
    uint16_t stack[16]{}; // stack
    uint8_t sp{}; // stack pointer
    uint8_t delayTimer{}; // delay timer
    uint8_t soundTimer{}; // sound timer
    uint16_t keypad[16]{};
    uint32_t video[64 * 32]{};
    uint16_t opcode;
    void decode_and_exec(uint16_t opcode);
};
