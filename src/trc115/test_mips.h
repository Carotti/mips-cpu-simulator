#include "mips.h"
#include "mips_instruction_test.h"

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
  vector<memory_state> memCheck;
  vector<register_state> regCheck;
  int success;

  // Constructor for test
  test(
    const char* testNameIn,
    const char* testDescriptionIn,
    unsigned numInstructionsIn):
    testName(testNameIn),
    testDescription(testDescriptionIn),
    numInstructions(numInstructionsIn),
    // -1 indicates test has not run, 0 is failure and 1 is pass
    success(-1){}

  mips_error perform_test(mips_cpu_h state, mips_mem_h mem);

  void checkMem(uint32_t address, uint32_t value){
    memCheck.push_back(memory_state(address, value));
  }

  void checkReg(uint8_t index, uint32_t value){
    regCheck.push_back(register_state(index, value));
  }
};

uint32_t get_pc(mips_cpu_h state);
void writeMem(mips_mem_h mem, uint32_t address, uint32_t value);
void writeReg(mips_cpu_h state, uint8_t index, uint32_t value);

void byte_swap_test(uint32_t &value);
