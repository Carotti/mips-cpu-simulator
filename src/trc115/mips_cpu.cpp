#include"mips_cpu_impl.h"

#include <iostream>
#include <bitset>

// Wherever a handle is passed to a function, it is required to check whether
// the handle is valid otherwise segmentation errors occur (!!)

mips_cpu_h mips_cpu_create(mips_mem_h mem){

  // Memory handle is invalid, return an empty CPU handler, so that the other
  // functions can recognise this CPU is invalid
  if (mem == NULL){
    return NULL;
  }

  // Creates a new instance of mips_cpu_impl with a handle to the memory
  mips_cpu_h newCPU = new mips_cpu_impl(mem);

  // Reset the CPU on creation to set all regs to 0
  mips_cpu_reset(newCPU);

  return newCPU;
}

mips_error mips_cpu_reset(mips_cpu_h state){
  // Resetting the CPU:
  //  - Sets all registers to 0
  //  - Sets the program counter to 0
  //  - Doesn't affect memory

  //
  if (state == NULL){
    return mips_ErrorInvalidHandle;
  }

  for (unsigned i = 0; i < 32; i++){
    state->registers[i] = 0;
  }

  state->pc = 0;

  state->hi = 0;
  state->lo = 0;

  return mips_Success;
}

mips_error mips_cpu_get_register(
  mips_cpu_h state,
  unsigned index,
  uint32_t *value)
{
  if (state == NULL || value == NULL){
    return mips_ErrorInvalidHandle;
  }

  if(index >= 32){
    return mips_ErrorInvalidArgument;
  }

  *value = state->registers[index];

  return mips_Success;
}

mips_error mips_cpu_set_register(
  mips_cpu_h state,
  unsigned index,
  uint32_t value)
{
  if (state == NULL){
    return mips_ErrorInvalidHandle;
  }

  if(index >= 32){
    return mips_ErrorInvalidArgument;
  }

  if(index != 0) {
    // Register 0 is always set to 0, even if you try and set it
    state->registers[index] = value;
  }

  return mips_Success;
}

mips_error mips_cpu_set_pc(mips_cpu_h state, uint32_t pc){
  if (state == NULL){
    return mips_ErrorInvalidHandle;
  }
  state->pc = pc;

  return mips_Success;
}

mips_error mips_cpu_get_pc(mips_cpu_h state, uint32_t *pc){
  if (state == NULL || pc == NULL){
    return mips_ErrorInvalidHandle;
  }
  *pc = state->pc;

  return mips_Success;
}

void mips_cpu_free(mips_cpu_h state){
    // Memory pointed to is only deallocated if not null so it is not
    // necessary to check whether or not state is set to null
    delete state;
}

mips_error mips_cpu_set_debug_level(
  mips_cpu_h state,
  unsigned level,
  FILE *dest)
{
  if (state == NULL){
    return mips_ErrorInvalidHandle;
  }
  state->debugLevel = level;
  state->debugDest = dest;

  return mips_Success;
}

mips_error mips_cpu_step(mips_cpu_h state){
  if (state == NULL){
    return mips_ErrorInvalidHandle;
  }
  // This is where the fun happens!!

  // TODO: Debugging printout.... ughhhh

  // Instructions are 4 bytes long
  uint32_t instructionData;

  // First step is to retrieve the instruction from memory at program counter
  mips_error attemptRead = mips_mem_read(
    state->mem,
    state->pc,
    4,
    (uint8_t*)&instructionData);

  // If mips_mem_read declares an error, the error is returned
  if(attemptRead != mips_Success){
    return attemptRead;
  }

  // u for undefined instruction
  instruction_impl nextInstruction = instruction_impl(instructionData, 'u');

  // Determine whether the instruction is R, I or J type or undefined
  for (unsigned i = 0; i < unsigned(state->instruction_set.size()); i++){
    // Loop through the instruction set
    if (state->instruction_set[i].opCode == nextInstruction.opCode){
      // type of the opCode is found so the instruction is valid
      nextInstruction.type = state->instruction_set[i].type;
    }
  }

  // cout << bitset<32>(nextInstruction.data) << endl;
  // cout << unsigned(nextInstruction.opCode) << endl;

  // advancing the pc now means if there is a branch, subtract 4 from that

  switch(nextInstruction.type){
    case 'r':
      return exec_r(state, nextInstruction);
      break;
    case 'j':
      return exec_j(state, nextInstruction);
      break;
    case 'i':
      return exec_i(state, nextInstruction);
      break;
    case 'u':
      // Instruction is invalid, return with error
      return mips_ExceptionInvalidInstruction;
      break;
    default:
      // If the type is anything other than these, something has gone very wrong
      // in reality, this can never be the case but it removed the warning!
      return mips_ExceptionInvalidInstruction;
  }
}

