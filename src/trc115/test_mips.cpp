#include "../shared/mips.h"

#include <iostream>

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

  int testID = mips_test_begin_test("<internal>");

  mips_mem_h testMem = mips_mem_create_ram(4096);
  mips_cpu_h testCPU = mips_cpu_create(testMem);

  uint8_t data[4];
  data[0] = 10;
  data[1] = 20;
  data[2] = 30;
  data[3] = 40;

  mips_mem_write(testMem, 0, 4, data);
  mips_cpu_step(testCPU);

  uint32_t value;
  mips_cpu_get_register(testCPU, 0, &value);
  cout << value << endl;

  mips_test_end_test(testID, 1, "Test to see if tests work..?");

  mips_cpu_free(testCPU);
  testCPU = NULL;

  mips_test_end_suite();

  return 0;
}
