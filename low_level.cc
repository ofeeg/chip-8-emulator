#include "low_level.hh"
#include <chrono>
#include <cstring>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x500;
const unsigned int FONTSET_SIZE = 80;

unsigned char fontset[FONTSET_SIZE] =
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

  table[0x0]=&chip_8::table_0;
  table[0x1]=&chip_8::OP_1nnn;
  table[0x2]=&chip_8::OP_2nnn;
  table[0x3]=&chip_8::OP_3xkk;
  table[0x4]=&chip_8::OP_4xkk;
  table[0x5]=&chip_8::OP_5xy0;
  table[0x6]=&chip_8::OP_6xkk;
  table[0x7]=&chip_8::OP_7xkk;
  table[0x8]=&chip_8::table_8;
  table[0x9]=&chip_8::OP_9xy0;
  table[0xA]=&chip_8::OP_Annn;
  table[0xB]=&chip_8::OP_Bnnn;
  table[0xC]=&chip_8::OP_Cxkk;
  table[0xD]=&chip_8::OP_Dxyn;
  table[0xE]=&chip_8::table_E;
  table[0xF]=&chip_8::table_F;

  table0[0x0]=&chip_8::OP_00E0;
  table0[0xE]=&chip_8::OP_00EE;

  table8[0x0]=&chip_8::OP_8xy0;
  table8[0x1]=&chip_8::OP_8xy1;
  table8[0x2]=&chip_8::OP_8xy2;
  table8[0x3]=&chip_8::OP_8xy3;
  table8[0x4]=&chip_8::OP_8xy4;
  table8[0x5]=&chip_8::OP_8xy5;
  table8[0x6]=&chip_8::OP_8xy6;
  table8[0x7]=&chip_8::OP_8xy7;
  table8[0xE]=&chip_8::OP_8xyE;

  tableE[0x1]=&chip_8::OP_ExA1;
  tableE[0xE]=&chip_8::OP_Ex9E;

  tableF[0x07]=&chip_8::OP_Fx07;
  tableF[0x0A]=&chip_8::OP_Fx0A;
  tableF[0x15]=&chip_8::OP_Fx15;
  tableF[0x18]=&chip_8::OP_Fx18;
  tableF[0x1E]=&chip_8::OP_Fx1E;
  tableF[0x29]=&chip_8::OP_Fx29;
  tableF[0x33]=&chip_8::OP_Fx33;
  tableF[0x55]=&chip_8::OP_Fx55;
  tableF[0x65]=&chip_8::OP_Fx65;
}

void chip_8::cycle(){
  //Fetch
  opcode = (memory[pc] << 8u) | memory[pc + 1];
  pc+=2;

  //decode and execute
  ((*this).*(table[(opcode & 0xF000u) >> 12u]))();
  
  if(delay_timer > 0){--delay_timer;}
  if(sound_timer > 0){--sound_timer;}
}

void chip_8::table_8(){((*this).*(chip_8::table8[opcode & 0x000Fu]))();}
void chip_8::table_0(){((*this).*(chip_8::table0[opcode & 0x000Fu]))();}
void chip_8::table_E(){((*this).*(chip_8::tableE[opcode & 0x000Fu]))();}
void chip_8::table_F(){((*this).*(chip_8::tableF[opcode & 0x00FFu]))();}

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
  unsigned short address = opcode & 0x0FFFu;
  pc = address;
}

void chip_8::OP_2nnn(){ //CALL addr
  unsigned short address = opcode & 0x0FFFu;
  stack[sp] = pc;
  ++sp;
  pc = address;
}

void chip_8::OP_3xkk(){ //SE Vx byte
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char byte = opcode & 0x00FFu;
  if(registers[Vx] == byte){
    pc+=2;
  }
}
  
void chip_8::OP_4xkk(){ //SNE Vx byte
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char byte = (opcode & 0x00FFu);
  if(registers[Vx] != byte){
    pc+=2;
  }
}  

void chip_8::OP_5xy0(){ //SE Vx Vy
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char Vy = (opcode & 0x00F0u) >> 4u;

  if(registers[Vx] == registers[Vy]){
    pc+=2;
  }
}


void chip_8::OP_6xkk(){ //LD Vx byte
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char byte = (opcode & 0x00FFu);
  registers[Vx] = byte;
}

void chip_8::OP_7xkk(){ //ADD Vx byte
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char byte = (opcode & 0x00FFu);
  registers[Vx]+=byte;
}

void chip_8::OP_8xy0(){// LD Vx Vy
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char Vy = (opcode & 0x00F0u) >> 4u;
  registers[Vx] = registers[Vy];
}

void chip_8::OP_8xy1(){ // OR Vx Vy
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char Vy = (opcode & 0x00F0u) >> 4u;
  registers[Vx] |= registers[Vy];
}

void chip_8::OP_8xy2(){ //AND Vx Vy
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char Vy = (opcode & 0x00F0u) >> 4u;
  registers[Vx] &= registers[Vy];
}
  
void chip_8::OP_8xy3(){ // XOR Vx Vy
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char Vy = (opcode & 0x00F0u) >> 4u;
  registers[Vx] ^= registers[Vy];
}

void chip_8::OP_8xy4(){ // ADD Vx Vy
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char Vy = (opcode & 0x00F0u) >> 4u;
  unsigned int sum  = registers[Vx] + registers[Vy];
  if(sum > 255U){registers[0xF]=1;}
  else{registers[0xF]=0;}
  registers[Vx] = sum & 0xFFu;
}

