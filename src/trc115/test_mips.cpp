#include "../shared/mips.h"
#include "mips_instruction.h"

#include <iostream>
#include <bitset>

using namespace std;

int main(){

/*  // Create a new instance of mips_mem_provider
  mips_mem_h testMem = mips_mem_create_ram(4096);

  mips_cpu_h testCPU = mips_cpu_create(testMem);

  uint32_t val = 108;
  cout << mips_cpu_set_register(testCPU, 5, val) << endl;
  val = 456;
  cout << mips_cpu_set_register(testCPU, 32, val) << endl;

  for (uint32_t i = 0; i < 32; i++){
    mips_cpu_get_register(testCPU, i, &val);
    cout << val << ", ";
  }

  cout << endl;
  mips_cpu_get_pc(testCPU, &val);
  cout << val << endl;

  val = 405;

  mips_cpu_set_pc(testCPU, val);
  mips_cpu_get_pc(testCPU, &val);
  cout << val << endl;

  mips_cpu_free(testCPU);
  // Set testCPU to NULL to prevent 'double delete' errors
  testCPU = NULL;

  mips_cpu_free(testCPU);*/

  mips_test_begin_suite();
  mips_mem_h testMem = mips_mem_create_ram(4096);
  mips_cpu_h testCPU = mips_cpu_create(testMem);



  int testID = mips_test_begin_test("sll");
  int success = 0;

  // [00094140] R8 = R9 << 5
  // uint32_t instruction = {0x00, 0x09, 0x41, 0x40};

  // Equivalent instruction: 00094140 R8 = R9 << 5
  instruction_impl_r instruction = instruction_impl_r(0,9,8,5,0);
  cout << bitset<32>(instruction.data) << endl;
  cout << "actual instruction: " << endl << bitset<32>(0x00094140) << endl;

  mips_mem_write(testMem, 0, 4, (uint8_t*)&instruction.data);

  uint32_t r8 = 0xFFFFFFFF;
  uint32_t r9 = 0xABABABAB;

  uint32_t expectedResult = r9 << 5;

  mips_cpu_set_register(testCPU, 8, r8);
  mips_cpu_set_register(testCPU, 9, r9);

  cout << "error: " << mips_cpu_step(testCPU) << endl;

  mips_cpu_get_register(testCPU, 8, &r8);

  cout << r8 << endl;
  cout << expectedResult << endl;

  if(r8 == expectedResult){
    success = 1;
  }

  mips_test_end_test(testID, success, "Testing sll");



  mips_cpu_free(testCPU);
  testCPU = NULL;

  mips_test_end_suite();

  return 0;
}
