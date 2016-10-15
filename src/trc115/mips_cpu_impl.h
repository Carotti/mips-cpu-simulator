#include <vector>

#include "../shared/mips.h"

// Define number of registers as a constant, allows for easier migration to
// a different architecture
#define NUMREGISTERS 32

using namespace std;

// Used to define the instruction set
struct instruction_impl{
  char type;
  uint8_t opCode;

  // Only used for R type insructions
  uint8_t function;
  uint8_t source2;
  uint8_t shift;

  // Used for R and I type instructions
  uint8_t source1;
  uint8_t dest;

  // Only used for I type instructions
  uint16_t immediate;

  // Only used for J type instructions
  uint32_t address;

  // Only used for an 'actual' function rather than instruction set member
  uint32_t data;

  // Constructor for instruction_impl without rFunction defined
  instruction_impl(char typeIn, uint8_t opCodeIn):
    type(typeIn),
    opCode(opCodeIn),
    // Define rFunction to 63 (unused) to prevent accidentally using it
    function(63){}

  // Constructor where function is defined
  instruction_impl(char typeIn, uint8_t opCodeIn, uint8_t functionIn):
    type(typeIn),
    opCode(opCodeIn),
    function(functionIn){}

  // Constructor where there is actual raw data - data first allows overload
  instruction_impl(uint32_t dataIn, char typeIn, uint8_t opCodeIn):
    type(typeIn),
    opCode(opCodeIn),
    data(dataIn){}
};

struct mips_cpu_impl{

  // Constructor function for mips_cpu_impl
  mips_cpu_impl(mips_mem_h memIn):
    mem(memIn),
    debugLevel(0),
    // Default to having no debug destination, as debugLevel defaults to 0
    debugDest(NULL){
      // Define the instruction set for the CPU
      // * denotes an instruction not in the specification

      // r type instructions share opcode 0 and have their own function
      instruction_set.push_back(instruction_impl('r', 0, 0)); // sll

      instruction_set.push_back(instruction_impl('r', 0, 2)); // srl
      instruction_set.push_back(instruction_impl('r', 0, 3)); // sra
      instruction_set.push_back(instruction_impl('r', 0, 4)); // sllv

      instruction_set.push_back(instruction_impl('r', 0, 6)); // srlv
      instruction_set.push_back(instruction_impl('r', 0, 7)); // srav
      instruction_set.push_back(instruction_impl('r', 0, 8)); // jr
      instruction_set.push_back(instruction_impl('r', 0, 9)); // jalr

      instruction_set.push_back(instruction_impl('r', 0, 12)); // syscall*
      instruction_set.push_back(instruction_impl('r', 0, 13)); // break*

      instruction_set.push_back(instruction_impl('r', 0, 16)); // mfhi
      instruction_set.push_back(instruction_impl('r', 0, 17)); // mthi
      instruction_set.push_back(instruction_impl('r', 0, 18)); // mflo
      instruction_set.push_back(instruction_impl('r', 0, 19)); // mtlo

      instruction_set.push_back(instruction_impl('r', 0, 24)); // mult
      instruction_set.push_back(instruction_impl('r', 0, 25)); // multu
      instruction_set.push_back(instruction_impl('r', 0, 26)); // div
      instruction_set.push_back(instruction_impl('r', 0, 27)); // divu

      instruction_set.push_back(instruction_impl('r', 0, 32)); // add
      instruction_set.push_back(instruction_impl('r', 0, 33)); // addu
      instruction_set.push_back(instruction_impl('r', 0, 34)); // sub
      instruction_set.push_back(instruction_impl('r', 0, 35)); // subu
      instruction_set.push_back(instruction_impl('r', 0, 36)); // and
      instruction_set.push_back(instruction_impl('r', 0, 37)); // or
      instruction_set.push_back(instruction_impl('r', 0, 38)); // xor
      instruction_set.push_back(instruction_impl('r', 0, 39)); // nor*

      instruction_set.push_back(instruction_impl('r', 0, 42)); // slt
      instruction_set.push_back(instruction_impl('r', 0, 43)); // sltu

      // bltz, bgez, bltzal and bgezal share opcode 1
      // bltz has rt = 0
      // bgez has rt = 1
      // bltzal has rt = 16
      // bgezal has rt = 17
      instruction_set.push_back(instruction_impl('i', 1));
      instruction_set.push_back(instruction_impl('j', 2)); // j
      instruction_set.push_back(instruction_impl('j', 3)); // jal
      instruction_set.push_back(instruction_impl('i', 4)); // beq
      instruction_set.push_back(instruction_impl('i', 5)); // bne
      instruction_set.push_back(instruction_impl('i', 6)); // blez (dest = 0)
      instruction_set.push_back(instruction_impl('i', 7)); // bgtz (dest = 0)
      instruction_set.push_back(instruction_impl('i', 8)); // addi
      instruction_set.push_back(instruction_impl('i', 9)); // addiu
      instruction_set.push_back(instruction_impl('i', 10)); // slti
      instruction_set.push_back(instruction_impl('i', 11)); // sltiu
      instruction_set.push_back(instruction_impl('i', 12)); // andi
      instruction_set.push_back(instruction_impl('i', 13)); // ori
      instruction_set.push_back(instruction_impl('i', 14)); // xori
      instruction_set.push_back(instruction_impl('i', 15)); // lui

      instruction_set.push_back(instruction_impl('i', 32)); // lb
      instruction_set.push_back(instruction_impl('i', 33)); // lh
      instruction_set.push_back(instruction_impl('i', 34)); // lwl
      instruction_set.push_back(instruction_impl('i', 35)); // lw
      instruction_set.push_back(instruction_impl('i', 36)); // lbu
      instruction_set.push_back(instruction_impl('i', 37)); // lhu
      instruction_set.push_back(instruction_impl('i', 38)); // lwr

      instruction_set.push_back(instruction_impl('i', 40)); // sb
      instruction_set.push_back(instruction_impl('i', 41)); // sh
      instruction_set.push_back(instruction_impl('i', 42)); // swl*
      instruction_set.push_back(instruction_impl('i', 43)); // sw
    }

  uint32_t pc;

  uint32_t registers[NUMREGISTERS];

  mips_mem_h mem;

  unsigned debugLevel;

  FILE* debugDest;

  vector<instruction_impl> instruction_set;
};
