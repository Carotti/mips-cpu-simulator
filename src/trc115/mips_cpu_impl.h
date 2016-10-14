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
    debugDest(NULL)
  {
    // Reset the CPU on initialisation to ensure all regs set to 0
    mips_cpu_reset(this);
  }

  uint32_t pc;

  uint32_t registers[NUMREGISTERS];

  mips_mem_h mem;

  unsigned debugLevel;

  FILE* debugDest;
};
