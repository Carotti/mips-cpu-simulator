#include "test_mips.h"

#include <bitset>


int main(){

  mips_test_begin_suite();
  mips_mem_h testMem = mips_mem_create_ram(4096);
  mips_cpu_h testCPU = mips_cpu_create(testMem);



  int testID = mips_test_begin_test("srl");
  int success = 0;

  // [00094140] R8 = R9 << 5
  // uint32_t instruction = {0x00, 0x09, 0x41, 0x40};

  // Equivalent instruction: 00094140 R8 = R9 << 5
  instruction_impl_r instruction = instruction_impl_r(0,9,8,5,2);
  cout << bitset<32>(instruction.data) << endl;
  cout << "actual instruction: " << endl << bitset<32>(0x00090142) << endl;

  mips_mem_write(testMem, 0, 4, (uint8_t*)&instruction.data);

  uint32_t r8 = 0xFFFFFFFF;
  uint32_t r9 = 0xFBABABAB;

  uint32_t expectedResult = r9 >> 5;

  mips_cpu_set_register(testCPU, 8, r8);
  mips_cpu_set_register(testCPU, 9, r9);

  uint32_t pc = 0;
  mips_cpu_get_pc(testCPU, &pc);

  cout << pc << endl;
  cout << "error: " << mips_cpu_step(testCPU) << endl;
  mips_cpu_get_pc(testCPU, &pc);
  cout << pc << endl;

  mips_cpu_get_register(testCPU, 8, &r8);

  cout << bitset<32>(r9) << endl;
  cout << bitset<32>(r8) << endl;
  cout << bitset<32>(expectedResult) << endl;

  if(r8 == expectedResult){
    success = 1;
  }

  mips_test_end_test(testID, success, "Testing srl");



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
  // Array of which registers the result can be expected from
  uint8_t* resultRegs,
  // Array of expected results
  uint32_t* expectedResults
){

}

mips_error test_instruction(
  char* instructionName,
  char* testDescription,
  mips_cpu_h state,
  instruction_impl instruction,
  // Array of which registers the result can be expected from
  uint8_t* resultRegs,
  // Array of expected results
  uint32_t* expectedResults,
  uint32_t expectedPc
){

}
