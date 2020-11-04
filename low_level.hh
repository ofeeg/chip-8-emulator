#ifndef LOW_LEVEL_H
#define LOW_LEVEL_H
#include <fstream>
#include <random>


const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

class chip_8{
public:
  unsigned short opcode{ /*example $7522 is 7(ADD) $(HEX)22(VAL) to 5(REG) */
		      
  }; 
  unsigned short I{};
  unsigned short pc{};
  unsigned short stack[16]{};
  unsigned char memory[4096]{};
  unsigned char sp{};
  unsigned char delay_timer{};
  unsigned char sound_timer{};
  unsigned char keypad[16]{};
  unsigned char registers[16]{};
  unsigned int video[64*32]{};
  void OP_NULL(){};
  typedef void (chip_8::*chip_8_func)();
  chip_8_func table[0xF + 1]{&chip_8::OP_NULL};
  chip_8_func table0[0xE + 1]{&chip_8::OP_NULL};
  void table_0(){((*this).*(table0[opcode & 0x000Fu]))();}
  chip_8_func table8[0xE + 1]{&chip_8::OP_NULL};
  void table_8(){((*this).*(table8[opcode & 0x000Fu]))();}
  chip_8_func tableE[0xE + 1]{&chip_8::OP_NULL};
  void table_E(){((*this).*(tableE[opcode & 0x000Fu]))();}
  chip_8_func tableF[0x65 + 1]{&chip_8::OP_NULL};
  void table_F(){((*this).*(tableF[opcode & 0x00FFu]))();}
  std::default_random_engine randGen;
  std::uniform_int_distribution<unsigned char> rand_byte;

  
  chip_8();
  void cycle();
  void load_rom(char const* filename);
  void OP_00E0();
  void OP_00EE();
  void OP_1nnn();
  void OP_2nnn();
  void OP_3xkk();
  void OP_4xkk();
  void OP_5xy0();
  void OP_6xkk();
  void OP_7xkk();
  void OP_8xy0();
  void OP_8xy1();
  void OP_8xy2();
  void OP_8xy3();
  void OP_8xy4();
  void OP_8xy5();
  void OP_8xy6();
  void OP_8xy7();
  void OP_8xyE();
  void OP_9xy0();
  void OP_Annn();
  void OP_Bnnn();
  void OP_Cxkk();
  void OP_Dxyn();
  void OP_Ex9E();
  void OP_ExA1();
  void OP_Fx07();
  void OP_Fx0A();
  void OP_Fx15();
  void OP_Fx18();
  void OP_Fx1E();
  void OP_Fx29();
  void OP_Fx33();
  void OP_Fx55();
  void OP_Fx65();

};
#endif
