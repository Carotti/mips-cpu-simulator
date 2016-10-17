#include "test_mips.h"

#include <bitset>


int main(){

  mips_test_begin_suite();
  mips_mem_h testMem = mips_mem_create_ram(4096);
  mips_cpu_h testCPU = mips_cpu_create(testMem);

  mips_cpu_set_register(testCPU, 9, 0xFBABABAB)
  uint_8_t resultRegs[2] = {8, 255}; // Test Register 8 and Program Counter
  uint_8_t expectedResults[2] = {}
  test_instruction(
    "srl",
    "Verify that R8 = R9 << 5",
    testCPU,
    instruction_impl(0,9,8,5,2),

  );

  mips_cpu_free(testCPU);
  testCPU = NULL;

  mips_test_end_suite();

  return 0;
}

mips_error test_instruction(
  char* instructionName,
  char* testDescription,
  mips_cpu_h state,
  instruction_impl instruction,
  // Array of which registers the result can be expected from, 255 means pc
  uint8_t* resultRegs,
  // Array of expected results
  uint32_t* expectedResults
){
  if(state == NULL ||
    resultRegs == NULL ||
    expectedResults == NULL ||
    instructionName == NULL ||
    testDescription == NULL){
    return mips_ErrorInvalidHandle;
  }
  int testID = mips_test_begin_test(instructionName);
  // write the instruction to the next instruction to be executed
  mips_mem_write(state->mem, state->pc, 4, (uint8_t*)&instruction.data);
  mips_cpu_step(state);

  // Assume the test is successful
  int success = 1;

  uint32_t regValue = 0;

  // If any of the registers don't match the expected result, fail the test
  for (unsigned i = 0; i < unsigned(resultRegs.size()); i++) {

    if(resultRegs[i] < 32){
      // Check Register
      mips_cpu_get_register(state, resultRegs[i], &regValue);
    } else if(resultRegs[i] == 255){
      // Check Program Counter
      mips_cpu_get_pc(state, &regValue);
    } else {
      return mips_ErrorInvalidArgument;
    }

    if (regValue != expectedResults[i]) {
      success = 0;
    }
  }

  mips_test_end_test(testID, success, testDescription);
  return mips_Success;
}
