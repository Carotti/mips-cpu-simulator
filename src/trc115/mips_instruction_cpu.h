#include "mips.h"

// Used to define the generic instruction implementation
struct instruction_impl{
  char type;
  uint8_t opCode;
  // Only used for an 'actual' function rather than instruction set member
  uint32_t data;

  // Constructor for instruction_impl used by the instruction set
  instruction_impl(char typeIn, uint8_t opCodeIn):
    type(typeIn),
    opCode(opCodeIn),
    data(0){}

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

  // Constructor with just the data
  instruction_impl_r(uint32_t dataIn):
    instruction_impl(dataIn, 'r'),
    source1(uint8_t((dataIn & 0x03E00000) >> 21)),
    source2(uint8_t((dataIn & 0x001F0000) >> 16)),
    dest(uint8_t((dataIn & 0x0000F800) >> 11)),
    shift(uint8_t((dataIn & 0x000007C0) >> 6)),
    function(uint8_t(dataIn & 0x0000003F)){}
};

// Used for J type instructions, inherits the generic function type
struct instruction_impl_j: public instruction_impl{
  uint32_t address;

  // Constructor with just the data
  instruction_impl_j(uint32_t dataIn):
    instruction_impl(dataIn, 'j'),
    address(dataIn & 0x03FFFFFF){}
};

// Used for I type instructions, inherits the generic function type
struct instruction_impl_i: public instruction_impl{
  uint8_t source;
  uint8_t dest;
  uint16_t immediate;

  // Constructor with just the data
  instruction_impl_i(uint32_t dataIn):
    instruction_impl(dataIn, 'i'),
    source(uint8_t((dataIn & 0x03E00000) >> 21)),
    dest(uint8_t((dataIn & 0x001F0000) >> 16)),
    immediate(uint16_t((dataIn & 0x0000FFFF))){}
  };
