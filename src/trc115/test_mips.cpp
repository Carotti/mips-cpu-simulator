#include "test_mips.h"

#include <bitset>

int main(){

  mips_test_begin_suite();

  mips_mem_h testMem = mips_mem_create_ram(4096);
  mips_cpu_h testCPU = mips_cpu_create(testMem);

  test basic_sll("sll", "Verify that R8 = R9 << 5 and R9 unchanged", 1);
  basic_sll.writeMem(get_pc(testCPU), instruction_impl_r(0, 9, 8, 5, 0).data);
  basic_sll.writeReg(9, 0xFBABABAB);
  basic_sll.checkReg(8, 0x75757560);
  basic_sll.checkReg(9, 0xFBABABAB);
  basic_sll.perform_test(testCPU, testMem);

  // Note here that R9 is not set again, since it was set in the previous test
  test basic_srl("srl", "Verify that R8 = R9 >> 5 and R9 unchanged", 1);
  basic_srl.writeMem(get_pc(testCPU), instruction_impl_r(0, 9, 8, 5, 2).data);
  basic_srl.checkReg(8, 0x7DD5D5D);
  basic_srl.checkReg(9, 0xFBABABAB);
  basic_srl.perform_test(testCPU, testMem);

  test basic_sra("sra", "Verify that R8 = R9 arith>> 5 and R9 unchanged", 1);
  basic_sra.writeMem(get_pc(testCPU), instruction_impl_r(0, 9, 8, 5, 3).data);
  basic_sra.writeReg(9, 0x80000000);
  basic_sra.checkReg(8, 0xFC000000);
  basic_sra.checkReg(9, 0x80000000);
  basic_sra.perform_test(testCPU, testMem);

  test basic_sllv("sllv", "Verify that R8 = R9 << R10 and R9 R10 unchanged", 1);
  basic_sllv.writeMem(get_pc(testCPU), instruction_impl_r(10, 9, 8, 0, 4).data);
  basic_sllv.writeReg(9, 0xFBABABAB);
  basic_sllv.writeReg(10, 5);
  basic_sllv.checkReg(8, 0x75757560);
  basic_sllv.checkReg(9, 0xFBABABAB);
  basic_sllv.checkReg(10, 5);
  basic_sllv.perform_test(testCPU, testMem);

  // Registers 9 and 10 already set in previous test
  test basic_srlv("srlv", "Verify that R8 = R9 >> R10 and R9 R10 unchanged", 1);
  basic_srlv.writeMem(get_pc(testCPU), instruction_impl_r(10, 9, 8, 0, 6).data);
  basic_srlv.checkReg(8, 0x7DD5D5D);
  basic_srlv.checkReg(9, 0xFBABABAB);
  basic_srlv.checkReg(10, 5);
  basic_srlv.perform_test(testCPU, testMem);

  test basic_srav("srav",
    "Verify that R8 = R9 arith>> R10 and R9 R10 unchanged", 1);
  basic_srav.writeMem(get_pc(testCPU), instruction_impl_r(10, 9, 8, 0, 7).data);
  basic_srav.writeReg(9, 0x80000000);
  basic_srav.writeReg(10, 5);
  basic_srav.checkReg(8, 0xFC000000);
  basic_srav.checkReg(9, 0x80000000);
  basic_srav.checkReg(10, 5);
  basic_srav.perform_test(testCPU, testMem);

  test basic_jr("jr", "Verify that pc = R9 and R9 unchanged", 1);
  basic_jr.writeMem(get_pc(testCPU), instruction_impl_r(9, 0, 0, 0, 8).data);
  basic_jr.writeReg(9, 0x000000A4);
  basic_jr.checkReg(255, 0x000000A4);
  basic_jr.checkReg(9, 0x000000A4);
  basic_jr.perform_test(testCPU, testMem);

  test basic_jalr("jalr", "Verify that pc = R9, R1 = R9 + 8 and R9 unchanged", 1);
  basic_jalr.writeMem(get_pc(testCPU), instruction_impl_r(9, 0, 31, 0, 9).data);
  basic_jalr.checkReg(9, 0x000000A4);
  basic_jalr.checkReg(255, 0x000000A4);
  basic_jalr.checkReg(31, 0x000000AC);
  basic_jalr.perform_test(testCPU, testMem);

  // TODO: Add nops since 2 instructions before mult/divide can't be mfhi or mflo
  test basic_multu("multu", "Verify values of hi and lo after R10 * R11", 5);
  basic_multu.writeMem(get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 25).data);
  basic_multu.writeMem(get_pc(testCPU) + 4, instruction_impl_r(0, 0, 8, 0, 16).data);
  basic_multu.writeMem(get_pc(testCPU) + 8, instruction_impl_r(0, 0, 9, 0, 18).data);
  basic_multu.writeMem(get_pc(testCPU) + 12, instruction_impl_r(0, 0, 0, 0, 0).data);
  basic_multu.writeMem(get_pc(testCPU) + 16, instruction_impl_r(0, 0, 0, 0, 0).data);
  basic_multu.writeReg(10, 0x0BABABAB);
  basic_multu.writeReg(11, 0xFEFEFEFE);
  basic_multu.checkReg(8, 0x0B9FF447);
  basic_multu.checkReg(9, 0xE651FDAA);
  basic_multu.perform_test(testCPU, testMem);

  test basic_mult("mult", "Verify values of hi and lo after R10 * R11", 5);
  basic_mult.writeMem(get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 24).data);
  basic_mult.writeMem(get_pc(testCPU) + 4, instruction_impl_r(0, 0, 8, 0, 16).data);
  basic_mult.writeMem(get_pc(testCPU) + 8, instruction_impl_r(0, 0, 9, 0, 18).data);
  basic_mult.writeMem(get_pc(testCPU) + 12, instruction_impl_r(0, 0, 0, 0, 0).data);
  basic_mult.writeMem(get_pc(testCPU) + 16, instruction_impl_r(0, 0, 0, 0, 0).data);
  basic_mult.checkReg(8, 0xFFF4489C);
  basic_mult.checkReg(9, 0xE651FDAA);
  basic_mult.perform_test(testCPU, testMem);

  test basic_divu("divu", "Check hi and lo after R10 / R11", 5);
  basic_divu.writeMem(get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 27).data);
  basic_divu.writeMem(get_pc(testCPU) + 4, instruction_impl_r(0, 0, 8, 0, 16).data);
  basic_divu.writeMem(get_pc(testCPU) + 8, instruction_impl_r(0, 0, 9, 0, 18).data);
  basic_divu.writeMem(get_pc(testCPU) + 12, instruction_impl_r(0, 0, 0, 0, 0).data);
  basic_divu.writeMem(get_pc(testCPU) + 16, instruction_impl_r(0, 0, 0, 0, 0).data);
  basic_divu.writeReg(10, 0xF1489B44);
  basic_divu.writeReg(11, 0x000000A1);
  basic_divu.checkReg(8, 121);
  basic_divu.checkReg(9, 25143275);
  basic_divu.perform_test(testCPU, testMem);

  test basic_div("div", "Check hi and lo after R10 / R11 (signed)", 3);
  basic_div.writeMem(get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 26).data);
  basic_div.writeMem(get_pc(testCPU) + 4, instruction_impl_r(0, 0, 8, 0, 16).data);
  basic_div.writeMem(get_pc(testCPU) + 8, instruction_impl_r(0, 0, 9, 0, 18).data);
  basic_div.writeMem(get_pc(testCPU) + 12, instruction_impl_r(0, 0, 0, 0, 0).data);
  basic_div.writeMem(get_pc(testCPU) + 16, instruction_impl_r(0, 0, 0, 0, 0).data);
  basic_div.checkReg(8, 0xffffff87);
  basic_div.checkReg(9, 0xffe8999d);
  basic_div.perform_test(testCPU, testMem);

  test div_negop2("div", "Check hi and lo after R10 / R11 (signed) where R11 is negative", 3);
  div_negop2.writeMem(get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 26).data);
  div_negop2.writeMem(get_pc(testCPU) + 4, instruction_impl_r(0, 0, 8, 0, 16).data);
  div_negop2.writeMem(get_pc(testCPU) + 8, instruction_impl_r(0, 0, 9, 0, 18).data);
  div_negop2.writeReg(10, 0x0BF9F147);
  div_negop2.writeReg(11, 0xFFFFFF12);
  div_negop2.checkReg(8, 0xB);
  div_negop2.checkReg(9, 0xfff31e2e);
  div_negop2.perform_test(testCPU, testMem);

  mips_mem_free(testMem);
  testMem = NULL;
  mips_cpu_free(testCPU);
  testCPU = NULL;

  mips_test_end_suite();

  return 0;
}

uint32_t get_pc(mips_cpu_h state){
  uint32_t pc;
  mips_cpu_get_pc(state, &pc);
  return pc;
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
    } else if (regCheck[i].index == 255){
      mips_cpu_get_pc(state, &testValue);
    }
    if (testValue != regCheck[i].value){
      success = 0;
    }
  }

  mips_test_end_test(testID, success, testDescription);

  return mips_Success;
}