mips_error exec_r(mips_cpu_h state, instruction_impl &instruction){

  // Create an instance of instruction_impl_r from the raw data from instruction
  instruction_impl_r instrR = instruction_impl_r(instruction.data);

  // The two operands corresponding to source1 and source2
  uint32_t op1 = 0;
  uint32_t op2 = 0;
  mips_cpu_get_register(state, instrR.source1, &op1);
  mips_cpu_get_register(state, instrR.source2, &op2);

  // Advance the program counter
  advance_pc(state, 4);

  uint32_t oldPc;
  mips_cpu_get_pc(state, &oldPc);

  // perform the operation based on the instruction
  switch(instrR.function){
    case 0:
      // sll
      return mips_cpu_set_register(state, instrR.dest, op2 << instrR.shift);
      break;
    case 2:
      // srl
      return mips_cpu_set_register(state, instrR.dest, op2 >> instrR.shift);
      break;
    case 3:
      // sra
      return mips_cpu_set_register(state, instrR.dest, int32_t(op2) >> instrR.shift);
      break;
    case 4:
      // sllv
      return mips_cpu_set_register(state, instrR.dest, op2 << op1);
      break;
    case 6:
      // srlv
      return mips_cpu_set_register(state, instrR.dest, op2 >> op1);
      break;
    case 7:
      // srav
      return mips_cpu_set_register(state, instrR.dest, int32_t(op2) >> op1);
      break;
    case 8:
      // jr
      return mips_cpu_set_pc(state, op1);
      break;
    case 9:
      // jalr
      mips_cpu_set_register(state, instrR.dest, oldPc + 4);
      return mips_cpu_set_pc(state, op1);
      break;
    case 12:
      // syscall
      return mips_ErrorNotImplemented;
      break;
    case 13:
      // break
      return mips_ExceptionBreak;
      break;
    case 16:
      // mfhi
      return mips_cpu_set_register(state, instrR.dest, state->hi);
      break;
    case 17:
      // mthi
      state->hi = op1;
      return mips_Success;
      break;
    case 18:
      // mflo
      return mips_cpu_set_register(state, instrR.dest, state->lo);
      break;
    case 19:
      // mtlo
      state->lo = op1;
      return mips_Success;
      break;
    case 24:{
      int64_t op1s = op1;
      int64_t op2s = op2;
      // mult
      if (op1 & 0x80000000){
        // op1 is negative, do a sign extension
        op1s = 0xFFFFFFFF00000000 | op1;
      }
      if (op2 & 0x80000000){
        // op2 is negative, do a sign extension
        op2s = 0xFFFFFFFF00000000 | op2;
      }
      int64_t result = op1s * op2s;
      state->hi = uint32_t(result >> 32);
      state->lo = uint32_t(result);
      return mips_Success;
      break;}
    case 25:{
      // multu
      uint64_t result = uint64_t(op1) * uint64_t(op2);
      state->hi = uint32_t(result >> 32);
      state->lo = uint32_t(result);
      return mips_Success;
      break;}
    case 26:
      // div
      if (op2 == 0){
        return mips_ExceptionInvalidInstruction;
      }
      state->lo = signed(op1) / signed(op2);
      state->hi = signed(op1) % signed(op2);
      return mips_Success;
      break;
    case 27:
      // divu
      if (op2 == 0){
        return mips_ExceptionInvalidInstruction;
      }
      state->lo = op1 / op2;
      state->hi = op1 % op2;
      return mips_Success;
      break;
    case 32:
      // add
      return mips_ErrorNotImplemented;
      break;
    case 33:
      // addu
      return mips_ErrorNotImplemented;
      break;
    case 34:
      // sub
      return mips_ErrorNotImplemented;
      break;
    case 35:
      // subu
      return mips_ErrorNotImplemented;
      break;
    case 36:
      // and
      return mips_ErrorNotImplemented;
      break;
    case 37:
      // or
      return mips_ErrorNotImplemented;
      break;
    case 38:
      // xor
      return mips_ErrorNotImplemented;
      break;
    case 39:
      // nor
      return mips_ErrorNotImplemented;
      break;
    default:
      // If not any of the above cases, the instruction is invalid
      return mips_ExceptionInvalidInstruction;
  }
}

mips_error exec_j(mips_cpu_h state, instruction_impl &instruction){

  return mips_Success;
}

mips_error exec_i(mips_cpu_h state, instruction_impl &instruction){

  return mips_Success;
}

void advance_pc(mips_cpu_h state, int offset){
  uint32_t oldPc;
  mips_cpu_get_pc(state, &oldPc);

  mips_cpu_set_pc(state, oldPc + offset);
}
