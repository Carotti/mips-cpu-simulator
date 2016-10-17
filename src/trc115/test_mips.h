#include "../shared/mips.h"
#include "mips_instruction.h"

#include <iostream>
#include <string>

using namespace std;

mips_error test_instruction(
  const char* instructionName,
  const char* testDescription,
  mips_cpu_h state,
  mips_mem_h mem,
  instruction_impl instruction,
  uint8_t numResults,
  uint8_t* resultRegs,
  uint32_t* expectedResults
);
