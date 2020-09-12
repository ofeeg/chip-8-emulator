#include "low_level.hh"
#include <chrono>
#include <cstring>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x500;
const unsigned int FONTSET_SIZE = 80;

uint8_t fontset[FONTSET_SIZE] =
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

chip_8::chip_8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()){
  pc = START_ADDRESS;
  for(unsigned int i=0; i < FONTSET_SIZE; ++i){
    memory[FONTSET_START_ADDRESS+i] = fontset[i];
  }
  rand_byte = std::uniform_int_distribution<unsigned char>(0,255U);
  
}

void chip_8::load_rom(char const* filename){
  std::ifstream file(filename, std::ios::binary | std::ios::ate); //open file as a stream of  binary and  point at the end
  if(file.is_open()){
    std::streampos size=file.tellg(); //get size of buffer and allocate buffer to hold contents
    char* buffer = new  char[size];
    file.seekg(0, std::ios::beg); //go back to beginning and fill buffer
    file.read(buffer, size);
    file.close();
    for (long i =0; i<size; ++i){ //fill chip-8 memory at start address
      memory[START_ADDRESS + i] = buffer[i];
    }
    delete[] buffer; //free the buffer
  }
}
	
void chip_8::OP_00E0(){ // CLS
  memset(video, 0, sizeof(video));
}

void chip_8::OP_00EE(){ //RET
  --sp;
  pc = stack[sp];
}

void chip_8::OP_1nnn(){ // JP addr
  unsigned short address = opcode & 0xFFFu;
  pc = address;
}

void chip_8::OP_2nnn{ //CALL addr
  unsigned short address = opcode & 0xFFFu;
  stack[sp] = pc;
  ++sp;
  pc = address;
}

void chip_8::OP_3xkk(){ //SE Vx byte
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char byte = opcode & 0x0F00u;
  if(registers[Vx] == byte){
    pc+=2;
  }
}
  
void chip_8::OP_4xkk(){ //SNE Vx byte
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char byte = (opcode & 0x0F00);
  if(registers[Vx] != byte){
    pc+=2;
  }
}

void chip_8::OP_5xy0(){ //SE Vx Vy
  unsigned char Vx = (opcode & 0x0F00) >> 8u;
  unsigned char Vy = (opcode & 0x0F00) >> 4u;

  if(registers[Vx] == registers[Vy]){
    pc+=2;
  }
}

void chip_8::OP_6xkk(){ //LD Vx byte
  unsigned char Vx = (opcode & 0x0F00) >> 8u;
  unsigned char byte = (opcode & 0x0F00);
  registers[Vx] = byte;
}

void chip_8::OP_7xkk(){
  unsigned char Vx = (opcode & 0x0F00) >> 8u;
  unsigned char byte = (opcode & 0x0F00);
  registers[Vx]+=byte;
}

void chip_8::OP_8xy0(){
  unsigned char Vx = (opcode & 0x0F00) >> 8u;
  unsigned char Vy = (opcode & 0x0F00) >> 4u;
  registers[Vx] = registers[Vy];
}

void chip_8::OP_8xy1(){
  unsigned char Vx = (opcode & 0x0F00) >> 8u;
  unsigned char Vy = (opcode & 0x0F00) >> 4u;
  registers[Vx] |= registers[Vy];
}

void chip_8::OP_8xy2(){
  unsigned char Vx = (opcode & 0x0F00) >> 8u;
  unsigned char Vy = (opcode & 0x0F00) >> 4u;
  registers[Vx] &= registers[Vy];
}
  
void chip_8::OP_8xy3(){
  
  unsigned char Vx = (opcode & 0x0F00) >> 8u;
  unsigned char Vy = (opcode & 0x0F00) >> 4u;
  registers[Vx] ^= registers[Vy];
}

void chip_8::OP_8xy4(){
  unsigned char Vx = (opcode & 0x0F00) >> 8u;
  unsigned char Vy = (opcode & 0x0F00) >> 4u;
  unsigned int sum  = registers[Vx] + registers[Vy];
  if(sum > 255U){registers[0xF]=1;}
  else{registers[0xF]=0;}
  registers[Vx] = sum & 0xFFu;
}

