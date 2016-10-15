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
  uint8_t rFunction;

  // Constructor for instruction_impl without rFunction defined
  instruction_impl(char typeIn, uint8_t opCodeIn):
    type(typeIn),
    opCode(opCodeIn),
    // Define rFunction to 63 (unused) to prevent accidentally using it
    rFunction(63){}

  // Constructor where rFunction is defined
    instruction_impl(char typeIn, uint8_t opCodeIn, uint8_t rFunctionIn):
      type(typeIn),
      opCode(opCodeIn),
      rFunction(rFunctionIn){}
};

struct mips_cpu_impl{

  // Constructor function for mips_cpu_impl
  mips_cpu_impl(mips_mem_h memIn):
    mem(memIn),
    debugLevel(0),
    // Default to having no debug destination, as debugLevel defaults to 0
    debugDest(NULL){
      // Define the instruction set for the CPU
      // Commented out instructions not yet implemented
      instruction_set.push_back(instruction_impl('r', 0, 0)); // sll

      instruction_set.push_back(instruction_impl('r', 0, 2)); // srl
      instruction_set.push_back(instruction_impl('r', 0, 3)); // sra
      instruction_set.push_back(instruction_impl('r', 0, 4)); // sllv

      instruction_set.push_back(instruction_impl('r', 0, 6)); // srlv
      instruction_set.push_back(instruction_impl('r', 0, 7)); // srav
      instruction_set.push_back(instruction_impl('r', 0, 8)); // jr
      instruction_set.push_back(instruction_impl('r', 0, 9)); // jalr

      instruction_set.push_back(instruction_impl('r', 0, 12)); // syscall
      instruction_set.push_back(instruction_impl('r', 0, 13)); // break

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
      instruction_set.push_back(instruction_impl('r', 0, 39)); // nor

      instruction_set.push_back(instruction_impl('r', 0, 42)); // slt
      instruction_set.push_back(instruction_impl('r', 0, 43)); // sltu




      instruction_set.push_back(instruction_impl('i', 8)); // addi
      instruction_set.push_back(instruction_impl('i', 9)); // addiu
      instruction_set.push_back(instruction_impl('i', 12)); // andi
      instruction_set.push_back(instruction_impl('i', 4)); // beq

      // bgez and bltz share this opcode
      // bgez has dest = 1
      // bltz has dest = 0
      instruction_set.push_back(instruction_impl('i', 1));

      instruction_set.push_back(instruction_impl('i', 7)); // bgtz (dest = 0)
      instruction_set.push_back(instruction_impl('i', 6)); // blez (dest = 0)

      instruction_set.push_back(instruction_impl('i', 5)); // bne
      instruction_set.push_back(instruction_impl('i', 32)); // lb
      instruction_set.push_back(instruction_impl('i', 36)); // lbu
      instruction_set.push_back(instruction_impl('i', 33)); // lh
      instruction_set.push_back(instruction_impl('i', 37)); // bne
      instruction_set.push_back(instruction_impl('i', 15)); // lui
      instruction_set.push_back(instruction_impl('i', 49)); // lwc1
      instruction_set.push_back(instruction_impl('i', 13)); // ori
      instruction_set.push_back(instruction_impl('i', 40)); // sb
      instruction_set.push_back(instruction_impl('i', 10)); // slti
      instruction_set.push_back(instruction_impl('i', 11)); // sltiu
      instruction_set.push_back(instruction_impl('i', 41)); // sh
      instruction_set.push_back(instruction_impl('i', 43)); // sw
      instruction_set.push_back(instruction_impl('i', 57)); // swc1
      instruction_set.push_back(instruction_impl('i', 14)); // xori

    }

  uint32_t pc;

  uint32_t registers[NUMREGISTERS];

  mips_mem_h mem;

  unsigned debugLevel;

  FILE* debugDest;

  vector<instruction_impl> instruction_set;

};
