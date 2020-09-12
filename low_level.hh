#include <fstream>
#include <random>

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
  std::default_random_engine randGen;
  std::uniform_int_distribution<unsigned char> rand_byte;
  
  chip_8();
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
  
  
};
