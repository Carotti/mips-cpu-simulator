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

  memory_state(uint32_t addressIn, uint32_t valueIn):
    address(addressIn),
    value(valueIn){}
};

struct register_state{
  uint8_t index; // 255 for checking program counter
  uint32_t value;

  register_state(uint8_t indexIn, uint32_t valueIn):
    index(indexIn),
    value(valueIn){}
};

struct test{
  const char* testName;
  const char* testDescription;
  unsigned numInstructions;
  vector<memory_state> memWrite;
  vector<memory_state> memCheck;
  vector<register_state> regWrite;
  vector<register_state> regCheck;

  // Constructor for test
  test(
    const char* testNameIn,
    const char* testDescriptionIn,
    unsigned numInstructionsIn):
    testName(testNameIn),
    testDescription(testDescriptionIn),
    numInstructions(numInstructionsIn){}

  mips_error perform_test(mips_cpu_h state, mips_mem_h mem);
};
