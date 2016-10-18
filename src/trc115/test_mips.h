#include "../shared/mips.h"
#include "mips_instruction.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

struct memory_state{
  uint32_t address;
  uint32_t value;
};

struct register_state{
  uint8_t index; // 255 for checking program counter
  uint32_t value;
};

struct test{
  mips_cpu_h state;
  mips_mem_h mem;
  const char* testName;
  const char* testDescription;
  unsigned numInstructions;
  vector<memory_state> memWrite;
  vector<memory_state> memCheck;
  vector<register_state> regWrite;
  vector<register_state> regCheck;
};

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
