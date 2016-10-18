#include "test_mips.h"

#include <bitset>


int main(){

  mips_test_begin_suite();

  mips_mem_h testMem = mips_mem_create_ram(4096);
  mips_cpu_h testCPU = mips_cpu_create(testMem);

  // Do testing here

  test basic_sll("sll", "Verify that R8 = R9 << 5 and R9 is unchanged", 1);
  basic_sll.memWrite.push_back(memory_state(
    0, instruction_impl_r(0, 9, 8, 5, 0).data));
  basic_sll.regWrite.push_back(register_state(9, 0xFBABABAB));
  basic_sll.regCheck.push_back(register_state(8, 0x75757560));
  basic_sll.regCheck.push_back(register_state(9, 0xFBABABAB));
  basic_sll.perform_test(testCPU, testMem);

  mips_mem_free(testMem);
  testMem = NULL;
  mips_cpu_free(testCPU);
  testCPU = NULL;

  mips_test_end_suite();

  return 0;
}

mips_error test::perform_test(mips_cpu_h state, mips_mem_h mem){

  // Setup the initial conditions of the test
  for(unsigned i = 0; i < unsigned(memWrite.size()); i++){
    mips_mem_write(mem, memWrite[i].address, 4, (uint8_t*)&memWrite[i].value);
  }

  for(unsigned i = 0; i < unsigned(regWrite.size()); i++){
    if (regWrite[i].index < 32){
      mips_cpu_set_register(
        state,
        unsigned(regWrite[i].index),
        regWrite[i].value);
    } else if (regWrite[i].index == 255){
      mips_cpu_set_pc(state, regWrite[i].value);
    }
  }

  int testID = mips_test_begin_test(testName);
  int success = 1;

  // Make the CPU perform the required number of instructions for the test
  for(unsigned i = 0; i < numInstructions; i++){
    mips_cpu_step(state);
  }

  // Check the state of the CPU is as expected afterward
  for(unsigned i = 0; i < unsigned(memCheck.size()); i++){
    uint8_t testValue[4];
    mips_mem_read(mem, memCheck[i].address, 4, testValue);
    if (uint32_t((testValue[0] << 24) | (testValue[1] << 16) |
      (testValue [2] << 8) | testValue [3]) != memCheck[i].value) {
        success = 0;
    }
  }

  for(unsigned i = 0; i < unsigned(regCheck.size()); i++){
    uint32_t testValue = 0;
    if (regCheck[i].index < 32){
      mips_cpu_get_register(state, unsigned(regCheck[i].index), &testValue);
    } else if (regWrite[i].index == 255){
      mips_cpu_get_pc(state, &testValue);
    }
    if (testValue != regCheck[i].value){
      success = 0;
    }
  }

  mips_test_end_test(testID, success, testDescription);

  return mips_Success;
}
