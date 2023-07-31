#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "util.h"

#define LIM 256

typedef struct VM2 {
  u1 m[LIM];  // memory
  u1 c[LIM];  // code
  u1 d[LIM];  // dictionary
  u1 pc;      // program counter
  u1 ci;      // current instruction
  u1 sp;      // stack pointer
  u1 rp;      // return pointer
  u1 a;       // register a
  u1 b;       // register b
  u1 f;       // flags

  // diagnostic only
  u1 dd[LIM]; // dictionary dump
  u1 ppc;     // pre fetch program counter
  u1 log;     // log level, 0 = silent, 1 = on
  u2 t;       // time
} VM2;

void vm2_init(VM2 *);
void vm2_run(VM2 *);
void vm2_print(VM2 *);
void vm2_cycle(VM2 *);
void vm2_fetch(VM2 *);
void vm2_decode(VM2 *);
void vm2_execute(VM2 *);
void vm2_compile(VM2 *, u1 *);
void vm2_dict_app(VM2 *vm, u1 *word, u1 word_n, u1 *code, u1 c);
void vm2_dict_find_idx(VM2 *vm, u1 *word, u1 word_n, u1 *code_i);

void vm2_mc_emit(VM2 *vm);
void vm2_mc_plus(VM2 *vm);
void vm2_mc_slash_mod(VM2 *vm);
void vm2_mc_dup(VM2 *vm);
void vm2_mc_equal(VM2 *vm);
void vm2_mc_drop(VM2 *vm);

void vm2_print_all(VM2 *vm);
void vm2_dict_parse(VM2 *vm);
void vm2_print(VM2 *vm);
void vm2_print_dict_bytes(VM2 *vm);
void vm2_print_dict_list(VM2 *vm);
void vm2_print_dict_dump(VM2 *vm);
void vm2_print_code_bytes(VM2 *vm);
void vm2_print_reg(VM2 *vm);
void vm2_print_mem_bytes(VM2 *vm);
void vm2_print_line_dec(u1 n);
void vm2_print_line_hex(u1 n);

int vm2_cmp(const char *x, u1* y, u1 n);

//void vm2_dict;


void vm2_tests();
void vm2_test_0(VM2 *);
void vm2_test_1(VM2 *);

const char * vm2_ins_str(u1);

// MATH HEADER                          0b0_______
static const u1 OPC_MATH_HEADER_MASK  = 0b10000000;
static const u1 OPC_MATH_HEADER       = 0b00000000;

// MATH OP                              0b_XXXX___
static const u1 OPC_MATH_OP_MASK      = 0b01111000;
static const u1 OPC_MATH_OP_ADD       = 0b00000000;
static const u1 OPC_MATH_OP_SUB       = 0b00001000;
static const u1 OPC_MATH_OP_MUL       = 0b00010000;
static const u1 OPC_MATH_OP_DIV       = 0b00011000;
static const u1 OPC_MATH_OP_SHL       = 0b00100000;
static const u1 OPC_MATH_OP_SHR       = 0b00101000;
static const u1 OPC_MATH_OP_AND       = 0b00110000;
static const u1 OPC_MATH_OP_IOR       = 0b00111000;
static const u1 OPC_MATH_OP_XOR       = 0b01000000;
static const u1 OPC_MATH_OP_NOT       = 0b01001000;
static const u1 OPC_MATH_OP_INC       = 0b01010000;
static const u1 OPC_MATH_OP_DEC       = 0b01011000;

// MATH DST                             0b_____X__
static const u1 OPC_MATH_DST_MASK     = 0b00000100;
static const u1 OPC_MATH_DST_A        = 0b00000000;
static const u1 OPC_MATH_DST_B        = 0b00000100;

// MATH SRC_0                           0b______X_
static const u1 OPC_MATH_SRC_0_MASK   = 0b00000010;
static const u1 OPC_MATH_SRC_0_A      = 0b00000000;
static const u1 OPC_MATH_SRC_0_B      = 0b00000010;

// MATH SRC_1                           0b_______X
static const u1 OPC_MATH_SRC_1_MASK   = 0b00000001;
static const u1 OPC_MATH_SRC_1_A      = 0b00000000;
static const u1 OPC_MATH_SRC_1_B      = 0b00000001;

// MATH INC                             0b_____XXX
static const u1 OPC_MATH_INC_MASK     = 0b00000111;
static const u1 OPC_MATH_INC_A        = 0b00000000;
static const u1 OPC_MATH_INC_B        = 0b00000001;
static const u1 OPC_MATH_INC_S        = 0b00000010;
static const u1 OPC_MATH_INC_R        = 0b00000011;


// MEMORY HEADER                        0b10______
static const u1 OPC_MEMO_HEADER_MASK  = 0b11000000;
static const u1 OPC_MEMO_HEADER       = 0b10000000;

