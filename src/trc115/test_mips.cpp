#include "test_mips.h"

int main(){

  mips_test_begin_suite();

  // Create 512MB of RAM - PHWOAAAR (used to test J and JAL instructions)
  mips_mem_h testMem = mips_mem_create_ram(512000000);
  mips_cpu_h testCPU = mips_cpu_create(testMem);

  // Check get and set for R1 to R31
  for (unsigned i = 1; i < 32; i++){
    test cpu_getSetReg("<internal>", "Verify that the value from a register is "
      "the same as the value the register was set to", 0);
    writeReg(testCPU, i, 0xABCDEF12);
    cpu_getSetReg.checkReg(i, 0xABCDEF12);
    cpu_getSetReg.perform_test(testCPU, testMem);
  }

  test cpu_getSetPc("<internal>", "Verify that the program counter is the same "
    "as it was set to", 0);
  writeReg(testCPU, 255, 404); // Joke: Program Counter not found?
  cpu_getSetPc.checkReg(255, 404);
  cpu_getSetPc.perform_test(testCPU, testMem);

  test set_zero("<internal>", "Verify that R0 = 0 after an attempted set", 0);
  writeReg(testCPU, 0, 0xABCDEF12);
  set_zero.checkReg(0, 0);
  set_zero.perform_test(testCPU, testMem);

  test cpu_reset("<internal>", "Verify all regs and "
    "pc set to 0 after reset", 0);
  // Example of using for loops to define a test -- Why it isn't from a file
  for (unsigned i = 0; i < 32; i++){
    // Set the value of each register to its index
    writeReg(testCPU, i, i);
    cpu_reset.checkReg(i, 0);
  }
  mips_cpu_reset(testCPU);
  cpu_reset.perform_test(testCPU, testMem);

  // Check that each combination of registers can be uses as source1, source2
  // and dest - Yes this does ~28,000 tests (Uses addu as simple instruction
  // to implement)
  // Could take this approach with every instruction but that would be OTT
  // Changed according to formative feedback for speed purposes!
  for (unsigned r1 = 1; r1 < 32; r1 += 7){
    for (unsigned r2 = 1; r2 < 32; r2 += 5){
      for (unsigned r3 = 1; r3 < 32; r3 += 3){
        // Make sure Source1 and Source 2 are different
        if(r2 != r3){
          test basic_addu("<internal>",
            "Verify the result of addu with no overflow for each combination of"
              " source1, source2 and destination", 1);
          writeMem(testMem, get_pc(testCPU),
            instruction_impl_r(r2, r3, r1, 0, 33).data);
          writeReg(testCPU, r2, 0x1374BAB3);
          writeReg(testCPU, r3, 0x3A947118);
          basic_addu.checkReg(r1, 0x4E092BCB);
          basic_addu.perform_test(testCPU, testMem);
          // Reset CPU after each test to keep instruction executing in same
          // memory location
          mips_cpu_reset(testCPU);
        }
      }
    }
  }

  test reg_all("<internal>", "Verify that a register can be used as both "
    "sources and a destination, all at once", 1);
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
  writeReg(testCPU, 9, 0x80309000);
  basic_sra.checkReg(8, 0xFC018480);
  basic_sra.checkReg(9, 0x80309000);
  basic_sra.perform_test(testCPU, testMem);

  test basic_sraPos("sra", "Verify that R8 = R9 arith>> 5 and R9 unchanged when"
    " R9 is positive", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(0, 9, 8, 19, 3).data);
  writeReg(testCPU, 9, 0x0ABA1A34);
  basic_sraPos.checkReg(8, 0x157);
  basic_sraPos.checkReg(9, 0x0ABA1A34);
  basic_sraPos.perform_test(testCPU, testMem);

  test basic_sllv("sllv", "Verify that R8 = R9 << R10 and R9 R10 unchanged", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 9, 8, 0, 4).data);
  writeReg(testCPU, 9, 0xFBABABAB);
  writeReg(testCPU, 10, 5);
  basic_sllv.checkReg(8, 0x75757560);
  basic_sllv.checkReg(9, 0xFBABABAB);
  basic_sllv.checkReg(10, 5);
  basic_sllv.perform_test(testCPU, testMem);

  test basic_srlv("srlv", "Verify that R8 = R9 >> R10 and R9 R10 unchanged", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 9, 8, 0, 6).data);
  writeReg(testCPU, 9, 0xFBABABAB);
  writeReg(testCPU, 10, 5);
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

  test basic_sravPos("srav",
    "verify that R8 = R9 >> R10 and R9 R10 unchanged when R9 is positive", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 9, 8, 0, 7).data);
  writeReg(testCPU, 9, 0x74BABBC1);
  writeReg(testCPU, 10, 9);
  basic_sravPos.checkReg(8, 0x003A5D5D);
  basic_sravPos.checkReg(9, 0x74BABBC1);
  basic_sravPos.checkReg(10, 9);
  basic_sravPos.perform_test(testCPU, testMem);

  test basic_jr("jr", "Verify that pc = R9 and R9 unchanged as well as that the"
    " instruction in the branch delay slot is executed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 0, 0, 0, 8).data);
  // R8 = R9 >> 1 in the branch delay slot
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 9, 8, 1, 2).data);
  writeReg(testCPU, 9, 0x000000A4);
  basic_jr.checkReg(255, 0x000000A4);
  basic_jr.checkReg(8, 0x00000052);
  basic_jr.checkReg(9, 0x000000A4);
  basic_jr.perform_test(testCPU, testMem);

  test basic_jalr("jalr",
    "Verify that pc = R9, R1 = R9 + 8 and R9 unchangedas well as that the"
    " instruction in the branch delay slot is executed and R31 set", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 0, 31, 0, 9).data);
  // R8 = R9 >> 3 in the branch delay slot
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 9, 8, 3, 2).data);
  writeReg(testCPU, 9, 0x000000B4);
  basic_jalr.checkReg(255, 0x000000B4);
  basic_jalr.checkReg(8, 0x00000016);
  basic_jalr.checkReg(31, get_pc(testCPU) + 8);
  basic_jalr.perform_test(testCPU, testMem);

  // nops after since:
  // "Reads of the HI or LO special registers must be separated from
  // subsequent instructions that write to them by two or more other
  // instructions."
  test basic_mthi("mthi", "Checks value from mfhi is same as given in mthi", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(8, 0, 0, 0, 17).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 9, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeMem(testMem, get_pc(testCPU) + 12,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x0BABABAC);
  basic_mthi.checkReg(9, 0x0BABABAC);
  basic_mthi.perform_test(testCPU, testMem);

  test basic_mfhi("mfhi", "Checks value from mfhi is same as given in mthi", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(8, 0, 0, 0, 17).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 9, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeMem(testMem, get_pc(testCPU) + 12,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x1F1C1D1A);
  basic_mfhi.checkReg(9, 0x1F1C1D1A);
  basic_mfhi.perform_test(testCPU, testMem);

  test basic_mtlo("mtlo", "Checks value from mflo is same as given in mtlo", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(8, 0, 0, 0, 17).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 9, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeMem(testMem, get_pc(testCPU) + 12,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x0BABABAC);
  basic_mtlo.checkReg(9, 0x0BABABAC);
  basic_mtlo.perform_test(testCPU, testMem);

  test basic_mflo("mflo", "Checks value from mflo is same as given in mtlo", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(8, 0, 0, 0, 17).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 9, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeMem(testMem, get_pc(testCPU) + 12,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x1F1C1D1A);
  basic_mflo.checkReg(9, 0x1F1C1D1A);
  basic_mflo.perform_test(testCPU, testMem);

  test basic_multu("multu", "Verify values of hi and lo after R10 * R11", 5);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 25).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 8, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8,
    instruction_impl_r(0, 0, 9, 0, 18).data);
  writeMem(testMem, get_pc(testCPU) + 12,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeMem(testMem, get_pc(testCPU) + 16,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 10, 0x0BABABAB);
  writeReg(testCPU, 11, 0xFEFEFEFE);
  basic_multu.checkReg(8, 0x0B9FF447);
  basic_multu.checkReg(9, 0xE651FDAA);
  basic_multu.perform_test(testCPU, testMem);

  test basic_mult("mult", "Verify values of hi and lo after R10 * R11 with R10 "
    "and R11 both positive", 5);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 24).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 8, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8,
    instruction_impl_r(0, 0, 9, 0, 18).data);
  writeMem(testMem, get_pc(testCPU) + 12,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeMem(testMem, get_pc(testCPU) + 16,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 10, 0x1ACDC121);
  writeReg(testCPU, 11, 0x22BBC999);
  basic_mult.checkReg(8, 0x03A2FD0E);
  basic_mult.checkReg(9, 0x06B655B9);
  basic_mult.perform_test(testCPU, testMem);

  test mult_negative("mult", "Verify values of hi and lo after R10 * R11", 5);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 24).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 8, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8,
    instruction_impl_r(0, 0, 9, 0, 18).data);
  writeMem(testMem, get_pc(testCPU) + 12,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeMem(testMem, get_pc(testCPU) + 16,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 10, 0x0BABABAB);
  writeReg(testCPU, 11, 0xFEFEFEFE);
  mult_negative.checkReg(8, 0xFFF4489C);
  mult_negative.checkReg(9, 0xE651FDAA);
  mult_negative.perform_test(testCPU, testMem);

  test basic_divu("divu", "Check hi and lo after R10 / R11", 5);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 27).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 8, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8,
    instruction_impl_r(0, 0, 9, 0, 18).data);
  writeMem(testMem, get_pc(testCPU) + 12,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeMem(testMem, get_pc(testCPU) + 16,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 10, 0xF1489B44);
  writeReg(testCPU, 11, 0x000000A1);
  basic_divu.checkReg(8, 121);
  basic_divu.checkReg(9, 25143275);
  basic_divu.perform_test(testCPU, testMem);

  test basic_div("div", "Check hi and lo after R10 / R11 (signed)", 3);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 26).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 8, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8,
    instruction_impl_r(0, 0, 9, 0, 18).data);
  writeMem(testMem, get_pc(testCPU) + 12,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeMem(testMem, get_pc(testCPU) + 16,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 10, 0xF1489B44);
  writeReg(testCPU, 11, 0x000000A1);
  basic_div.checkReg(8, 0xffffff87);
  basic_div.checkReg(9, 0xffe8999d);
  basic_div.perform_test(testCPU, testMem);

  test div_negop2("div", "Check hi and lo after R10 / R11 (signed) where R11 is"
    " negative", 3);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(10, 11, 0, 0, 26).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 8, 0, 16).data);
  writeMem(testMem, get_pc(testCPU) + 8,
    instruction_impl_r(0, 0, 9, 0, 18).data);
  writeReg(testCPU, 10, 0x0BF9F147);
  writeReg(testCPU, 11, 0xFFFFFF12);
  div_negop2.checkReg(8, 0xB);
  div_negop2.checkReg(9, 0xfff31e2e);
  div_negop2.perform_test(testCPU, testMem);

  test basic_add("add",
    "Verify the result of an add where there is no overflow", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 32).data);
  writeReg(testCPU, 9, 0x0BB8BB8F);
  writeReg(testCPU, 10, 0x00AAA1C1);
  basic_add.checkReg(8, 0xC635D50);
  basic_add.perform_test(testCPU, testMem);

  writeReg(testCPU, 8, 0xFAFAFAFA);
  test add_overflow_pos("add", "Check that dest register doesn't change if "
    "positive overflow occurs in add", 1, mips_ExceptionArithmeticOverflow);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 32).data);
  writeReg(testCPU, 9, 0x7A8B8BB1);
  writeReg(testCPU, 10, 0x71649BCD);
  add_overflow_pos.checkReg(8, 0xFAFAFAFA);
  add_overflow_pos.perform_test(testCPU, testMem);

  writeReg(testCPU, 8, 0xFAFAFAFA);
  test add_overflow_neg("add", "Check that dest register doesn't change if "
    "negative overflow occurs in add", 1, mips_ExceptionArithmeticOverflow);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 32).data);
  writeReg(testCPU, 9, 0x80ABAF14);
  writeReg(testCPU, 10, 0xF1649BCD);
  add_overflow_neg.checkReg(8, 0xFAFAFAFA);
  add_overflow_neg.perform_test(testCPU, testMem);

  test basic_addu("addu", "Verify the result of addu with no overflow", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 33).data);
  writeReg(testCPU, 9, 0x1374BAB3);
  writeReg(testCPU, 10, 0x3A947118);
  basic_addu.checkReg(8, 0x4E092BCB);
  basic_addu.perform_test(testCPU, testMem);

  test addu_overflow("addu",
    "Verify that addu result valid even with overflow", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 33).data);
  writeReg(testCPU, 9, 0xF374BAB3);
  writeReg(testCPU, 10, 0x3A947118);
  addu_overflow.checkReg(8, 0x2E092BCB);
  addu_overflow.perform_test(testCPU, testMem);

  test basic_sub("sub",
    "Verify the result of a sub where there is no overflow", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 34).data);
  writeReg(testCPU, 9, 0x0BB8BB8F);
  writeReg(testCPU, 10, 0x00AAA1C1);
  basic_sub.checkReg(8, 0xB0E19CE);
  basic_sub.perform_test(testCPU, testMem);

  test sub_overflow_pos("sub", "Check that dest register doesn't change if "
    "positive overflow occurs in sub", 1, mips_ExceptionArithmeticOverflow);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 34).data);
  writeReg(testCPU, 8, 0xFAFAFAFA);
  writeReg(testCPU, 9, 0x7A8B8BB1);
  writeReg(testCPU, 10, 0x81649BCD);
  sub_overflow_pos.checkReg(8, 0xFAFAFAFA);
  sub_overflow_pos.perform_test(testCPU, testMem);

  test sub_overflow_neg("sub", "Check that dest register doesn't change if "
    "negative overflow occurs in sub", 1, mips_ExceptionArithmeticOverflow);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 34).data);
  writeReg(testCPU, 8, 0xFAFAFAFA);
  writeReg(testCPU, 9, 0x80ABAF14);
  writeReg(testCPU, 10, 0x71649BCD);
  sub_overflow_neg.checkReg(8, 0xFAFAFAFA);
  sub_overflow_neg.perform_test(testCPU, testMem);

  test basic_subu("subu", "Verify the result of subu with no overflow", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 35).data);
  writeReg(testCPU, 9, 0x1374BAB3);
  writeReg(testCPU, 10, 0x3A947118);
  basic_subu.checkReg(8, 0xD8E0499B);
  basic_subu.perform_test(testCPU, testMem);

  test subu_overflow("subu",
    "Verify that subu result valid even with overflow", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 35).data);
  writeReg(testCPU, 9, 0x7374BAB3);
  writeReg(testCPU, 10, 0xF14A8BC1);
  subu_overflow.checkReg(8, 0x822A2EF2);
  subu_overflow.perform_test(testCPU, testMem);

  test and_basic("and", "Verify the result of R8 = R9 & R10", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 36).data);
  writeReg(testCPU, 9, 0x1337C3D0);
  writeReg(testCPU, 10, 0xBB81A1AC);
  and_basic.checkReg(8, 0x13018180);
  and_basic.perform_test(testCPU, testMem);

  test or_basic("or", "Verify the result of R8 = R9 | R10", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 37).data);
  writeReg(testCPU, 9, 0x1337C3D0);
  writeReg(testCPU, 10, 0xBB81A1AC);
  or_basic.checkReg(8, 0xBBB7E3FC);
  or_basic.perform_test(testCPU, testMem);

  test xor_basic("xor", "Verify the result of R8 = R9 ^ R10", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 38).data);
  writeReg(testCPU, 9, 0x1337C3D0);
  writeReg(testCPU, 10, 0xBB81A1AC);
  xor_basic.checkReg(8, 0xA8B6627C);
  xor_basic.perform_test(testCPU, testMem);

  test slt_basic_unset("slt",
    "Check that R8 isn't set since R9 > R10 (signed)", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 42).data);
  writeReg(testCPU, 8, 0x1234ABCD);
  writeReg(testCPU, 9, 0x03456789);
  writeReg(testCPU, 10, 0xF4F4BEEE);
  slt_basic_unset.checkReg(8, 0);
  slt_basic_unset.perform_test(testCPU, testMem);

  test sltu_basic("sltu", "Check that R8 is set since R9 < R10 (unsigned)", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 43).data);
  writeReg(testCPU, 8, 0x1234ABCD);
  writeReg(testCPU, 9, 0x03456789);
  writeReg(testCPU, 10, 0xF4F4BEEE);
  sltu_basic.checkReg(8, 1);
  sltu_basic.perform_test(testCPU, testMem);

  test slt_basic_set("slt", "Check that R8 is set since R9 < R10 (signed)", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_r(9, 10, 8, 0, 42).data);
  writeReg(testCPU, 8, 0xBBBBBBBB);
  writeReg(testCPU, 9, 0x08008501);
  writeReg(testCPU, 10, 0x0F00F13D);
  slt_basic_set.checkReg(8, 1);
  slt_basic_set.perform_test(testCPU, testMem);

  test j_basic("j", "Check that pc has correct value after branch delay slot "
    "and the instruction in the branch delay slot is executed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_j(2, 99).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(9, 0, 8, 0, 32).data);
  writeReg(testCPU, 9, 0xAB);
  j_basic.checkReg(255, 396);
  j_basic.checkReg(8, 0xAB);
  j_basic.perform_test(testCPU, testMem);

  test j_aligned("j", "Check that the upper 4 bits of the pc remain unchanged",
    2);
  writeReg(testCPU, 255, 0x1ABCDEF4);
  writeMem(testMem, get_pc(testCPU), instruction_impl_j(2, 99).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  j_aligned.checkReg(255, 0x1000018C);
  j_aligned.perform_test(testCPU, testMem);

  test j_alignedWithBranch("j", "Check that the new PC is aligned with the "
    "instruction in the branch delay slot", 2);
  writeReg(testCPU, 255, 0x0FFFFFFC);
  writeMem(testMem, get_pc(testCPU), instruction_impl_j(2, 99).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  j_alignedWithBranch.checkReg(255, 0x1000018C);
  j_alignedWithBranch.perform_test(testCPU, testMem);

  test jal_basic("jal", "Check that pc and link register have correct values "
    "after branch delay slot and the instruction in the branch delay slot is "
    "executed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_j(3, 109).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(9, 0, 8, 0, 32).data);
  writeReg(testCPU, 9, 0xAB);
  jal_basic.checkReg(255, 436);
  jal_basic.checkReg(8, 0xAB);
  jal_basic.checkReg(31, get_pc(testCPU) + 8);
  jal_basic.perform_test(testCPU, testMem);

  test jal_aligned("jal", "Check that the upper 4 bits of the pc remain "
    "unchanged", 2);
  writeReg(testCPU, 255, 0x1ABCDEF4);
  writeMem(testMem, get_pc(testCPU), instruction_impl_j(3, 99).data);
  writeMem(testMem, get_pc(testCPU) + 4, instruction_impl_r(0, 0, 0, 0, 0).data);
  j_aligned.checkReg(255, 0x1000018C);
  j_aligned.perform_test(testCPU, testMem);

  test jal_alignedWithBranch("j", "Check that the new PC is aligned with the "
    "instruction in the branch delay slot", 2);
  writeReg(testCPU, 255, 0x0FFFFFFC);
  writeMem(testMem, get_pc(testCPU), instruction_impl_j(3, 99).data);
  writeMem(testMem, get_pc(testCPU) + 4, instruction_impl_r(0, 0, 0, 0, 0).data);
  jal_alignedWithBranch.checkReg(255, 0x1000018C);
  jal_alignedWithBranch.perform_test(testCPU, testMem);

  test bltz_basic_no("bltz", "Check that branch isn't taken if source register "
    "is equal to 0. Also check that source isn't changed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(1, 8, 0, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0);
  bltz_basic_no.checkReg(8, 0);
  bltz_basic_no.checkReg(255, get_pc(testCPU) + 8);
  bltz_basic_no.perform_test(testCPU, testMem);

  test bltz_basic_noPos("bltz", "Check that branch isn't taken if source "
    "register is > 0. Also check that source isn't changed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(1, 8, 0, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x1349DE40);
  bltz_basic_noPos.checkReg(8, 0x1349DE40);
  bltz_basic_noPos.checkReg(255, get_pc(testCPU) + 8);
  bltz_basic_noPos.perform_test(testCPU, testMem);

  test bltz_basic_yes("bltz", "Check that branch is taken if source register "
    "is < 0. Also check that source isn't changed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(1, 8, 0, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x80FAB131);
  bltz_basic_yes.checkReg(8, 0x80FAB131);
  bltz_basic_yes.checkReg(255, get_pc(testCPU) - 36);
  bltz_basic_yes.perform_test(testCPU, testMem);

  test bgez_basic_yesZero("bgez", "Check that branch is taken if source "
    "register is = 0. Also check that source isn't changed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(1, 8, 1, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0);
  bgez_basic_yesZero.checkReg(8, 0);
  bgez_basic_yesZero.checkReg(255, get_pc(testCPU) - 36);
  bgez_basic_yesZero.perform_test(testCPU, testMem);

  test bgez_basic_yesPos("bgez", "Check that branch is taken if source register"
    " is > 0. Also check that source isn't changed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(1, 8, 1, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x3323);
  bgez_basic_yesPos.checkReg(8, 0x3323);
  bgez_basic_yesPos.checkReg(255, get_pc(testCPU) - 36);
  bgez_basic_yesPos.perform_test(testCPU, testMem);

  test bgez_basic_no("bgez", "Check that branch isn't taken if source register "
    "is < 0. Also check that source isn't changed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(1, 8, 1, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x8349DE40);
  bgez_basic_no.checkReg(8, 0x8349DE40);
  bgez_basic_no.checkReg(255, get_pc(testCPU) + 8);
  bgez_basic_no.perform_test(testCPU, testMem);

  test bltzal_basic_no("bltzal", "Check that branch isn't taken if source "
    "register is equal to 0. Also check that source isn't changed and the "
    "return address is set", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(1, 8, 16, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0);
  bltzal_basic_no.checkReg(8, 0);
  bltzal_basic_no.checkReg(255, get_pc(testCPU) + 8);
  bltzal_basic_no.checkReg(31, get_pc(testCPU) + 8);
  bltzal_basic_no.perform_test(testCPU, testMem);

  test bltzal_basic_noPos("bltzal", "Check that branch isn't taken if source "
    "register is > 0. Also check that source isn't changed and the return "
    "address is set", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(1, 8, 16, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x1349DE40);
  bltzal_basic_noPos.checkReg(8, 0x1349DE40);
  bltzal_basic_noPos.checkReg(255, get_pc(testCPU) + 8);
  bltzal_basic_noPos.checkReg(31, get_pc(testCPU) + 8);
  bltzal_basic_noPos.perform_test(testCPU, testMem);

  test bltzal_basic_yes("bltzal", "Check that branch is taken if source "
    "register is < 0. Also check that source isn't changed and the return "
    "address is set", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(1, 8, 16, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x80FAB131);
  bltzal_basic_yes.checkReg(8, 0x80FAB131);
  bltzal_basic_yes.checkReg(255, get_pc(testCPU) - 36);
  bltzal_basic_yes.checkReg(31, get_pc(testCPU) + 8);
  bltzal_basic_yes.perform_test(testCPU, testMem);

  test bgezal_basic_yesZero("bgezal", "Check that branch is taken if source "
    "register is = 0. Also check that source isn't changed and the return "
    "address is set", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(1, 8, 17, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0);
  bgezal_basic_yesZero.checkReg(8, 0);
  bgezal_basic_yesZero.checkReg(255, get_pc(testCPU) - 36);
  bgezal_basic_yesZero.checkReg(31, get_pc(testCPU) + 8);
  bgezal_basic_yesZero.perform_test(testCPU, testMem);

  test bgezal_basic_yesPos("bgezal", "Check that branch is taken if source "
    "register is > 0. Also check that source isn't changed and the return "
    "address is set", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(1, 8, 17, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x3323);
  bgezal_basic_yesPos.checkReg(8, 0x3323);
  bgezal_basic_yesPos.checkReg(255, get_pc(testCPU) - 36);
  bgezal_basic_yesPos.checkReg(31, get_pc(testCPU) + 8);
  bgezal_basic_yesPos.perform_test(testCPU, testMem);

  test bgezal_basic_no("bgezal", "Check that branch isn't taken if source "
    "register is < 0. Also check that source isn't changed and the return "
    "address is set", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(1, 8, 17, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x8349DE40);
  bgezal_basic_no.checkReg(8, 0x8349DE40);
  bgezal_basic_no.checkReg(255, get_pc(testCPU) + 8);
  bgezal_basic_no.checkReg(31, get_pc(testCPU) + 8);
  bgezal_basic_no.perform_test(testCPU, testMem);

  test beq_no("beq", "Check that branch isn't taken if "
    "source register != destination register. Also check that source, "
    "destination aren't changed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(4, 8, 9, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0xBA11DEEE);
  writeReg(testCPU, 9, 0xEEED11AB);
  beq_no.checkReg(8, 0xBA11DEEE);
  beq_no.checkReg(9, 0xEEED11AB);
  beq_no.checkReg(255, get_pc(testCPU) + 8);
  beq_no.perform_test(testCPU, testMem);

  test bne_yes("bne", "Check that branch is taken if "
    "source register != destination register. Also check that source, "
    "destination aren't changed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(5, 8, 9, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0xBA11DEEE);
  writeReg(testCPU, 9, 0xEEED11AB);
  bne_yes.checkReg(8, 0xBA11DEEE);
  bne_yes.checkReg(9, 0xEEED11AB);
  bne_yes.checkReg(255, get_pc(testCPU) - 36);
  bne_yes.perform_test(testCPU, testMem);

  test beq_yes("beq", "Check that branch is taken if "
    "source register = destination register. Also check that source, "
    "destination aren't changed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(4, 8, 9, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x1EAFEEEE);
  writeReg(testCPU, 9, 0x1EAFEEEE);
  beq_yes.checkReg(8, 0x1EAFEEEE);
  beq_yes.checkReg(9, 0x1EAFEEEE);
  beq_yes.checkReg(255, get_pc(testCPU) - 36);
  beq_yes.perform_test(testCPU, testMem);

  test bne_no("bne", "Check that the branch isn't taken if "
    "source register = destination register. Also check that source, "
    "destination aren't changed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(5, 8, 9, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x1EAFEEEE);
  writeReg(testCPU, 9, 0x1EAFEEEE);
  bne_no.checkReg(8, 0x1EAFEEEE);
  bne_no.checkReg(9, 0x1EAFEEEE);
  bne_no.checkReg(255, get_pc(testCPU) + 8);
  bne_no.perform_test(testCPU, testMem);

  test blez_yes("blez", "Check that the branch is taken if source < 0. Also "
    "check that source isn't changed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(6, 8, 0, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x80009D04);
  blez_yes.checkReg(8, 0x80009D04);
  blez_yes.checkReg(255, get_pc(testCPU) - 36);
  blez_yes.perform_test(testCPU, testMem);

  test blez_yesZero("blez", "Check that the branch is taken if source = 0. "
    "Also check that source isn't changed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(6, 8, 0, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0);
  blez_yesZero.checkReg(8, 0);
  blez_yesZero.checkReg(255, get_pc(testCPU) - 36);
  blez_yesZero.perform_test(testCPU, testMem);

  test blez_no("blez", "Check that the branch isn't taken if source > 0. Also "
    "check that source isn't changed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(6, 8, 0, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x333F125A);
  blez_no.checkReg(8, 0x333F125A);
  blez_no.checkReg(255, get_pc(testCPU) + 8);
  blez_no.perform_test(testCPU, testMem);

  test bgtz_no("bgtz", "Check that the branch isn't taken if source < 0. Also "
    "check that source isn't changed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(7, 8, 0, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x80009D04);
  bgtz_no.checkReg(8, 0x80009D04);
  bgtz_no.checkReg(255, get_pc(testCPU) + 8);
  bgtz_no.perform_test(testCPU, testMem);

  test bgtz_noZero("bgtz", "Check that the branch isn't taken if source = 0. "
    "Also check that source isn't changed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(7, 8, 0, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0);
  bgtz_noZero.checkReg(8, 0);
  bgtz_noZero.checkReg(255, get_pc(testCPU) + 8);
  bgtz_noZero.perform_test(testCPU, testMem);

  test bgtz_yes("bgtz", "Check that the branch is taken if source > 0. Also "
    "check that source isn't changed", 2);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(7, 8, 0, 0xFFF6).data);
  writeMem(testMem, get_pc(testCPU) + 4,
    instruction_impl_r(0, 0, 0, 0, 0).data);
  writeReg(testCPU, 8, 0x333F125A);
  bgtz_yes.checkReg(8, 0x333F125A);
  bgtz_yes.checkReg(255, get_pc(testCPU) - 36);
  bgtz_yes.perform_test(testCPU, testMem);

  test basic_addi("addi", "Verify the result of an addi where there is no "
    "overflow", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(8, 9, 8, 0x034D).data);
  writeReg(testCPU, 9, 0x0BB8BB8F);
  basic_addi.checkReg(8, 0x0BB8BEDC);
  basic_addi.perform_test(testCPU, testMem);

  writeReg(testCPU, 8, 0xFAFAFAFA);
  test addi_overflow_pos("addi", "Check that dest register doesn't change if "
    "positive overflow occurs in addi", 1, mips_ExceptionArithmeticOverflow);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(8, 9, 8, 0x734D).data);
  writeReg(testCPU, 9, 0x7FFFFBCD);
  addi_overflow_pos.checkReg(8, 0xFAFAFAFA);
  addi_overflow_pos.perform_test(testCPU, testMem);

  writeReg(testCPU, 8, 0xFAFAFAFA);
  test addi_overflow_neg("addi", "Check that dest register doesn't change if "
    "negative overflow occurs in addi", 1, mips_ExceptionArithmeticOverflow);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(8, 9, 8, 0x8003).data);
  writeReg(testCPU, 9, 0x80000014);
  addi_overflow_neg.checkReg(8, 0xFAFAFAFA);
  addi_overflow_neg.perform_test(testCPU, testMem);

  test basic_addiu("addiu", "Verify the result of addiu with no overflow", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(9, 9, 8, 0x1437).data);
  writeReg(testCPU, 9, 0x1374BAB3);
  basic_addiu.checkReg(8, 0x1374CEEA);
  basic_addiu.perform_test(testCPU, testMem);

  test addiu_overflow("addiu", "Verify that addiu result valid even with "
    "overflow", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(9, 9, 8, 0xABCD).data);
  writeReg(testCPU, 9, 0xFFFFFAB3);
  addiu_overflow.checkReg(8, 0xFFFFA680);
  addiu_overflow.perform_test(testCPU, testMem);

  test slti_yesNeg("slti", "Verify that R8 is 1 when R9 is less than the "
    "negative immediate", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(10, 9, 8, 0xFFD3).data);
  writeReg(testCPU, 9, 0xF14D3E0B);
  writeReg(testCPU, 8, 2);
  slti_yesNeg.checkReg(8, 1);
  slti_yesNeg.perform_test(testCPU, testMem);

  test slti_noNeg("slti", "Verify that R8 is 0 when R9 is more than the "
    "negative immediate", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(10, 9, 8, 0x80D3).data);
  writeReg(testCPU, 9, 0xFFFFF329);
  writeReg(testCPU, 8, 2);
  slti_noNeg.checkReg(8, 0);
  slti_noNeg.perform_test(testCPU, testMem);

  test slti_yesPos("slti", "Verify that R8 is 1 when R9 is less than the "
    "positive immediate", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(10, 9, 8, 0x74D1).data);
  writeReg(testCPU, 9, 0x63D0);
  writeReg(testCPU, 8, 2);
  slti_yesPos.checkReg(8, 1);
  slti_yesPos.perform_test(testCPU, testMem);

  test slti_noPos("slti", "Verify that R8 is 0 when R9 is more than the "
    "positive immediate", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(10, 9, 8, 0x034C).data);
  writeReg(testCPU, 9, 0x1AAAAAA4);
  writeReg(testCPU, 8, 2);
  slti_noPos.checkReg(8, 0);
  slti_noPos.perform_test(testCPU, testMem);

  test slti_noPosEq("slti", "Verify that R8 is 0 when R9 is equal to the "
    "positive immediate", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(10, 9, 8, 0x3B3B).data);
  writeReg(testCPU, 9, 0x3B3B);
  writeReg(testCPU, 8, 2);
  slti_noPosEq.checkReg(8, 0);
  slti_noPosEq.perform_test(testCPU, testMem);

  test slti_noNegEq("slti", "Verify that R8 is 0 when R9 is equal to the "
    "negative immediate", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(10, 9, 8, 0x8303).data);
  writeReg(testCPU, 9, 0xFFFF8303);
  writeReg(testCPU, 8, 2);
  slti_noNegEq.checkReg(8, 0);
  slti_noNegEq.perform_test(testCPU, testMem);

  test sltiu_yesSmall("sltiu", "Verify that R8 is 1 when R9 is less than the "
    "immediate", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(11, 9, 8, 0x7432).data);
  writeReg(testCPU, 9, 0x6DF0);
  writeReg(testCPU, 8, 2);
  sltiu_yesSmall.checkReg(8, 1);
  sltiu_yesSmall.perform_test(testCPU, testMem);

  test sltiu_yesBig("sltiu", "Verify that R8 is 1 when R9 is less than the "
    "immediate using the fact the immediate is still sign extended", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(11, 9, 8, 0xFF14).data);
  writeReg(testCPU, 9, 0xFFFF7D1b);
  writeReg(testCPU, 8, 2);
  sltiu_yesBig.checkReg(8, 1);
  sltiu_yesBig.perform_test(testCPU, testMem);

  test sltiu_noSmall("sltiu", "Verify that R8 is 0 when R9 is more than the "
    "immediate", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(11, 9, 8, 0x6B1F).data);
  writeReg(testCPU, 9, 0x7BF12734);
  writeReg(testCPU, 8, 2);
  sltiu_noSmall.checkReg(8, 0);
  sltiu_noSmall.perform_test(testCPU, testMem);

  test sltiu_noBig("sltiu", "Verify that R8 is 0 when R9 is more than the "
    "immediate using the fact the immediate is still sign extended", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(11, 9, 8, 0x8932).data);
  writeReg(testCPU, 9, 0xFFFF9632);
  writeReg(testCPU, 8, 2);
  sltiu_noBig.checkReg(8, 0);
  sltiu_noBig.perform_test(testCPU, testMem);

  test basic_andi("andi", "Verify the result of R8 after R9 ANDed with "
    "immediate", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(12, 9, 8, 0xF6D7).data);
  writeReg(testCPU, 9, 0xAB34965B);
  basic_andi.checkReg(8, 0x9653);
  basic_andi.perform_test(testCPU, testMem);

  test basic_ori("ori", "Verify the result of R8 after R9 ORed with "
    "immediate", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(13, 9, 8, 0x2864).data);
  writeReg(testCPU, 9, 0xFABADAB1);
  basic_ori.checkReg(8, 0xFABAFAF5);
  basic_ori.perform_test(testCPU, testMem);

  test basic_xori("xori", "Verify the result of R8 after R9 XORed with "
    "immediate", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(14, 9, 8, 0x31AB).data);
  writeReg(testCPU, 9, 0x1DABAD00);
  basic_xori.checkReg(8, 0x1DAB9CAB);
  basic_xori.perform_test(testCPU, testMem);

  test basic_lui("lui", "Verify that the immediate is placed in the 16 MSBs of "
    "R8", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(15, 0, 8, 0xB153).data);
  writeReg(testCPU, 8, 0xFABFABFA);
  basic_lui.checkReg(8, 0xB1530000);
  basic_lui.perform_test(testCPU, testMem);

  test basic_lb("lb", "Verify that the byte from the effective address is "
    "stored in R8", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(32, 9, 8, 0xFFF2).data);
  writeReg(testCPU, 9, 512);
  writeMem(testMem, 496, 0xABAB1DAB);
  basic_lb.checkReg(8, 0x1D);
  basic_lb.perform_test(testCPU, testMem);

  test lb_signExtend("lb", "Verify that the sign extended byte from the "
    "effective address is stored in R8", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(32, 9, 8, 0xFF8F).data);
  writeReg(testCPU, 9, 616);
  writeMem(testMem, 500, 0x777777F3);
  lb_signExtend.checkReg(8, 0xFFFFFFF3);
  lb_signExtend.perform_test(testCPU, testMem);

  test lb_posOffset("lb", "Verify that the byte from the effective address "
    "(generated with a positive offset) is stored in R8", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(32, 9, 8, 15).data);
  writeReg(testCPU, 9, 640);
  writeMem(testMem, 652, 0xBFBFBF75);
  lb_posOffset.checkReg(8, 0x00000075);
  lb_posOffset.perform_test(testCPU, testMem);

  test basic_lh("lh", "Verify that the half-word from the effective address is "
    "stored in R8", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(33, 9, 8, 0xFFF3).data);
  writeReg(testCPU, 9, 551);
  writeMem(testMem, 536, 0x55557B7B);
  basic_lh.checkReg(8, 0x00007B7B);
  basic_lh.perform_test(testCPU, testMem);

  test lh_signExtend("lh", "Verify that the sign extended half-word from the "
    "effective address is stored in R8", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(33, 9, 8, 0xFFF4).data);
  writeReg(testCPU, 9, 540);
  writeMem(testMem, 528, 0x84F37676);
  lh_signExtend.checkReg(8, 0xFFFF84F3);
  lh_signExtend.perform_test(testCPU, testMem);

  test lh_posOffset("lh", "Verify that the half-word from the effective "
    "address (generated with a positive offset) is stored in R8", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(33, 9, 8, 10).data);
  writeReg(testCPU, 9, 510);
  writeMem(testMem, 520, 0x7BBCC3D0);
  lh_posOffset.checkReg(8, 0x00007BBC);
  lh_posOffset.perform_test(testCPU, testMem);

  test lh_unalignedReg("lh", "Verify that the destination register isn't "
    "changed after an Invalid Alignment Exception", 1,
    mips_ExceptionInvalidAlignment);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(33, 9, 8, 11).data);
  writeReg(testCPU, 9, 520);
  writeReg(testCPU, 8, 0xA4C3B177);
  lh_unalignedReg.checkReg(8, 0xA4C3B177);
  lh_unalignedReg.perform_test(testCPU, testMem);

  test basic_lwl1("lwl", "Verify that the most significant byte of the "
    "unaligned word replaces the most significant byte of R8", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(34, 9, 8, 3).data);
  writeMem(testMem, 600, 0xF13BA4A4);
  writeReg(testCPU, 8, 0x17462538);
  writeReg(testCPU, 9, 600);
  basic_lwl1.checkReg(8, 0xA4462538);
  basic_lwl1.perform_test(testCPU, testMem);

  test basic_lwl2("lwl", "Verify that the 2 most significant bytes of the "
    "unaligned word replaces the 2 most significant bytes of R8", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(34, 9, 8, 2).data);
  writeMem(testMem, 600, 0xF13BA4A4);
  writeReg(testCPU, 8, 0x17462538);
  writeReg(testCPU, 9, 600);
  basic_lwl2.checkReg(8, 0xA4A42538);
  basic_lwl2.perform_test(testCPU, testMem);

  test basic_lw("lw", "Verify that the contents of the aligned word are loaded "
    "into R8", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(35, 9, 8, -3).data);
  writeReg(testCPU, 9, 543);
  writeMem(testMem, 540, 0xBADBADBA);
  basic_lw.checkReg(8, 0xBADBADBA);
  basic_lw.perform_test(testCPU, testMem);

  test lw_unaligned("lw", "Verify an Invalid Aligment error occurs for lw", 1,
    mips_ExceptionInvalidAlignment);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(35, 9, 8, 0).data);
  writeReg(testCPU, 9, 474);
  lw_unaligned.perform_test(testCPU, testMem);

  test basic_lbu("lbu", "Verify that the byte is not sign extended when loaded "
    "into R8", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(36, 9, 8, 24).data);
  writeReg(testCPU, 9, 500);
  writeMem(testMem, 524, 0x83D14751);
  basic_lbu.checkReg(8, 0x00000083);
  basic_lbu.perform_test(testCPU, testMem);

  test basic_lhu("lhu", "Verify that the half-word is not sign extended when "
    "loaded into R8", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(37, 9, 8, 8).data);
  writeReg(testCPU, 9, 540);
  writeMem(testMem, 548, 0x01189998);
  basic_lhu.checkReg(8, 0x00000118);
  basic_lhu.perform_test(testCPU, testMem);

  test lhu_unaligned("lhu", "Verify an Invalid Alignment error occurs for lhu",
    1, mips_ExceptionInvalidAlignment);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(37, 9, 8, 3).data);
  writeReg(testCPU, 9, 574);
  lhu_unaligned.perform_test(testCPU, testMem);

  test basic_lwr1("lwr", "Verify that the least significant byte of an "
    "unaligned word replaces the least significant byte in R8", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(38, 9, 8, 0).data);
  writeMem(testMem, 564, 0x73B04E1C);
  writeReg(testCPU, 8, 0x075936AB);
  writeReg(testCPU, 9, 564);
  basic_lwr1.checkReg(8, 0x07593673);
  basic_lwr1.perform_test(testCPU, testMem);

  test basic_lwr2("lwr", "Verify that the 3 least significant bytes of an "
    "unaligned word replace the 3 least significant bytes in R8", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(38, 9, 8, 2).data);
  writeMem(testMem, 564, 0x73B04E1C);
  writeReg(testCPU, 8, 0x075936AB);
  writeReg(testCPU, 9, 564);
  basic_lwr2.checkReg(8, 0x0773B04E);
  basic_lwr2.perform_test(testCPU, testMem);

  test basic_sb("sb",
    "Verify that the least significant byte of R8 is stored in the effective "
    "address without overwriting the rest of the word in memory", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(40, 9, 8, 3).data);
  writeMem(testMem, 540, 0x1ACD4B3D);
  writeReg(testCPU, 8, 0xABCDEF93);
  writeReg(testCPU, 9, 540);
  basic_sb.checkMem(540, 0x1ACD4B93);
  basic_sb.perform_test(testCPU, testMem);

  test basic_sb1("sb", "With offset 0, verify that the least significant byte "
    "of R8 is stored in the effective address without overwriting the rest of "
    "the word in memory", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(40, 9, 8, 0).data);
  writeMem(testMem, 536, 0xB3310463);
  writeReg(testCPU, 8, 0x123B4582);
  writeReg(testCPU, 9, 536);
  basic_sb1.checkMem(536, 0x82310463);
  basic_sb1.perform_test(testCPU, testMem);

  test basic_sh("sh", "Verify that the least significant half-word of R8 is "
    "stored in the effective address without overwriting the rest of the word "
    "in memory", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(41, 9, 8, 2).data);
  writeMem(testMem, 544, 0x3E459C12);
  writeReg(testCPU, 8, 0xABCD1149);
  writeReg(testCPU, 9, 544);
  basic_sh.checkMem(544, 0x3E451149);
  basic_sh.perform_test(testCPU, testMem);

  test basic_sh1("sh", "With offset 0, verify that the least significant "
    "half-word of R8 is stored in the effective address without overwriting "
    "the rest of the word in memory", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(41, 9, 8, 0).data);
  writeMem(testMem, 548, 0xBB8C13D0);
  writeReg(testCPU, 8, 0x9932B712);
  writeReg(testCPU, 9, 548);
  basic_sh1.checkMem(548, 0xB71213D0);
  basic_sh1.perform_test(testCPU, testMem);

  test basic_sw("sw", "Check that the value from R8 is correctly loaded into "
    "memory", 1);
  writeMem(testMem, get_pc(testCPU), instruction_impl_i(43, 9, 8, 0).data);
  writeMem(testMem, 552, 0xABABABAB);
  writeReg(testCPU, 8, 0x01590624);
  writeReg(testCPU, 9, 552);
  basic_sw.checkMem(552, 0x01590624);
  basic_sw.perform_test(testCPU, testMem);

  // This test ensures that instructions don't just work independantly
  // Expected number of steps is 5496 for 12th fibonacci number
  test fib("<internal>", "Check that the CPU can calculate the 12th fibonacci "
    "number using various instructions", 5496);
  writeMem(testMem, 0x0,	0x27bdffe0); 	// addiu	sp,sp,-32
  writeMem(testMem, 0x4, 0x2c820002); 	// sltiu	v0,a0,2
  writeMem(testMem, 0x8, 0xafb20018); 	// sw	s2,24(sp);
  writeMem(testMem, 0xc, 0xafbf001c); 	// sw	ra,28(sp);
  writeMem(testMem, 0x10, 0xafb10014); 	// sw	s1,20(sp);
  writeMem(testMem, 0x14, 0xafb00010); 	// sw	s0,16(sp);
  writeMem(testMem, 0x18, 0x14400011); 	// bnez	v0,60 <f_fibonacci+0x60>
  writeMem(testMem, 0x1c, 0x00809021); 	// move	s2,a0
  writeMem(testMem, 0x20, 0x00808021); 	// move	s0,a0
  writeMem(testMem, 0x24, 0x00008821); 	// move	s1,zero
  writeMem(testMem, 0x28, 0x2604ffff); 	// addiu	a0,s0,-1
  writeMem(testMem, 0x2c, 0x0c000000); 	// jal	0 <f_fibonacci>
  writeMem(testMem, 0x30, 0x2610fffe); 	// addiu	s0,s0,-2
  writeMem(testMem, 0x34, 0x2e030002); 	// sltiu	v1,s0,2
  writeMem(testMem, 0x38, 0x1060fffb); 	// beqz	v1,28 <f_fibonacci+0x28>
  writeMem(testMem, 0x3c, 0x02228821); 	// addu	s1,s1,v0
  writeMem(testMem, 0x40, 0x32520001); 	// andi	s2,s2,0x1
  writeMem(testMem, 0x44, 0x8fbf001c); 	// lw	ra,28(sp);
  writeMem(testMem, 0x48, 0x02321021); 	// addu	v0,s1,s2
  writeMem(testMem, 0x4c, 0x8fb00010); 	// lw	s0,16(sp);
  writeMem(testMem, 0x50, 0x8fb20018); 	// lw	s2,24(sp);
  writeMem(testMem, 0x54, 0x8fb10014); 	// lw	s1,20(sp);
  writeMem(testMem, 0x58, 0x03e00008); 	// jr	ra
  writeMem(testMem, 0x5c, 0x27bd0020); 	// addiu	sp,sp,32
  writeMem(testMem, 0x60, 0x08000011); 	// j	44 <f_fibonacci+0x44>
  writeMem(testMem, 0x64, 0x00008821); 	// move	s1,zero
  writeReg(testCPU, 31, 0x10000000);
  writeReg(testCPU, 4, 12); // 12th fibonacci number
  writeReg(testCPU, 29, 0x1000); // stack pointer
  fib.checkReg(2, 144); // Expected result is 144
  fib.perform_test(testCPU, testMem);

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

  // Make the CPU perform the required number of instructions for the test
  for(unsigned i = 0; i < numInstructions; i++){
    mips_error attemptStep = mips_cpu_step(state);
    if(attemptStep != mips_Success){
      lastError = attemptStep;
    }
  }

  // Check the state of the mem is as expected afterward
  for(unsigned i = 0; i < unsigned(memCheck.size()); i++){
    uint32_t testValue;
    mips_mem_read(mem, memCheck[i].address, 4, (uint8_t*)&testValue);
    byte_swap_test(testValue);
    if (testValue != memCheck[i].value) {
        fprintf(stderr, "[Test %d] Mem[0x%X] is 0x%X - Expected 0x%X\n",
          testID, memCheck[i].address, testValue, memCheck[i].value);
        success = 0;
    }
  }

  // Check the state of the regs is as expected afterward
  for(unsigned i = 0; i < unsigned(regCheck.size()); i++){
    uint32_t testValue = 0;
    if (regCheck[i].index < 32){
      mips_cpu_get_register(state, unsigned(regCheck[i].index), &testValue);
      if (testValue != regCheck[i].value){
        fprintf(stderr, "[Test %d] Reg[%d] is 0x%X - Expected 0x%X\n",
          testID, regCheck[i].index, testValue, regCheck[i].value);
        success = 0;
      }
    } else if (regCheck[i].index == 255){
      mips_cpu_get_pc(state, &testValue);
      if (testValue != regCheck[i].value){
        fprintf(stderr, "[Test %d] PC is 0x%X - Expected 0x%X\n",
          testID, testValue, regCheck[i].value);
        success = 0;
      }
    }

  }

  if (expectedError != lastError){
    fprintf(stderr, "[Test %d] Returned error 0x%X - Expected error 0x%X\n",
      testID, lastError, expectedError);
    success = 0;
  }

  if (success == 0){
    // Test has failed
    fprintf(stderr, "[Test %d] FAILED\n", testID);
    fprintf(stderr, "[Test %d] '%s' - %s\n", testID, testName, testDescription);
    fprintf(stderr, "-------------------------------------------\n");
  }

  mips_test_end_test(testID, success, testDescription);

  // Reset the state of the CPU after each test to ensure independance
  mips_cpu_reset(state);

  return lastError;
}

void writeMem(mips_mem_h mem, uint32_t address, uint32_t value){
  byte_swap_test(value);
  mips_mem_write(mem, address, 4, (uint8_t*)&value);
}

void writeReg(mips_cpu_h state, uint8_t index, uint32_t value){
  if (index < 32){
    mips_cpu_set_register(state, index, value);
  } else if (index == 255){
    mips_cpu_set_pc(state, value);
  }
}

void byte_swap_test(uint32_t &value){
  value =
    (value<<24) |
    ((value>>8)&0x0000FF00) |
    ((value<<8)&0x00FF0000) |
    (value>>24);
}
