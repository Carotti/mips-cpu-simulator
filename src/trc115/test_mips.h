#include "../shared/mips.h"
#include "mips_instruction.h"

#include <iostream>

using namespace std;

mips_error test_instruction(
  mips_cpu_h state,
  instruction_impl instruction,
  // Array of which registers the result can be expected from
  uint8_t* resultRegs,
  // Array of expected results
  uint32_t* expectedResults
);

// Overloaded test_instruction for when the program counter needs to be
// checked as well.
mips_error test_instruction(
  mips_cpu_h state,
  instruction_impl instruction,
  // Array of which registers the result can be expected from
  uint8_t* resultRegs,
  // Array of expected results
  uint32_t* expectedResults,
  uint32_t expectedPc
);
