#include "vm2.h"


void vm2_tests() {
  info("enter");
  VM2 vm;
//  vm2_test_0(&vm);
  vm2_test_1(&vm);
  info("leave");
}

void vm2_test_0(VM2 *vm) {
  assert(vm);
  trace("enter");
  vm2_init(vm);
  u1 code[] = {
    I_NOP,        //  0
    I_LOAD_A_PC,  //  1   1
    1,            //  2
    I_SAVE_S_A,   //  3
    I_INC_S,      //  4
    I_LOAD_A_PC,  //  5   2
    2,            //  6
    I_SAVE_S_A,   //  7
    I_INC_S,      //  8
    I_DEC_S,      //  9   +
    I_LOAD_A_S,   // 10
    I_DEC_S,      // 11
    I_LOAD_B_S,   // 12
    I_ADD_A_A_B,  // 13
    I_SAVE_S_A,   // 14
    I_INC_S,      // 15
  };
  memcpy(vm->c, code, sizeof(code));
  vm2_run(vm);
  assert(vm->sp == 1);
  assert(vm->m[0] == 3);
  trace("leave");
}

void vm2_test_1(VM2 *vm) {
  assert(vm);
  trace("enter");
  vm2_init(vm);
  u1 code[] = {
    I_LOAD_A_PC,  //  00   0  127
    127,          //  01   1
    I_SAVE_S_A,   //  02   2
    I_INC_S,      //  03   3
    I_CAL,        //  04   4  .
    7,            //  05   5
    I_HLT,        //  06   6
//  a: ---------------------[ .
    I_DEC_S,      //  07   7
    I_LOAD_A_S,   //  08   8
    I_LOAD_B_PC,  //  09   9
    10,           //  0A  10
    I_DIV_A_A_B,  //  0B  11
    I_SAVE_S_B,   //  0C  12
    I_INC_S,      //  0D  13
    I_LOAD_B_PC,  //  0E  14
    0,            //  0F  15
    I_CMP,        //  10  16
    I_JEQ,        //  11  17
    23,           //  12  18
    I_SAVE_S_A,   //  13  19
    I_INC_S,      //  14  20
    I_CAL,        //  15  21  .
    7,            //  16  22
//  b:                  
    I_DEC_S,      //  17  23
    I_LOAD_A_S,   //  18  24
    I_LOAD_B_PC,  //  19  25
    '0',          //  1A  26
    I_ADD_A_A_B,  //  1B  27
    I_SAVE_S_A,   //  1C  28
    I_INC_S,      //  1D  29
    I_CAL,        //  1E  30  putchar
    33,           //  1F  31
    I_RET,        //  20  32
//  c: ---------------------[ putchar
    I_DEC_S,      //  21  33
    I_LOAD_A_S,   //  22  34
    I_PUT,        //  23  35
    I_RET,        //  24  36
  };
  memcpy(vm->c, code, sizeof(code));
  vm2_run(vm);
  assert(vm->sp == 0);
  trace("leave");
}

