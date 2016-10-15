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

  state->registers[index] = value;

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

  // Instructions are 4 bytes long
  uint8_t instructionData[4];

  // First step is to retrieve the instruction from memory at program counter
  mips_error attemptRead = mips_mem_read(
    state->mem,
    state->pc,
    4,
    instructionData);

  // If mips_mem_read declares an error, the error is returned
  if(attemptRead != mips_Success){
    return attemptRead;
  }

  uint8_t opCode = instructionData[0] >> 2;

  // u for undefined (or unimplemented) instruction
  char type = 'u';

  // Determine whether the instruction is R, I or J type or undefined
  for (unsigned i = 0; i < unsigned(state->instruction_set.size()); i++){
    // Loop through the instruction set
    if (state->instruction_set[i].opCode == opCode){
      // type of the opCode is found so the instruction is valid
      type = state->instruction_set[i].type;
    }
  }

  instruction_impl nextInstruction = instruction_impl(
    uint32_t((instructionData[0] << 24)|(instructionData[1] << 16)|
      (instructionData[2] << 8)|(instructionData[3])),
    type,
    opCode);

  cout << bitset<32>(nextInstruction.data) << endl;
  cout << unsigned(nextInstruction.opCode) << endl;

  switch(nextInstruction.type){
    case 'r':
      exec_r(state, nextInstruction);
      break;
    case 'j':
      exec_j(state, nextInstruction);
      break;
    case 'i':
      exec_i(state, nextInstruction);
      break;
    case 'u':
      // Instruction is invalid, return with error
      return mips_ExceptionInvalidInstruction;
      break;
  }

  return mips_Success;
}

mips_error exec_r(mips_cpu_h state, instruction_impl &instruction){
  uint8_t source1 = uint8_t((instruction.data & 0x03E00000) >> 21);
  uint8_t source2 = uint8_t((instruction.data & 0x001F0000) >> 16);
  uint8_t dest = uint8_t((instruction.data & 0x0000F800) >> 11);
  uint8_t shift = uint8_t((instruction.data & 0x000007C0) >> 11);
  uint8_t function = uint8_t((instruction.data & 0x0000003F) >> 11);

  // result is used to store what the destination register will be set to
  uint32_t result;


  mips_cpu_set_register(state, dest, result);

  return mips_Success;
}

mips_error exec_j(mips_cpu_h state, instruction_impl &instruction){

  return mips_Success;
}

mips_error exec_i(mips_cpu_h state, instruction_impl &instruction){

  return mips_Success;
}
