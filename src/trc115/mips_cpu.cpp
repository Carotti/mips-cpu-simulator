#include"mips_cpu_impl.h"

mips_cpu_h mips_cpu_create(mips_mem_h mem){

  // Creates a new instance of mips_cpu_impl with a handle to the memory
  mips_cpu_h newCPU = new mips_cpu_impl(mem);

  mips_cpu_reset(newCPU);

  return newCPU;
}

mips_error mips_cpu_reset(mips_cpu_h state){
  // Resetting the CPU:
  //  - Sets all registers to 0
  //  - Sets the program counter to 0
  //  - Doesn't affect memory

  for (unsigned int i = 0; i < NUMREGISTERS; i++){
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
  // TODO: Range Checking

  *value = state->registers[index];

  return mips_Success;
}

mips_error mips_cpu_set_register(mips_cpu_h state,
  unsigned index,
  uint32_t value)
{
  // TODO: Range checking

  state->registers[index] = value;

  return mips_Success;
}

mips_error mips_cpu_set_pc(mips_cpu_h state, uint32_t pc){

  state->pc = pc;

  return mips_Success;
}

mips_error mips_cpu_get_pc(mips_cpu_h state, uint32_t *pc){

  *pc = state->pc;

  return mips_Success;
}

void mips_cpu_free(mips_cpu_h state){

    // Memory pointed to is only deallocated if state is not null so it is not
    // necessary to check whether or not state is set to null
    delete state;
}

mips_error mips_cpu_set_debug_level(
  mips_cpu_h state,
  unsigned level,
  FILE *dest)
{
  state->debugLevel = level;
  state->debugDest = dest;

  return mips_Success;
}

mips_error mips_cpu_step(mips_cpu_h state){
  // This is where the fun happens!!
  state = state;

  return mips_Success;
}
