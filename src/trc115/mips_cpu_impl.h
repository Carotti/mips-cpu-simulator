#include <vector>

#include "../shared/mips.h"

// Define number of registers as a constant, allows for easier migration to
// a different architecture
#define NUMREGISTERS 32

struct mips_cpu_impl{

  // Constructor function for mips_cpu_impl
  mips_cpu_impl(mips_mem_h memIn):
    mem(memIn),
    debugLevel(0),
    // Default to having no debug destination, as debugLevel defaults to 0
    debugDest(NULL){}

  uint32_t pc;

  uint32_t registers[NUMREGISTERS];

  mips_mem_h mem;

  unsigned debugLevel;

  FILE* debugDest;

  vector<instruction_impl>

};

// Used to define the instruction set
struct instruction_impl{
  char type;
  uint8_t opCode;

  instruction_impl(char typeIn, uint8_t opCodeIn):
    type(typeIn),
    opCode(opCodeIn){}

};

struct r_instruction_impl : public instruction_impl{
  uint8_t function;
};
