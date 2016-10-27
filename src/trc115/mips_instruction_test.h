#include "mips.h"

// Used to define the generic instruction implementation
struct instruction_impl{
  char type;
  uint8_t opCode;
  // Only used for an 'actual' function rather than instruction set member
  uint32_t data;

  // Constructor where there is actual raw data
  instruction_impl(uint32_t dataIn, char typeIn):
    type(typeIn),
    opCode(uint8_t((dataIn & 0xFC000000) >> 26)),
    data(dataIn){}
};

// Used for R type instructions, inherits the generic function type
struct instruction_impl_r : public instruction_impl{
  uint8_t source1;
  uint8_t source2;
  uint8_t dest;
  uint8_t shift;
  uint8_t function;

  // Constructor specifying each part of the instruction
  instruction_impl_r(
    uint8_t source1In,
    uint8_t source2In,
    uint8_t destIn,
    uint8_t shiftIn,
    uint8_t functionIn):
      instruction_impl(
        uint32_t(
          0x00000000 | ((uint32_t(source1In) & 0x0000001F) << 21) |
          ((uint32_t(source2In) & 0x0000001F) << 16) |
          ((uint32_t(destIn) & 0x0000001F) << 11) |
          ((uint32_t(shiftIn) & 0x0000001F) << 6) |
          (uint32_t(functionIn) & 0x0000003F)),
        'r'),
      source1(source1In),
      source2(source2In),
      dest(destIn),
      shift(shiftIn),
      function(functionIn){}
};

// Used for J type instructions, inherits the generic function type
struct instruction_impl_j: public instruction_impl{
  uint32_t address;

  instruction_impl_j(uint8_t opCodeIn, uint32_t addressIn):
    instruction_impl(uint32_t(((uint32_t(opCodeIn) & 0x0000003F) << 26) | (addressIn & 0x03FFFFFF)), 'j'),
    address(addressIn){}
};

// Used for I type instructions, inherits the generic function type
struct instruction_impl_i: public instruction_impl{
  uint8_t source;
  uint8_t dest;
  uint16_t immediate;

  // Constructor specifying each part of the instruction
  instruction_impl_i(uint8_t opCodeIn, uint8_t sourceIn, uint8_t destIn, uint16_t immediateIn):
    instruction_impl(
      uint32_t(
        0x00000000 | ((uint32_t(opCodeIn) & 0x0000003F) << 26) |
        ((uint32_t(sourceIn) & 0x0000001F) << 21) |
        ((uint32_t(destIn) & 0x0000001F) << 16) |
        (uint32_t(immediateIn))),
      'i'),
    source(sourceIn),
    dest(destIn),
    immediate(immediateIn){}
};
