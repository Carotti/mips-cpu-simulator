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

  state->delaySlot = 0;

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
  if (state == NULL || dest == NULL){
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

  if(state->debugLevel >= 1){
    fprintf(state->debugDest, "--------------------------------------\n");
    fprintf(state->debugDest, "About to execute '%c' type instruction\nProgram counter = %d\n", nextInstruction.type, state->pc);
  }

  if(state->debugLevel >= 2){
    fprintf(state->debugDest, "Prior register values: (signed decimal)\n");
    for (unsigned i = 0; i < 32; i++){
      fprintf(state->debugDest, "    R%d\t= %d\n",i, state->registers[i]);
    }
  }

  if (state->delaySlot > 0){
    mips_cpu_set_pc(state, state->delaySlot);
    state->delaySlot = 0;
  } else {
    uint32_t oldPc;
    mips_cpu_get_pc(state, &oldPc);
    mips_cpu_set_pc(state, oldPc + 4);
  }

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

  if(state->debugLevel >= 2){
    fprintf(state->debugDest, "Raw Instruction = %d\n", instrR.data);
    fprintf(state->debugDest, "Source1 = R%d\n", instrR.source1);
    fprintf(state->debugDest, "Source2 = R%d\n", instrR.source2);
    fprintf(state->debugDest, "Destination = R%d\n", instrR.dest);
    fprintf(state->debugDest, "Shift = %d\n", instrR.shift);
    fprintf(state->debugDest, "Function Code = %d\n", instrR.function);
  }

  // The two operands corresponding to source1 and source2
  uint32_t op1 = 0;
  uint32_t op2 = 0;
  mips_cpu_get_register(state, instrR.source1, &op1);
  mips_cpu_get_register(state, instrR.source2, &op2);

  // Signed versions of op1 and op2
  int32_t op1s = op1;
  int32_t op2s = op2;

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
      return mips_cpu_set_register(state, instrR.dest, op2s >> instrR.shift);
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
      return mips_cpu_set_register(state, instrR.dest, op2s >> op1);
      break;
    case 8:
      // jr
      state->delaySlot = op1;
      return mips_Success;
      break;
    case 9:
      // jalr
      state->delaySlot = op1;
      return mips_cpu_set_register(state, instrR.dest, oldPc + 4);
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
    case 24:
      // mul
      // sign extend the operands then calculate the result
      state->hi = (((int64_t(op1) << 32) >> 32) * ((int64_t(op2) << 32) >> 32)) >> 32;
      state->lo = ((int64_t(op1) << 32) >> 32) * ((int64_t(op2) << 32) >> 32);
      return mips_Success;
      break;
    case 25:
      // multu
      state->hi = (uint64_t(op1) * uint64_t(op2)) >> 32;
      state->lo = uint64_t(op1) * uint64_t(op2);
      return mips_Success;
      break;
    case 26:
      // div
      if (op2 == 0){
        return mips_ExceptionInvalidInstruction;
      }
      state->lo = op1s / op2s;
      state->hi = op1s % op2s;
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
      if(((op1s > 0) && (op2s > 0) && (op1s + op2s <= 0)) ||
        ((op1s < 0) && (op2s < 0) && (op1s + op2s >= 0))){
        // If both operands are +ve and result is -ve
        // OR If both operands are -ve and result is +ve
        return mips_ExceptionArithmeticOverflow;
      }
      return mips_cpu_set_register(state, instrR.dest, op1s + op2s);
      break;
    case 33:
      // addu
      return mips_cpu_set_register(state, instrR.dest, op1 + op2);
      break;
    case 34:
      // sub
      if(((op1s < 0) && (op2s > 0) && (op1s - op2s >= 0)) ||
        ((op1s > 0) && (op2s < 0) && (op1s - op2s <= 0))){
        // If op1 -ve, op2 +ve, result +ve
        // OR If op1 +ve, op2 -ve, result -ve
        return mips_ExceptionArithmeticOverflow;
      }
      return mips_cpu_set_register(state, instrR.dest, op1s - op2s);
      break;
    case 35:
      // subu
      return mips_cpu_set_register(state, instrR.dest, op1 - op2);
      break;
    case 36:
      // and
      return mips_cpu_set_register(state, instrR.dest, op1 & op2);
      break;
    case 37:
      // or
      return mips_cpu_set_register(state, instrR.dest, op1 | op2);
      break;
    case 38:
      // xor
      return mips_cpu_set_register(state, instrR.dest, op1 ^ op2);
      break;
    case 39:
      // nor
      return mips_cpu_set_register(state, instrR.dest, ~(op1 | op2));
      break;
    case 42:
      // slt
      if (op1s < op2s){
        return mips_cpu_set_register(state, instrR.dest, 1);
      } else {
        return mips_cpu_set_register(state, instrR.dest, 0);
      }
      break;
    case 43:
      // sltu
      if (op1 < op2){
        return mips_cpu_set_register(state, instrR.dest, 1);
      } else {
        return mips_cpu_set_register(state, instrR.dest, 0);
      }
      break;
    default:
      // If not any of the above cases, the instruction is invalid
      return mips_ExceptionInvalidInstruction;
  }
}

