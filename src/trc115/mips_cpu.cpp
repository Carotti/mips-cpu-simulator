#include "../shared/mips.h"

#define NUMREGISTERS 32

using namespace std;

struct mips_cpu_impl{

};

mips_cpu_h mips_cpu_create(mips_mem_h mem){

}

mips_error mips_cpu_reset(mips_cpu_h state){

}

mips_error mips_cpu_get_register(mips_cpu_h state, unsigned index, uint32_t *value){

}

mips_error mips_cpu_set_register(mips_cpu_h state, unsigned index, uint32_t value){

}

mips_error mips_cpu_set_pc(mips_cpu_h state, uint32_t pc){

}

mips_error mips_cpu_get_pc(mips_cpu_h state, uint32_t *pc){

}

mips_error mips_cpu_step(mips_cpu_h state){

}

mips_error mips_cpu_set_debug_level(mips_cpu_h state, unsigned level, FILE *dest){

}

void mips_cpu_free(mips_cpu_h state){

}
