-- CHIP-8 Emulator --

An emulator built by me to help learn about emulators in depth.

  COMPONENTS:

  - 16 8-bit registers
    registers V0-VF
  - 4KB Memory
  - an index register
  - a program counter
  - a stack
  - a stack pointer
  - a delay timer
  - a sound timer
  - a key pad (array of 16 elements, representing keys)
  - a 64 * 32 panel of pixels, representing the screen
  - opcode


  INSTRUCTIONS:
    This CHIP-8 implementation features 34 instructions, described below:
    
    * NOTE: Instructions are in hexadecimal format, and many of them utilize the 'N', 'X', and 'Y' characters, which are placeholders and not valid hexadecimal. For the purposes of this emulator, these characters refer to different parts of an instruction's opcode:

      X: the second hex digit of the opcode, the x register index, mainly used to select register VX 
      Y: the third hex digit of the opcode, the y register index, mainly used to select register VY
      N: the last hex digit of the opcode
      NN: the last two hex digits of the opcode
      NNN: the last three hex digits of the opcode

    00E0: Clears the screen
    00EE: Return from a subroutine
    1NNN:
    2NNN:
    3XNN:
    4XNN:
    5XY0:
    6XNN:
    7XNN:
    8XY0:
    8XY1:
    8XY2:
    8XY3:
    8XY4:
    8XY5:
    8XY6:
    8XY7:
    8XYE:
    9XY0:
    A000:
    BNNN:
    CXNN:
    DXYN:
    EX9E:
    EXA1:
    FX07:
    FX15:
    FX18:
    FX1E:
    FX0A:
    FX29:
    FX33:
    FX55:
    FX65:
