#include "test_mips.h"

#include <bitset>

int main(){

  mips_test_begin_suite();

  mips_mem_h testMem = mips_mem_create_ram(4096);
  mips_cpu_h testCPU = mips_cpu_create(testMem);

  // Check get and set for R1 to R31
  for (unsigned i = 1; i < 32; i++){
    test cpu_getSetReg("<internal>", "Verify that the value from a register is the same as the value the register was set to", 0);
    writeReg(testCPU, i, 0xABCDEF12);
    cpu_getSetReg.checkReg(i, 0xABCDEF12);
    cpu_getSetReg.perform_test(testCPU, testMem);
  }

  test cpu_getSetPc("<internal>", "Verify that the program counter is the same as it was set to", 0);
  writeReg(testCPU, 255, 404); // Joke: Program Counter not found?
  cpu_getSetPc.checkReg(255, 404);
  cpu_getSetPc.perform_test(testCPU, testMem);

  test set_zero("<internal>", "Verify that R0 = 0 after an attempted set", 0);
  writeReg(testCPU, 0, 0xABCDEF12);
  set_zero.checkReg(0, 0);
  set_zero.perform_test(testCPU, testMem);

  test cpu_reset("<internal>", "Verify all regs and pc set to 0 after reset", 0);
  // Example of using for loops to define a test -- Why it isn't from a file
  for (unsigned i = 0; i < 32; i++){
    // Set the value of each register to its index
    writeReg(testCPU, i, i);
    cpu_reset.checkReg(i, 0);
  }
  mips_cpu_reset(testCPU);
  cpu_reset.perform_test(testCPU, testMem);

  // Check that each combination of registers can be uses as source1, source2
  // and dest - Yes this does 29,791 tests (Uses addu as simple instruction to implement)
  // Could take this approach with every instruction but that would be OTT
  for (unsigned r1 = 1; r1 < 32; r1++){
    for (unsigned r2 = 1; r2 < 32; r2++){
      for (unsigned r3 = 1; r3 < 32; r3 ++){
        // Make sure Source1 and Source 2 are different
        if(r2 != r3){
          test basic_addu("<internal>", "Verify the result of addu with no overflow for each combination of source1, source2 and destination", 1);
          writeMem(testMem, get_pc(testCPU), instruction_impl_r(r2, r3, r1, 0, 33).data);
          writeReg(testCPU, r2, 0x1374BAB3);
          writeReg(testCPU, r3, 0x3A947118);
          basic_addu.checkReg(r1, 0x4E092BCB);
          basic_addu.perform_test(testCPU, testMem);
          // Reset CPU after each test to keep instruction executing in same memory location
          mips_cpu_reset(testCPU);
        }
      }
    }
  }

  test reg_all("<internal>", "Verify that a register can be used as both sources and a destination, all at once", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(8, 8, 8, 0, 33).data);
  writeReg(testCPU, 8, 0x1AFAC3B0);
  reg_all.checkReg(8, 0x35F58760);
  reg_all.perform_test(testCPU, testMem);

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

  test basic_mult("mult", "Verify values of hi and lo after R10 * R11 with R10 and R11 both positive", 5);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 24).data);
  writeMem(testMem, get_pc(testCPU) + 4, instruction_impl_r(0, 0, 8, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8, instruction_impl_r(0, 0, 9, 0, 18).data);
  writeMem(testMem, get_pc(testCPU) + 12, instruction_impl_r(0, 0, 0, 0, 0).data);
  writeMem(testMem, get_pc(testCPU) + 16, instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 10, 0x1ACDC121);
  writeReg(testCPU, 11, 0x22BBC999);
  basic_mult.checkReg(8, 0x03A2FD0E);
  basic_mult.checkReg(9, 0x06B655B9);
  basic_mult.perform_test(testCPU, testMem);

  test mult_negative("mult", "Verify values of hi and lo after R10 * R11", 5);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 24).data);
  writeMem(testMem, get_pc(testCPU) + 4, instruction_impl_r(0, 0, 8, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8, instruction_impl_r(0, 0, 9, 0, 18).data);
  writeMem(testMem, get_pc(testCPU) + 12, instruction_impl_r(0, 0, 0, 0, 0).data);
  writeMem(testMem, get_pc(testCPU) + 16, instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 10, 0x0BABABAB);
  writeReg(testCPU, 11, 0xFEFEFEFE);
  mult_negative.checkReg(8, 0xFFF4489C);
  mult_negative.checkReg(9, 0xE651FDAA);
  mult_negative.perform_test(testCPU, testMem);

  // TODO: Test for divide by 0 error (check spec for behaviour...)
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

  test basic_add("add", "Verify the result of an add where there is no overflow", 1);
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

  mips_error overflowErrorAddPos = add_overflow_pos.perform_test(testCPU, testMem);
  int overflowTestAddPos =  mips_test_begin_test("add");
  if (overflowErrorAddPos == mips_ExceptionArithmeticOverflow){
    mips_test_end_test(overflowTestAddPos, 1, "Check for Overflow Exception on adding +ve");
  } else {
    mips_test_end_test(overflowTestAddPos, 0, "Check for Overflow Exception on adding +ve");
  }

  writeReg(testCPU, 8, 0xFAFAFAFA);
  test add_overflow_neg("add", "Check that dest register doesn't change if negative overflow occurs in add", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 32).data);
  writeReg(testCPU, 9, 0x80ABAF14);
  writeReg(testCPU, 10, 0xF1649BCD);
  add_overflow_neg.checkReg(8, 0xFAFAFAFA);

  mips_error overflowErrorAddNeg = add_overflow_neg.perform_test(testCPU, testMem);
  int overflowTestAddNeg =  mips_test_begin_test("add");
  if (overflowErrorAddNeg == mips_ExceptionArithmeticOverflow){
    mips_test_end_test(overflowTestAddNeg, 1, "Check for Overflow Exception on adding -ve");
  } else {
    mips_test_end_test(overflowTestAddNeg, 0, "Check for Overflow Exception on adding -ve");
  }

  test basic_addu("addu", "Verify the result of addu with no overflow", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 33).data);
  writeReg(testCPU, 9, 0x1374BAB3);
  writeReg(testCPU, 10, 0x3A947118);
  basic_addu.checkReg(8, 0x4E092BCB);
  basic_addu.perform_test(testCPU, testMem);

  test addu_overflow("addu", "Verify that addu result valid even with overflow", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 33).data);
  writeReg(testCPU, 9, 0xF374BAB3);
  addu_overflow.checkReg(8, 0x2E092BCB);

  mips_error overflowErrorAddu = addu_overflow.perform_test(testCPU, testMem);
  int overflowTestAddu = mips_test_begin_test("addu");
  if (overflowErrorAddu == mips_Success){
    // Addu working correctly, doesn't return overflow (or any other) error
    mips_test_end_test(overflowTestAddu, 1, "Check addu overflowing doesn't produce exception");
  } else {
    mips_test_end_test(overflowTestAddu, 0, "Check addu overflowing doesn't produce exception");
  }

  test basic_sub("sub", "Verify the result of a sub where there is no overflow", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 34).data);
  writeReg(testCPU, 9, 0x0BB8BB8F);
  writeReg(testCPU, 10, 0x00AAA1C1);
  basic_sub.checkReg(8, 0xB0E19CE);
  basic_sub.perform_test(testCPU, testMem);

  writeReg(testCPU, 8, 0xFAFAFAFA);
  test sub_overflow_pos("sub", "Check that dest register doesn't change if positive overflow occurs in sub", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 34).data);
  writeReg(testCPU, 9, 0x7A8B8BB1);
  writeReg(testCPU, 10, 0x81649BCD);
  sub_overflow_pos.checkReg(8, 0xFAFAFAFA);

  mips_error overflowErrorSubPos = sub_overflow_pos.perform_test(testCPU, testMem);
  int overflowTestSubPos =  mips_test_begin_test("sub");
  if (overflowErrorSubPos == mips_ExceptionArithmeticOverflow){
    mips_test_end_test(overflowTestSubPos, 1, "Check for Overflow Exception on subing +ve");
  } else {
    mips_test_end_test(overflowTestSubPos, 0, "Check for Overflow Exception on subing +ve");
  }

  writeReg(testCPU, 8, 0xFAFAFAFA);
  test sub_overflow_neg("sub", "Check that dest register doesn't change if negative overflow occurs in sub", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 34).data);
  writeReg(testCPU, 9, 0x80ABAF14);
  writeReg(testCPU, 10, 0x71649BCD);
  sub_overflow_neg.checkReg(8, 0xFAFAFAFA);

  mips_error overflowErrorSubNeg = sub_overflow_neg.perform_test(testCPU, testMem);
  int overflowTestSubNeg =  mips_test_begin_test("sub");
  if (overflowErrorSubNeg == mips_ExceptionArithmeticOverflow){
    mips_test_end_test(overflowTestSubNeg, 1, "Check for Overflow Exception on subing -ve");
  } else {
    mips_test_end_test(overflowTestSubNeg, 0, "Check for Overflow Exception on subing -ve");
  }

  test basic_subu("subu", "Verify the result of subu with no overflow", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 35).data);
  writeReg(testCPU, 9, 0x1374BAB3);
  writeReg(testCPU, 10, 0x3A947118);
  basic_subu.checkReg(8, 0xD8E0499B);
  basic_subu.perform_test(testCPU, testMem);

  test subu_overflow("subu", "Verify that subu result valid even with overflow", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 35).data);
  writeReg(testCPU, 9, 0x7374BAB3);
  writeReg(testCPU, 10, 0xF14A8BC1);
  subu_overflow.checkReg(8, 0x822A2EF2);

  mips_error overflowErrorSubu = subu_overflow.perform_test(testCPU, testMem);
  int overflowTestSubu = mips_test_begin_test("subu");
  if (overflowErrorSubu == mips_Success){
    // Subu working correctly, doesn't return overflow (or any other) error
    mips_test_end_test(overflowTestSubu, 1, "Check subu overflowing doesn't produce exception");
  } else {
    mips_test_end_test(overflowTestSubu, 0, "Check subu overflowing doesn't produce exception");
  }

  test and_basic("and", "Verify the result of R8 = R9 & R10", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 36).data);
  writeReg(testCPU, 9, 0x1337C3D0);
  writeReg(testCPU, 10, 0xBB81A1AC);
  and_basic.checkReg(8, 0x13018180);
  and_basic.perform_test(testCPU, testMem);

  test or_basic("or", "Verify the result of R8 = R9 | R10", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 37).data);
  or_basic.checkReg(8, 0xBBB7E3FC);
  or_basic.perform_test(testCPU, testMem);

  test xor_basic("xor", "Verify the result of R8 = R9 ^ R10", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 38).data);
  xor_basic.checkReg(8, 0xA8B6627C);
  xor_basic.perform_test(testCPU, testMem);

  test slt_basic_unset("slt", "Check that R8 isn't set since R9 > R10 (signed)", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 42).data);
  writeReg(testCPU, 9, 0x03456789);
  writeReg(testCPU, 10, 0xF4F4BEEE);
  writeReg(testCPU, 8, 0x1234ABCD);
  slt_basic_unset.checkReg(8, 0);
  slt_basic_unset.perform_test(testCPU, testMem);

  test sltu_basic("sltu", "Check that R8 is set since R9 < R10 (unsigned)", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 43).data);
  writeReg(testCPU, 8, 0x1234ABCD);
  sltu_basic.checkReg(8, 1);
  sltu_basic.perform_test(testCPU, testMem);

  writeReg(testCPU, 8, 0xBBBBBBBB);

  test slt_basic_set("slt", "Check that R8 is set since R9 < R10 (signed)", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 42).data);
  writeReg(testCPU, 9, 0x08008501);
  writeReg(testCPU, 10, 0x0F00F13D);
  slt_basic_set.checkReg(8, 1);
  slt_basic_set.perform_test(testCPU, testMem);

  mips_cpu_set_debug_level(testCPU, 2, stderr);

  test j_basic("j", "Check that pc has correct value after branch delay slot and the instruction in the branch delay slot is executed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_j(2, 99).data);
  writeMem(testMem, get_pc(testCPU) + 4, instruction_impl_r(9, 0, 8, 0, 32).data);
  writeReg(testCPU, 9, 0xAB);
  // Top 4 bits of new program counter have to be 0, since we have less than 256MB of memory
  j_basic.checkReg(255, 396);
  j_basic.checkReg(8, 0xAB);
  j_basic.perform_test(testCPU, testMem);
  cout << get_pc(testCPU) << endl;

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

  // Assume the test passes
  success = 1;

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