mips_error exec_j(mips_cpu_h state, instruction_impl &instruction){
  // Create an instance of instruction_impl_j from the raw data from instruction
  instruction_impl_j instrJ = instruction_impl_j(instruction.data);

  if(state->debugLevel >= 2){
    fprintf(state->debugDest, "Raw Instruction = %d\n", instrJ.data);
    fprintf(state->debugDest, "OpCode = %d\n", instrJ.opCode);
    fprintf(state->debugDest, "Address = %d\n", instrJ.address);
  }

  uint32_t oldPc;
  mips_cpu_get_pc(state, &oldPc);

  // Both j and jal instructions do the following
  state->delaySlot = (oldPc & 0xF0000000) | ((instrJ.address & 0x03FFFFFF) << 2);

  if (instrJ.opCode == 3){
    // jal instruction, set return address
    uint32_t oldPc;
    mips_cpu_get_pc(state, &oldPc);
    mips_cpu_set_register(state, 31, oldPc + 4);
  }
  return mips_Success;
}

mips_error exec_i(mips_cpu_h state, instruction_impl &instruction){
  // Create an instance of instruction_impl_i from the raw data from instruction
  instruction_impl_i instrI = instruction_impl_i(instruction.data);
  if(state->debugLevel >= 2){
    fprintf(state->debugDest, "Raw Instruction = %d\n", instrI.data);
    fprintf(state->debugDest, "Source = R%d\n", instrI.source);
    fprintf(state->debugDest, "Destination = R%d\n", instrI.dest);
    fprintf(state->debugDest, "Immediate = %d\n", instrI.immediate);
  }

  // The two operands corresponding to source1 and dest(used less)
  uint32_t op1 = 0;
  uint32_t op2 = 0;
  mips_cpu_get_register(state, instrI.source, &op1);
  mips_cpu_get_register(state, instrI.dest, &op2);

  // Signed version of op1
  int32_t op1s = op1;

  // A signed 32 bit sign extended version of the immediate
  int32_t signExtImmediate = int32_t(instrI.immediate << 16) >> 16;

  // Store the old value of the PC
  uint32_t oldPc;
  mips_cpu_get_pc(state, &oldPc);

  // Calculate the effective address, used for memory instructions
  uint32_t effectiveAddress = op1 + signExtImmediate;

  // Read the word enclosed around the effective address
  uint32_t memRead;
  mips_mem_read(state->mem, (effectiveAddress / 4) * 4, 4, (uint8_t*)&memRead);

  cout << "WORD:: " <<  bitset<32>(memRead) << endl;

  switch(instrI.opCode){
    case 1:
      // bltz, bgez, bltzal OR bgezal
      if (instrI.dest == 0 || instrI.dest == 16){
        // bltz, or bltzal
        if (op1s < 0){
          state->delaySlot = oldPc + (signExtImmediate << 2);
        }
      } else if (instrI.dest == 1 || instrI.dest == 17){
        // bgez or bgezal
        if (op1s >= 0){
          state->delaySlot = oldPc + (signExtImmediate << 2);
        }
      } else {
        return mips_ExceptionInvalidInstruction;
      }
      if(instrI.dest == 16 || instrI.dest == 17){
        // bltzal or bgezal - set R31 unconditionally
        mips_cpu_set_register(state, 31, oldPc + 4);
      }
      return mips_Success;
    case 4:
      // beq
      if (op1 == op2){
        state->delaySlot = oldPc + (signExtImmediate << 2);
      }
      return mips_Success;
      break;
    case 5:
      // bne
      if (op1 != op2){
        state->delaySlot = oldPc + (signExtImmediate << 2);
      }
      return mips_Success;
      break;
    case 6:
      // blez
      if (signed(op1) <= 0){
        state->delaySlot = oldPc + (signExtImmediate << 2);
      }
      return mips_Success;
      break;
    case 7:
      // bgtz
      if (signed(op1) > 0){
        state->delaySlot = oldPc + (signExtImmediate << 2);
      }
      return mips_Success;
      break;
    case 8:
      // addi
      if(((op1s > 0) && (signExtImmediate > 0) && (op1s + signExtImmediate <= 0)) ||
        ((op1s < 0) && (signExtImmediate < 0) && (op1s + signExtImmediate >= 0))){
        // If both operands are +ve and result is -ve
        // OR If both operands are -ve and result is +ve
        return mips_ExceptionArithmeticOverflow;
      }
      return mips_cpu_set_register(state, instrI.dest, op1s + signExtImmediate);
      break;
    case 9:
      // addiu
      return mips_cpu_set_register(state, instrI.dest, op1 + instrI.immediate);
      break;
    case 10:
      // slti
      if (op1s < signExtImmediate){
        return mips_cpu_set_register(state, instrI.dest, 1);
      } else {
        return mips_cpu_set_register(state, instrI.dest, 0);
      }
      break;
    case 11:
      // sltiu
      if (op1 < uint32_t(signExtImmediate)){
        return mips_cpu_set_register(state, instrI.dest, 1);
      } else {
        return mips_cpu_set_register(state, instrI.dest, 0);
      }
      break;
    case 12:
      // andi
      return mips_cpu_set_register(state, instrI.dest, op1 & uint32_t(instrI.immediate));
      break;
    case 13:
      // ori
      return mips_cpu_set_register(state, instrI.dest, op1 | uint32_t(instrI.immediate));
      break;
    case 14:
      // xori
      return mips_cpu_set_register(state, instrI.dest, op1 ^ uint32_t(instrI.immediate));
      break;
    case 15:
      // lui
      return mips_cpu_set_register(state, instrI.dest, uint32_t(instrI.immediate) << 16);
      break;
    case 32:{
      // lb
      // Store the sign extended byte after getting the correct part from the word read from memory
      return mips_cpu_set_register(state, instrI.dest, (int32_t((memRead >> (8 * (3 - (effectiveAddress % 4)))) & 0xFF) << 24) >> 24);
      break;}
    case 33:
      // lh
      if ((effectiveAddress % 2) != 0){
        // Unaligned halfword, return invalid address exception
        return mips_ExceptionInvalidAddress;
      }
      // Store the sign extended half-word after getting the correct part from the word read from memory
      return mips_cpu_set_register(state, instrI.dest, (int32_t((memRead >> (8 * (2 - (effectiveAddress % 4)))) & 0xFFFF) << 16) >> 16);
      break;
    case 34:
      // lwl
      return mips_ErrorNotImplemented;
      break;
    case 35:
      // lw
      return mips_ErrorNotImplemented;
      break;
    case 36:
      // lbu
      return mips_cpu_set_register(state, instrI.dest, (memRead >> (8 * (3 - (effectiveAddress % 4)))) & 0xFF);
      break;
    case 37:
      // lhu
      return mips_ErrorNotImplemented;
      break;
    case 38:
      // lwr
      return mips_ErrorNotImplemented;
      break;
    case 40:
      // sb
      return mips_ErrorNotImplemented;
      break;
    case 41:
      // sh
      return mips_ErrorNotImplemented;
      break;
    case 42:
      // swl
      return mips_ErrorNotImplemented;
      break;
    case 43:
      // sw
      return mips_ErrorNotImplemented;
      break;
    case 46:
      // swr
      return mips_ErrorNotImplemented;
      break;
    default:
      // This path will never be taken..
      return mips_ExceptionInvalidInstruction;
      break;
  }
}
