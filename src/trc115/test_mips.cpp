#include "../shared/mips.h"

#include <iostream>

using namespace std;

int main(){

  // Create a new instance of mips_mem_provider
  mips_mem_h testMem = mips_mem_create_ram(4096);

  mips_cpu_h testCPU = mips_cpu_create(testMem);

  uint32_t val = 108;
  mips_cpu_set_register(testCPU, 5, val);
  val = 456;
  mips_cpu_set_register(testCPU, 31, val);

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
  // Set testCPU to null to prevent "double delete" issues
  testCPU = NULL;

  mips_cpu_free(testCPU);

  return 0;
}
