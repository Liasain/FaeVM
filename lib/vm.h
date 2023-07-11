#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#define DEBUG 0

#if defined(DEBUG) && DEBUG > 0
 #define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)
#else
 #define DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#endif

typedef struct VM1 {
  u1  *p;     // main memory pointer
  char *d;    // dictionary
  char *src;  // source string
  u0 si;      // source index
  u1 a;       // general register a
  u1 b;       // general register b
  u1 i;       // current instruction register
  u1 s;       // data stack pointer, increment
  u1 r;       // return stack pointer, decrement
  u1 ll;      // log level
  u2 t;       // time counter
} VM1;

void vm1_init(VM1 *, u1 *, char *, char *);
void vm1_outer(VM1 *);
void vm1_print(VM1 *);
void vm1_src_print(VM1 *);
char * vm1_src_get(VM1 *);
void vm1_mem_print(VM1 *);
void vm1_reg_print(VM1 *);
void vm1_print_line(int);


/* microcode */
void vm1_mc_inc_r(VM1 *);     // r++, shrink return stack by one
void vm1_mc_inc_s(VM1 *);     // s++, grow stack by one
void vm1_mc_inc_t(VM1 *);     // r++, increment timer

void vm1_mc_dec_s(VM1 *);     // s--, shrink stack by one
void vm1_mc_dec_r(VM1 *);     // r--, grow return stack by one

void vm1_mc_add_a_b(VM1 *);   // a += b, add b to a
void vm1_mc_sub_a_a_b(VM1 *); // a = a - b, sub b from a
void vm1_mc_mul_a_a_b(VM1 *); // a = a * b, mul b with a
void vm1_mc_div_a_a_b(VM1 *); // a = a / b, div a by b
void vm1_mc_div_a_b(VM1 *);   // a /= b, b = rem, div and rem

void vm1_mc_cmp_a_0(VM1 *);   // a == 0   set a to 1 if 0 else 0

void vm1_mc_mov_a_b(VM1 *);   // a = b    set a to b
void vm1_mc_mov_a_si(VM1 *);  // a = si   set a to si
void vm1_mc_mov_b_a(VM1 *);   // b = a    set b to a
void vm1_mc_mov_b_10(VM1 *);  // b = 10   set b to 10
void vm1_mc_mov_b_0(VM1 *);   // b = 0    set b to 10
void vm1_mc_mov_si_a(VM1 *);  // si = a   set si to a
void vm1_mc_mov_ll_a(VM1 *);  // ll = a   set ll to a

void vm1_mc_swp_a_b(VM1 *);   // swap a and b

void vm1_mc_rea_a_s(VM1 *);   // p[s] = a, write a to stack
void vm1_mc_rea_a_r(VM1 *);   // p[r] = a, write a to return stack

void vm1_mc_wri_s_a(VM1 *);   // p[s] = a, write a to stack
void vm1_mc_wri_r_a(VM1 *);   // p[r] = a, write a to return stack

void vm1_mc_pri_a(VM1 *);     // putchar(a), write a to stdout
void vm1_mc_pri_b(VM1 *);     // putchar(b), write b to stdout


/* machine code */
void vm1_in_push_s(VM1 *);
void vm1_in_push_r(VM1 *);

void vm1_in_pull_s(VM1 *);
void vm1_in_pull_r(VM1 *);

void vm1_in_add(VM1 *);
void vm1_in_mul_a_a_b(VM1 *);
void vm1_in_div_a_a_b(VM1 *);

void vm1_in_call(VM1 *);

void vm1_in_print(VM1 *);

void vm1_in_cr(VM1 *);

void vm1_in_log(VM1 *);

void vm1_in_nop(VM1 *);

/* other */

void vm1_def(VM1 *);
void vm1_call(VM1 *);
void vm1_ret(VM1 *);

void vm1_execute(VM1 *);

enum vm1_instruction {
  VM1_NOP         = 0x00,
  VM1_PUSH_S      = 0xF1,
  VM1_CALL        = 0xF2,
  VM1_ADD_A_A_B   = 0b00000001,
  VM1_SUB_A_A_B   = 0b00001001,
  VM1_MUL_A_A_B   = 0b00010001,
  VM1_DIV_A_A_B   = 0b00011001,
  VM1_DIV_REM     = 0b00100000,
  VM1_PRINT       = 0x34,
  VM1_CR          = 0x40,
  VM1_LOG         = 0x50,
};