// MEMORY FUN                          0b__X_____
static const u1 OPC_MEMO_FUN_MASK     = 0b00100000;
static const u1 OPC_MEMO_FUN_LOAD     = 0b00000000;
static const u1 OPC_MEMO_FUN_SAVE     = 0b00100000;

// MEMORY DST                          0b___XX___
static const u1 OPC_MEMO_DST_MASK     = 0b00011000;
static const u1 OPC_MEMO_DST_A        = 0b00000000;
static const u1 OPC_MEMO_DST_B        = 0b00001000;
static const u1 OPC_MEMO_DST_S        = 0b00010000;
static const u1 OPC_MEMO_DST_R        = 0b00011000;

// MEMORY SRC                           0b_____XXX
static const u1 OPC_MEMO_SRC_MASK     = 0b00000111;
static const u1 OPC_MEMO_SRC_A        = 0b00000000;
static const u1 OPC_MEMO_SRC_B        = 0b00000001;
static const u1 OPC_MEMO_SRC_S        = 0b00000010;
static const u1 OPC_MEMO_SRC_R        = 0b00000011;
static const u1 OPC_MEMO_SRC_PC       = 0b00000100;
static const u1 OPC_MEMO_SRC_F        = 0b00000101;


// JUMP HEADER                          0b110_____
static const u1 OPC_JUMP_HEADER_MASK  = 0b11100000;
static const u1 OPC_JUMP_HEADER       = 0b11000000;

// JUMP TYPE                            0b___XXXX_
static const u1 OPC_JUMP_TYPE_MASK    = 0b00011110;
static const u1 OPC_JUMP_TYPE_MP      = 0b00000000; // unconditional
static const u1 OPC_JUMP_TYPE_EQ      = 0b00000010; // equal            ==
static const u1 OPC_JUMP_TYPE_NE      = 0b00000100; // not equal        !=
static const u1 OPC_JUMP_TYPE_BL      = 0b00000110; // below            <
static const u1 OPC_JUMP_TYPE_BE      = 0b00000110; // below or equal   <=
static const u1 OPC_JUMP_TYPE_AB      = 0b00001000; // above            >
static const u1 OPC_JUMP_TYPE_AE      = 0b00001010; // above or equal   >=
static const u1 OPC_JUMP_TYPE_OF      = 0b00001100; // overflow         carry == 1
static const u1 OPC_JUMP_TYPE_NO      = 0b00001110; // not overflow     carry == 0

// JUMP REG                             0b_______X
static const u1 OPC_JUMP_REG_MASK     = 0b00000001;
static const u1 OPC_JUMP_REG_A        = 0b00000000;
static const u1 OPC_JUMP_REG_B        = 0b00000001;


// MOVE HEADER                          0b1110____
static const u1 OPC_MOVE_HEADER_MASK  = 0b11110000;
static const u1 OPC_MOVE_HEADER       = 0b11100000;

// MOVE DST                             0b____XX__
static const u1 OPC_MOVE_DST_MASK     = 0b00001100;
static const u1 OPC_MOVE_DST_A        = 0b00000000;
static const u1 OPC_MOVE_DST_B        = 0b00000100;
static const u1 OPC_MOVE_DST_S        = 0b00001000;
static const u1 OPC_MOVE_DST_R        = 0b00001100;

// MOVE SRC                             0b______XX
static const u1 OPC_MOVE_SRC_MASK     = 0b00000011;
static const u1 OPC_MOVE_SRC_A        = 0b00000000;
static const u1 OPC_MOVE_SRC_B        = 0b00000001;
static const u1 OPC_MOVE_SRC_S        = 0b00000010;
static const u1 OPC_MOVE_SRC_R        = 0b00000011;


// SPECIAL HEADER                       0b1111____
static const u1 OPC_SPEC_HEADER_MASK  = 0b11110000;
static const u1 OPC_SPEC_HEADER       = 0b11110000;

// SPECIAL TYPE                         0b____XXXX
static const u1 OPC_SPEC_TYPE_MASK    = 0b00001111;
static const u1 OPC_SPEC_TYPE_NOP     = 0b00000000;
static const u1 OPC_SPEC_TYPE_CMP     = 0b00000001;
static const u1 OPC_SPEC_TYPE_RET     = 0b00000010;
static const u1 OPC_SPEC_TYPE_CAL     = 0b00000011;
static const u1 OPC_SPEC_TYPE_PUT     = 0b00000100;
static const u1 OPC_SPEC_TYPE_MKR     = 0b00000101;
static const u1 OPC_SPEC_TYPE_HLT     = 0b00001111;