void chip_8::OP_8xy5(){ //SUB Vx Vy
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char Vy = (opcode & 0x00F0u) >> 4u;
  if(registers[Vx] > registers[Vy]) {registers[0xF] = 1;}
  else{registers[0xF] = 0;}
  registers[Vx] -= registers[Vy];
}

void chip_8::OP_8xy6(){ //SHR Vx
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  registers[0xF] = (registers[Vx] & 0x1u);
  registers[Vx] >>= 1;
}

void chip_8::OP_8xy7(){ //SUBN Vx, Vy
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char Vy = (opcode & 0x00F0u) >> 4u;
  if(registers[Vy] > registers[Vx]){
    registers[0xF] = 1;
  }
  else{
    registers[0xF] = 0;
  }
  registers[Vx] = registers[Vy] - registers[Vx];
}

void chip_8::OP_8xyE(){ //SHL Vx {, Vy}
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
  registers[Vx] <<= 1;
}

void chip_8::OP_9xy0(){ // SNE Vx, Vy
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char Vy = (opcode & 0x00F0u) >> 4u;

  if(registers[Vx] != registers[Vy]){pc += 2;}
}

void chip_8::OP_Annn(){ //LD I, address
  unsigned short address = opcode & 0x0FFFu;
  I = address;
}

void chip_8::OP_Bnnn(){ //JP V0, address
  unsigned short address = opcode & 0x0FFFu;
  pc = registers[0] + address;
}

void chip_8::OP_Cxkk(){ //RND Vx, byte
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char byte = opcode & 0x00FFu;

  registers[Vx] = rand_byte(randGen) & byte;
}

void chip_8::OP_Dxyn(){ // DRW Vx, Vy, nibble
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char Vy = (opcode & 0x00F0u) >> 4u;
  unsigned char height = opcode & 0x000Fu;
  unsigned char x_pos = registers[Vx] % VIDEO_WIDTH;
  unsigned char y_pos = registers[Vy] % VIDEO_HEIGHT;
  registers[0xF] = 0;
  for (unsigned int row = 0; row < height; ++row){
    unsigned char sprite_byte = memory[I + row];
    for (unsigned int col = 0; col < 8; ++col){
      unsigned char sprite_pixel = sprite_byte & (0x80u >> col);
      unsigned int* screen_pixel = &video[(y_pos + row) * VIDEO_WIDTH + (x_pos + col)];
      if (sprite_pixel){
	//Screen pixel also on - collision
	if(*screen_pixel == 0xFFFFFFFF){
	  registers[0xF] = 1;
	}
	*screen_pixel ^= 0xFFFFFFFF;
      }
    }
  }
}

void chip_8::OP_Ex9E(){// SKP Vx: skips next instruction if key is pressed
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char key = registers[Vx];
  if(keypad[key]){pc+=2;}
}

void chip_8::OP_ExA1(){//SKNP Vx
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char key = registers[Vx];
  if(!(keypad[key])){pc+=2;}
}

void chip_8::OP_Fx07(){//LD Vx, DT
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  registers[Vx] = delay_timer;
}

void chip_8::OP_Fx0A(){// LD Vx, K
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  if(keypad[0]){registers[Vx] = 0;}
  else if(keypad[1]){registers[Vx] = 1;}
  else if(keypad[2]){registers[Vx] = 2;}
  else if(keypad[3]){registers[Vx] = 3;}
  else if(keypad[4]){registers[Vx] = 4;}
  else if(keypad[5]){registers[Vx] = 5;}
  else if(keypad[6]){registers[Vx] = 6;}
  else if(keypad[7]){registers[Vx] = 7;}
  else if(keypad[8]){registers[Vx] = 8;}
  else if(keypad[9]){registers[Vx] = 9;}
  else if(keypad[10]){registers[Vx] = 10;}
  else if(keypad[11]){registers[Vx] = 11;}
  else if(keypad[12]){registers[Vx] = 12;}
  else if(keypad[13]){registers[Vx] = 13;}
  else if(keypad[14]){registers[Vx] = 14;}
  else if(keypad[15]){registers[Vx] = 15;}
  else{pc-=2;}
}

void chip_8::OP_Fx15(){// LD DT, Vx
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  delay_timer = registers[Vx];
}

void chip_8::OP_Fx18(){// LD ST, Vx
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  sound_timer = registers[Vx];
}

void chip_8::OP_Fx1E(){// ADD I, Vx
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  I += registers[Vx];
}

void chip_8::OP_Fx29(){// LD F, Vx
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char digit = registers[Vx];
  I = FONTSET_START_ADDRESS + (5 * digit);
}

void chip_8::OP_Fx33(){// LD B, Vx
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  unsigned char digit = registers[Vx];
  memory[I+2] = digit%10;
  digit/=10;
  memory[I+1] =((digit%10));
  digit/=10;
  memory[I] = (digit%10);
}

void chip_8::OP_Fx55(){// LD [I], Vx
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  for(unsigned char i = 0; i<=Vx; ++i){
    memory[I+i] = registers[i];
  }
}

void chip_8::OP_Fx65(){//LD Vx, [I]
  unsigned char Vx = (opcode & 0x0F00u) >> 8u;
  for(unsigned char i = 0; i<=Vx; ++i){
    registers[i] = memory[I+i];
  }
}



