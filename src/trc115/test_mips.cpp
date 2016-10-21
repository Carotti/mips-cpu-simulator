#include "test_mips.h"

#include <bitset>

int main(){

  mips_test_begin_suite();

  mips_mem_h testMem = mips_mem_create_ram(8192);
  mips_cpu_h testCPU = mips_cpu_create(testMem);

  test basic_sll("sll", "Verify that R8 = R9 << 5 and R9 unchanged", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(0, 9, 8, 5, 0).data);
  writeReg(testCPU, 9, 0xFBABABAB);
  basic_sll.checkReg(8, 0x75757560);
  basic_sll.checkReg(9, 0xFBABABAB);
  basic_sll.perform_test(testCPU, testMem);

  test basic_srl("srl", "Verify that R8 = R9 >> 5 and R9 unchanged", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(0, 9, 8, 5, 2).data);
  writeReg(testCPU, 9, 0xFBABABAB);
  basic_srl.checkReg(8, 0x7DD5D5D);
  basic_srl.checkReg(9, 0xFBABABAB);
  basic_srl.perform_test(testCPU, testMem);

  test basic_sra("sra", "Verify that R8 = R9 arith>> 5 and R9 unchanged", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(0, 9, 8, 5, 3).data);
  writeReg(testCPU, 9, 0x80000000);
  basic_sra.checkReg(8, 0xFC000000);
  basic_sra.checkReg(9, 0x80000000);
  basic_sra.perform_test(testCPU, testMem);

  test basic_sllv("sllv", "Verify that R8 = R9 << R10 and R9 R10 unchanged", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 9, 8, 0, 4).data);
  writeReg(testCPU, 9, 0xFBABABAB);
  writeReg(testCPU, 10, 5);
  basic_sllv.checkReg(8, 0x75757560);
  basic_sllv.checkReg(9, 0xFBABABAB);
  basic_sllv.checkReg(10, 5);
  basic_sllv.perform_test(testCPU, testMem);

  // Registers 9 and 10 already set in previous test
  test basic_srlv("srlv", "Verify that R8 = R9 >> R10 and R9 R10 unchanged", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 9, 8, 0, 6).data);
  basic_srlv.checkReg(8, 0x7DD5D5D);
  basic_srlv.checkReg(9, 0xFBABABAB);
  basic_srlv.checkReg(10, 5);
  basic_srlv.perform_test(testCPU, testMem);

  test basic_srav("srav",
    "Verify that R8 = R9 arith>> R10 and R9 R10 unchanged", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 9, 8, 0, 7).data);
  writeReg(testCPU, 9, 0x80000000);
  writeReg(testCPU, 10, 5);
  basic_srav.checkReg(8, 0xFC000000);
  basic_srav.checkReg(9, 0x80000000);
  basic_srav.checkReg(10, 5);
  basic_srav.perform_test(testCPU, testMem);

  test basic_jr("jr", "Verify that pc = R9 and R9 unchanged", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 0, 0, 0, 8).data);
  // R8 = R9 >> 1 in the branch delay slot
  writeMem(testMem, get_pc(testCPU) + 4, instruction_impl_r(0, 9, 8, 1, 2).data);
  writeReg(testCPU, 9, 0x000000A4);
  basic_jr.checkReg(255, 0x000000A4);
  basic_jr.checkReg(9, 0x000000A4);
  basic_jr.perform_test(testCPU, testMem);

  test branch_delay_jr("<internal>", "Testing that the instruction after jr is executed before the branch is taken", 0);
  branch_delay_jr.checkReg(8, 0x00000052);
  branch_delay_jr.perform_test(testCPU, testMem);

  test basic_jalr("jalr", "Verify that pc = R9, R1 = R9 + 8 and R9 unchanged", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 0, 31, 0, 9).data);
  // R8 = R9 >> 3 in the branch delay slot
  writeMem(testMem, get_pc(testCPU) + 4, instruction_impl_r(0, 9, 8, 3, 2).data);
  basic_jalr.checkReg(9, 0x000000A4);
  basic_jalr.checkReg(255, 0x000000A4);
  basic_jalr.checkReg(9, 0x000000A4);
  basic_jalr.checkReg(31, 0x000000AC);
  basic_jalr.perform_test(testCPU, testMem);

  test branch_delay_jalr("<internal>", "Testing that the instruction after jalr is executed before the branch is taken", 0);
  branch_delay_jalr.checkReg(8, 0x00000014);
  branch_delay_jalr.perform_test(testCPU, testMem);

  // nops after since:
  // "Reads of the HI or LO special registers must be separated from
  // subsequent instructions that write to them by two or more other instructions."
  test basic_multu("multu", "Verify values of hi and lo after R10 * R11", 5);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 25).data);
  writeMem(testMem, get_pc(testCPU) + 4, instruction_impl_r(0, 0, 8, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8, instruction_impl_r(0, 0, 9, 0, 18).data);
  writeMem(testMem, get_pc(testCPU) + 12, instruction_impl_r(0, 0, 0, 0, 0).data);
  writeMem(testMem, get_pc(testCPU) + 16, instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 10, 0x0BABABAB);
  writeReg(testCPU, 11, 0xFEFEFEFE);
  basic_multu.checkReg(8, 0x0B9FF447);
  basic_multu.checkReg(9, 0xE651FDAA);
  basic_multu.perform_test(testCPU, testMem);

  test basic_mult("mult", "Verify values of hi and lo after R10 * R11", 5);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 24).data);
  writeMem(testMem, get_pc(testCPU) + 4, instruction_impl_r(0, 0, 8, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8, instruction_impl_r(0, 0, 9, 0, 18).data);
  writeMem(testMem, get_pc(testCPU) + 12, instruction_impl_r(0, 0, 0, 0, 0).data);
  writeMem(testMem, get_pc(testCPU) + 16, instruction_impl_r(0, 0, 0, 0, 0).data);
  basic_mult.checkReg(8, 0xFFF4489C);
  basic_mult.checkReg(9, 0xE651FDAA);
  basic_mult.perform_test(testCPU, testMem);

  test basic_divu("divu", "Check hi and lo after R10 / R11", 5);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 27).data);
  writeMem(testMem, get_pc(testCPU) + 4, instruction_impl_r(0, 0, 8, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8, instruction_impl_r(0, 0, 9, 0, 18).data);
  writeMem(testMem, get_pc(testCPU) + 12, instruction_impl_r(0, 0, 0, 0, 0).data);
  writeMem(testMem, get_pc(testCPU) + 16, instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 10, 0xF1489B44);
  writeReg(testCPU, 11, 0x000000A1);
  basic_divu.checkReg(8, 121);
  basic_divu.checkReg(9, 25143275);
  basic_divu.perform_test(testCPU, testMem);

  test basic_div("div", "Check hi and lo after R10 / R11 (signed)", 3);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 26).data);
  writeMem(testMem, get_pc(testCPU) + 4, instruction_impl_r(0, 0, 8, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8, instruction_impl_r(0, 0, 9, 0, 18).data);
  writeMem(testMem, get_pc(testCPU) + 12, instruction_impl_r(0, 0, 0, 0, 0).data);
  writeMem(testMem, get_pc(testCPU) + 16, instruction_impl_r(0, 0, 0, 0, 0).data);
  basic_div.checkReg(8, 0xffffff87);
  basic_div.checkReg(9, 0xffe8999d);
  basic_div.perform_test(testCPU, testMem);

  test div_negop2("div", "Check hi and lo after R10 / R11 (signed) where R11 is negative", 3);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 26).data);
  writeMem(testMem, get_pc(testCPU) + 4, instruction_impl_r(0, 0, 8, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8, instruction_impl_r(0, 0, 9, 0, 18).data);
  writeReg(testCPU, 10, 0x0BF9F147);
  writeReg(testCPU, 11, 0xFFFFFF12);
  div_negop2.checkReg(8, 0xB);
  div_negop2.checkReg(9, 0xfff31e2e);
  div_negop2.perform_test(testCPU, testMem);

  test cpu_reset("<internal>", "Verify all regs and pc set to 0 after reset", 0);
  // Example of using for loops to define a test -- Why it isn't from a file
  for (unsigned i = 0; i < 32; i++){
    // Check the value of each register
    cpu_reset.checkReg(i, 0);
  }
  mips_cpu_reset(testCPU);
  cpu_reset.perform_test(testCPU, testMem);

  test basic_add("add", "Verify the result of an add with no overflow", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 32).data);
  writeReg(testCPU, 9, 0x0BB8BB8F);
  writeReg(testCPU, 10, 0x00AAA1C1);
  basic_add.checkReg(8, 0xC635D50);
  basic_add.perform_test(testCPU, testMem);

  writeReg(testCPU, 8, 0xFAFAFAFA);
  test add_overflow_pos("add", "Check that dest register doesn't change if positive overflow occurs in add", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 32).data);
  writeReg(testCPU, 9, 0x7A8B8BB1);
  writeReg(testCPU, 10, 0x71649BCD);
  add_overflow_pos.checkReg(8, 0xFAFAFAFA);

  mips_error overflowErrorPos = add_overflow_pos.perform_test(testCPU, testMem);
  int overflowTestPos =  mips_test_begin_test("add");
  if (overflowErrorPos == mips_ExceptionArithmeticOverflow){
    mips_test_end_test(overflowTestPos, 1, "Check for Overflow Exception on adding +ve");
  } else {
    mips_test_end_test(overflowTestPos, 0, "Check for Overflow Exception on adding +ve");
  }

  writeReg(testCPU, 8, 0xFAFAFAFA);
  test add_overflow_neg("add", "Check that dest register doesn't change if negative overflow occurs in add", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 32).data);
  writeReg(testCPU, 9, 0x80ABAF14);
  writeReg(testCPU, 10, 0xF1649BCD);
  add_overflow_neg.checkReg(8, 0xFAFAFAFA);

  mips_error overflowErrorNeg = add_overflow_neg.perform_test(testCPU, testMem);
  int overflowTestNeg =  mips_test_begin_test("add");
  if (overflowErrorNeg == mips_ExceptionArithmeticOverflow){
    mips_test_end_test(overflowTestNeg, 1, "Check for Overflow Exception on adding -ve");
  } else {
    mips_test_end_test(overflowTestNeg, 0, "Check for Overflow Exception on adding -ve");
  }

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

  mips_error lastError = mips_Success;

  int testID = mips_test_begin_test(testName);
  int success = 1;

  // Make the CPU perform the required number of instructions for the test
  for(unsigned i = 0; i < numInstructions; i++){
    mips_error attemptStep = mips_cpu_step(state);
    if(attemptStep != mips_Success){
      lastError = attemptStep;
    }
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

  return lastError;
}

void writeMem(mips_mem_h mem, uint32_t address, uint32_t value){
  mips_mem_write(mem, address, 4, (uint8_t*)&value);
}

void writeReg(mips_cpu_h state, uint8_t index, uint32_t value){
  if (index < 32){
    mips_cpu_set_register(state, index, value);
  } else if (index == 255){
    mips_cpu_set_pc(state, value);
  }
}