// INSTRUCTIONS
// MATH
static const u1 I_ADD_A_A_B = OPC_MATH_HEADER | OPC_MATH_OP_ADD | OPC_MATH_DST_A | OPC_MATH_SRC_0_A | OPC_MATH_SRC_1_B;
static const u1 I_DIV_A_A_B = OPC_MATH_HEADER | OPC_MATH_OP_DIV | OPC_MATH_DST_A | OPC_MATH_SRC_0_A | OPC_MATH_SRC_1_B;
static const u1 I_AND_A_A_B = OPC_MATH_HEADER | OPC_MATH_OP_AND | OPC_MATH_DST_A | OPC_MATH_SRC_0_A | OPC_MATH_SRC_1_B;
static const u1 I_INC_S     = OPC_MATH_HEADER | OPC_MATH_OP_INC | OPC_MATH_INC_S;
static const u1 I_DEC_S     = OPC_MATH_HEADER | OPC_MATH_OP_DEC | OPC_MATH_INC_S;
// MEMORY
static const u1 I_LOAD_A_PC = OPC_MEMO_HEADER | OPC_MEMO_FUN_LOAD | OPC_MEMO_DST_A | OPC_MEMO_SRC_PC;
static const u1 I_LOAD_B_PC = OPC_MEMO_HEADER | OPC_MEMO_FUN_LOAD | OPC_MEMO_DST_B | OPC_MEMO_SRC_PC;
static const u1 I_LOAD_A_S  = OPC_MEMO_HEADER | OPC_MEMO_FUN_LOAD | OPC_MEMO_DST_A | OPC_MEMO_SRC_S;
static const u1 I_LOAD_B_S  = OPC_MEMO_HEADER | OPC_MEMO_FUN_LOAD | OPC_MEMO_DST_B | OPC_MEMO_SRC_S;
static const u1 I_SAVE_S_A  = OPC_MEMO_HEADER | OPC_MEMO_FUN_SAVE | OPC_MEMO_DST_S | OPC_MEMO_SRC_A;
static const u1 I_SAVE_S_B  = OPC_MEMO_HEADER | OPC_MEMO_FUN_SAVE | OPC_MEMO_DST_S | OPC_MEMO_SRC_B;
static const u1 I_SAVE_S_PC = OPC_MEMO_HEADER | OPC_MEMO_FUN_SAVE | OPC_MEMO_DST_S | OPC_MEMO_SRC_PC;
static const u1 I_SAVE_S_F  = OPC_MEMO_HEADER | OPC_MEMO_FUN_SAVE | OPC_MEMO_DST_S | OPC_MEMO_SRC_F;
// JUMP
static const u1 I_JMP = OPC_JUMP_HEADER | OPC_JUMP_TYPE_MP;
static const u1 I_JEQ = OPC_JUMP_HEADER | OPC_JUMP_TYPE_EQ;
static const u1 I_JNE = OPC_JUMP_HEADER | OPC_JUMP_TYPE_NE;
// SPECIAL
static const u1 I_CMP = OPC_SPEC_HEADER | OPC_SPEC_TYPE_CMP;
static const u1 I_HLT = OPC_SPEC_HEADER | OPC_SPEC_TYPE_HLT;
static const u1 I_NOP = OPC_SPEC_HEADER | OPC_SPEC_TYPE_NOP;
static const u1 I_RET = OPC_SPEC_HEADER | OPC_SPEC_TYPE_RET;
static const u1 I_CAL = OPC_SPEC_HEADER | OPC_SPEC_TYPE_CAL;
static const u1 I_PUT = OPC_SPEC_HEADER | OPC_SPEC_TYPE_PUT;
static const u1 I_MKR = OPC_SPEC_HEADER | OPC_SPEC_TYPE_MKR;


// FLAGS
static const u1 BIT_NEGATIVE  = 0b00000001;
static const u1 BIT_ZERO      = 0b00000010;
static const u1 BIT_CARRY     = 0b00000100;
static const u1 BIT_OVERFLOW  = 0b00001000;


// CODE
static const u1 CODE_SIZE_INDEX   = 0;
static const u1 CODE_NEXT_INDEX   = 1;
static const u1 CODE_ENTRY_INDEX  = 3;
static const u1 CODE_INITIAL_SIZE = 4;

// DICT
static const u1 DICT_COUNT_INDEX  = 0;
static const u1 DICT_SIZE_INDEX   = 1;
static const u1 DICT_TAIL_INDEX   = 2;
static const u1 DICT_FIRST        = 3;

// DICT DUMP
static const u1 DICT_DUMP_COUNT_INDEX     = 0;
static const u1 DICT_DUMP_MAX_KEY_N_INDEX = 1;
static const u1 DICT_DUMP_FIRST           = 2;

static const char * IF    = "IF";
static const char * THEN  = "THEN";
static const char * ELSE  = "ELSE";



