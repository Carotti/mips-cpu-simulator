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
    debugDest(NULL),
    externalErrorStatus(mips_Success),
    internalErrorStatus(mips_Success)
  {
    // Reset the CPU on initialisation to ensure all regs set to 0
    mips_cpu_reset(this);
  }

  uint32_t pc;

  uint32_t registers[NUMREGISTERS];

  mips_mem_h mem;

  unsigned debugLevel;

  FILE* debugDest;

  // Gives the current error status of the CPU (Initialised to mips_Success)
  // This is returned in functions which require it, and should only use codes
  // understood by the public API (see mips_core.h)
  mips_error externalErrorStatus;

  // Should never be returned by functions defined in the public API, used only
  // for debugging
  mips_error internalErrorStatus;
};
