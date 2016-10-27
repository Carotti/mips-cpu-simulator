#include <vector>

#include "mips_instruction_cpu.h"

using namespace std;

struct mips_cpu_impl{

  // Constructor function for mips_cpu_impl
  mips_cpu_impl(mips_mem_h memIn):
    mem(memIn),
    debugLevel(0),
    // Default to having no debug destination, as debugLevel defaults to 0
    debugDest(NULL){
      // Define the instruction set for the CPU
      // * denotes an instruction not in the specification

      // sll, srl, sra, sllv, srlv, srav, jr, jalr, syscall*, break*, mfhi,
      // mthi, mflo, mtlo, mult, multu, div, divu, add, addu, sub, subu, and,
      // or, xor, nor*, slt, sltu
      instruction_set.push_back(instruction_impl('r', 0));

      // bltz, bgez, bltzal, bgezal
      instruction_set.push_back(instruction_impl('i', 1));
      instruction_set.push_back(instruction_impl('j', 2)); // j
      instruction_set.push_back(instruction_impl('j', 3)); // jal
      instruction_set.push_back(instruction_impl('i', 4)); // beq
      instruction_set.push_back(instruction_impl('i', 5)); // bne
      instruction_set.push_back(instruction_impl('i', 6)); // blez (dest = 0)
      instruction_set.push_back(instruction_impl('i', 7)); // bgtz (dest = 0)
      instruction_set.push_back(instruction_impl('i', 8)); // addi
      instruction_set.push_back(instruction_impl('i', 9)); // addiu
      instruction_set.push_back(instruction_impl('i', 10)); // slti
      instruction_set.push_back(instruction_impl('i', 11)); // sltiu
      instruction_set.push_back(instruction_impl('i', 12)); // andi
      instruction_set.push_back(instruction_impl('i', 13)); // ori
      instruction_set.push_back(instruction_impl('i', 14)); // xori
      instruction_set.push_back(instruction_impl('i', 15)); // lui

      instruction_set.push_back(instruction_impl('i', 32)); // lb
      instruction_set.push_back(instruction_impl('i', 33)); // lh
      instruction_set.push_back(instruction_impl('i', 34)); // lwl
      instruction_set.push_back(instruction_impl('i', 35)); // lw
      instruction_set.push_back(instruction_impl('i', 36)); // lbu
      instruction_set.push_back(instruction_impl('i', 37)); // lhu
      instruction_set.push_back(instruction_impl('i', 38)); // lwr

      instruction_set.push_back(instruction_impl('i', 40)); // sb
      instruction_set.push_back(instruction_impl('i', 41)); // sh
      instruction_set.push_back(instruction_impl('i', 42)); // swl*
      instruction_set.push_back(instruction_impl('i', 43)); // sw
    }

  uint32_t registers[32];

  uint32_t pcNew;

  uint32_t pc;

  uint32_t hi;
  uint32_t lo;

  mips_mem_h mem;

  unsigned debugLevel;

  FILE* debugDest;

  vector<instruction_impl> instruction_set;

};

// function should execute r instruction (passed by reference) on state
mips_error exec_r(mips_cpu_h state, instruction_impl &instruction);

// function should execute j instruction (passed by reference) on state
mips_error exec_j(mips_cpu_h state, instruction_impl &instruction);

// function should execute i instruction (passed by reference) on state
mips_error exec_i(mips_cpu_h state, instruction_impl &instruction);

void byte_swap(uint32_t & value);
