#include "vm.h"


/*
 * instructions
 */

void vm1_in_push_r(VM1 *vm) {
  assert(vm);
  DEBUG_PRINT("enter R = %02hhX, A = %02hhX\n", vm->r, vm->a);
  vm1_mc_wri_r_a(vm);
  vm1_mc_dec_r(vm);
  DEBUG_PRINT("leave R = %02hhX, P[R+1] = %02hhX\n", vm->r, vm->p[vm->r + 1]);
}

void vm1_in_push_s(VM1 *vm) {
  assert(vm);
  DEBUG_PRINT("enter S = %02hhX, A = %02hhX\n", vm->s, vm->a);
  vm1_mc_wri_s_a(vm);
  vm1_mc_inc_s(vm);
  DEBUG_PRINT("leave S = %02hhX, P[S-1] = %02hhX\n", vm->s, vm->p[vm->s - 1]);
}

void vm1_in_pull_r(VM1 *vm) {
  assert(vm);
  DEBUG_PRINT("enter R = %02hhX, P[R+1] = %02hhX\n", vm->r, vm->p[vm->r + 1]);
  vm1_mc_inc_r(vm);
  vm1_mc_rea_a_r(vm);
  DEBUG_PRINT("leave R = %02hhX, A = %02hhX\n", vm->r, vm->a);
}

void vm1_in_pull_s(VM1 *vm) {
  assert(vm);
  DEBUG_PRINT("enter S = %02hhX, P[S-1] = %02hhX\n", vm->s, vm->p[vm->s - 1]);
  vm1_mc_dec_s(vm);
  vm1_mc_rea_a_s(vm);
  DEBUG_PRINT("leave S = %02hhX, A = %02hhX\n", vm->s, vm->a);
}

void vm1_in_add(VM1 *vm) {
  assert(vm);
  DEBUG_PRINT("enter S = %02hhX, P[S-1] = %02hhX, P[S-2] = %02hhX\n",
      vm->s, vm->p[vm->s - 1], vm->p[vm->s - 2]);
  vm1_mc_dec_s(vm);
  vm1_mc_rea_a_s(vm);
  vm1_mc_mov_b_a(vm);
  vm1_mc_dec_s(vm);
  vm1_mc_rea_a_s(vm);
  vm1_mc_add_a_b(vm);
  vm1_mc_wri_s_a(vm);
  vm1_mc_inc_s(vm);
  DEBUG_PRINT("leave S = %02hhX, P[S-1] = %02hhX\n", vm->s, vm->p[vm->s - 1]);
}

void vm1_in_sub_a_a_b(VM1 *vm) {
  assert(vm);
  DEBUG_PRINT("enter\n");
  vm1_in_pull_s(vm);
  vm1_mc_mov_b_a(vm);
  vm1_in_pull_s(vm);
  vm1_mc_sub_a_a_b(vm);
  vm1_in_push_s(vm);
  DEBUG_PRINT("leave\n");
}

void vm1_in_mul_a_a_b(VM1 *vm) {
  assert(vm);
  DEBUG_PRINT("enter\n");
  vm1_in_pull_s(vm);
  vm1_mc_mov_b_a(vm);
  vm1_in_pull_s(vm);
  vm1_mc_mul_a_a_b(vm);
  vm1_in_push_s(vm);
  DEBUG_PRINT("leave\n");
}

void vm1_in_div_a_a_b(VM1 *vm) {
  assert(vm);
  DEBUG_PRINT("enter\n");
  vm1_in_pull_s(vm);
  vm1_mc_mov_b_a(vm);
  vm1_in_pull_s(vm);
  vm1_mc_div_a_a_b(vm);
  vm1_in_push_s(vm);
  DEBUG_PRINT("leave\n");
}

void vm1_in_call(VM1 *vm) {
  assert(vm);
  DEBUG_PRINT("enter\n");
  switch (vm->a) {
    default: printf("vm1_call error (%02hhX) (%.1s)\n", vm->a, &vm->a); break;
  }
  DEBUG_PRINT("leave\n");
}

void vm1_in_print(VM1 *vm) {
  assert(vm);
  DEBUG_PRINT("enter P[S-1] = %02hhX\n", vm->p[vm->s - 1]);
  vm1_in_pull_s(vm);
  vm1_mc_mov_b_10(vm);
  vm1_mc_div_a_b(vm);
  vm1_mc_swp_a_b(vm);
  vm1_in_push_s(vm); // push digit n-1 digit
  vm1_mc_mov_a_b(vm);
  vm1_mc_cmp_a_0(vm);
  if (!vm->a) {
    vm1_mc_mov_a_b(vm);
    vm1_mc_mov_b_10(vm);
    vm1_mc_div_a_b(vm);
    vm1_mc_swp_a_b(vm);
    vm1_in_push_s(vm); // push digit n-2
    vm1_mc_mov_a_b(vm);
    vm1_mc_cmp_a_0(vm);
    if (!vm->a) vm1_mc_pri_b(vm); // print digit 0
    vm1_in_pull_s(vm);
    vm1_mc_pri_a(vm);
  }
  vm1_in_pull_s(vm);
  vm1_mc_pri_a(vm);
  DEBUG_PRINT("leave\n");
}

void vm1_in_log(VM1 *vm) {
  assert(vm);
  vm1_in_pull_s(vm);
  vm1_mc_mov_ll_a(vm);
}

void vm1_in_cr(VM1 *vm) {(void) vm; putchar('\n');}
void vm1_in_nop(VM1 *vm) {(void) vm;}


/*
 * microcode
 */

void vm1_mc_inc_s(VM1 *vm) {
  assert(vm);
  vm->s++;
}
void vm1_mc_inc_r(VM1 *vm) {
  assert(vm);
  vm->r++;
}
void vm1_mc_inc_t(VM1 *vm) {
  assert(vm);
  vm->t++;
}

void vm1_mc_dec_s(VM1 *vm) {
  assert(vm);
  vm->s--;
}
void vm1_mc_dec_r(VM1 *vm) {
  assert(vm);
  vm->r--;
}

void vm1_mc_add_a_b(VM1 *vm) {
  assert(vm);
  vm->a += vm->b;
}
void vm1_mc_sub_a_a_b(VM1 *vm) {
  assert(vm);
  vm->a = vm->a - vm->b;
}
void vm1_mc_mul_a_a_b(VM1 *vm) {
  assert(vm);
  vm->a = vm->a * vm->b;
}
void vm1_mc_div_a_a_b(VM1 *vm) {
  assert(vm);
  vm->a = vm->a / vm->b;
}
void vm1_mc_div_a_b(VM1 *vm) {
  assert(vm);
  u1 a0 = vm->a;
  vm->a = a0 / vm->b;
  vm->b = a0 % vm->b;
}

void vm1_mc_cmp_a_0(VM1 *vm) {
  assert(vm);
  vm->a = vm->a == 0;
}

void vm1_mc_mov_a_b(VM1 *vm) {
  assert(vm);
  vm->a = vm->b;
}
void vm1_mc_mov_a_si(VM1 *vm) {
  assert(vm);
  vm->a = vm->si;
}
void vm1_mc_mov_b_a(VM1 *vm) {
  assert(vm);
  vm->b = vm->a;
}
void vm1_mc_mov_b_10(VM1 *vm) {
  assert(vm);
  vm->b = 10;
}
void vm1_mc_mov_si_a(VM1 *vm) {
  assert(vm);
  vm->si = vm->a;
}
void vm1_mc_mov_ll_a(VM1 *vm) {
  assert(vm);
  vm->ll = vm->a;
}

void vm1_mc_swp_a_b(VM1 *vm) {
  assert(vm);
  u1 temp = vm->a;
  vm->a = vm->b;
  vm->b = temp;
}

void vm1_mc_rea_a_r(VM1 *vm) {
  assert(vm);
  vm->a = vm->p[vm->r];
}

void vm1_mc_rea_a_s(VM1 *vm) {
  assert(vm);
  vm->a = vm->p[vm->s];
}

void vm1_mc_wri_s_a(VM1 *vm) {
  assert(vm);
  vm->p[vm->s] = vm->a;
}
void vm1_mc_wri_r_a(VM1 *vm) {
  assert(vm);
  vm->p[vm->r] = vm->a;
}

void vm1_mc_pri_a(VM1 *vm) {
  assert(vm);
  putchar(vm->a + '0');
}
void vm1_mc_pri_b(VM1 *vm) {
  assert(vm);
  putchar(vm->b + '0');
}


/*
 * other
 */

void vm1_def(VM1 *vm) {
  assert(vm);
  DEBUG_PRINT("enter SRC = (%s), SI = %ld, D = (%s)\n", vm->src, vm->si, vm->d);
  vm->i = VM1_NOP;
  u1 lo = vm->si - 1;
  while (vm->si < 250 && vm->src[vm->si] != ';') vm->si++;
  vm->si++;
  int j = 0;
  while (vm->d[j] != 0) j++;
  memcpy(vm->d + j, vm->src + lo, vm->si - lo);
  vm->d[j + vm->si - lo] = ' ';
  DEBUG_PRINT("leave SRC = (%s), SI = %ld, D = (%s)\n", vm->src, vm->si, vm->d);
}

void vm1_call(VM1 *vm) {
  assert(vm);
  DEBUG_PRINT("enter R = %02hhX, SI = %ld, \n", vm->r, vm->si);
   int size = 1;
  char *s = vm1_src_get(vm);
  vm1_mc_mov_a_si(vm);
  vm1_in_push_r(vm);
   int n = size + 3;
  char buf[n];
  memset(buf, 0, n);
  memcpy(buf + 2, s + vm->si - 1, size);
  buf[0] = ':';
  buf[1] = ' ';
  char *lo = strstr(vm->d, buf);
  if (!lo) {
    fprintf(stderr, "unknown procedure (%.*s)\n", size, s);
    DEBUG_PRINT("leave R = %02hhX, SI = %ld\n", vm->r, vm->si);
    exit(EXIT_FAILURE);
    return;
  }
  vm->si = lo - vm->d + n - 1;
  DEBUG_PRINT("leave R = %02hhX, SI = %ld\n", vm->r, vm->si);
}

void vm1_ret(VM1 *vm) {
  DEBUG_PRINT("enter R = %02hhX, P[R+1] = %02hhX\n", vm->r, vm->p[vm->r + 1]);
  vm1_in_pull_r(vm);
  vm1_mc_mov_si_a(vm);
  DEBUG_PRINT("leave R = %02hhX, SI = %ld\n", vm->r, vm->si);
}

void vm1_execute(VM1 *vm) {
  assert(vm);
  DEBUG_PRINT("enter\n");
  if (vm->i != VM1_NOP) vm1_print(vm);
  switch (vm->i) {
    case VM1_NOP:       vm1_in_nop(vm);       break;
    case VM1_PUSH_S:    vm1_in_push_s(vm);    break;
    case VM1_ADD_A_A_B: vm1_in_add(vm);       break;
    case VM1_SUB_A_A_B: vm1_in_sub_a_a_b(vm); break;
    case VM1_MUL_A_A_B: vm1_in_mul_a_a_b(vm); break;
    case VM1_DIV_A_A_B: vm1_in_div_a_a_b(vm); break;
    case VM1_CALL:      vm1_in_call(vm);      break;
    case VM1_PRINT:     vm1_in_print(vm);     break;
    case VM1_CR:        vm1_in_cr(vm);        break;
    case VM1_LOG:       vm1_in_log(vm);       break;
    default: printf("logic error: vm1_execute (%02hhX)\n", vm->i); break;
  }
  vm1_mc_inc_t(vm);
  DEBUG_PRINT("leave\n");
}

void vm1_outer(VM1 *vm) {
  assert(vm);
  DEBUG_PRINT("enter\n");
  char *s = vm->src;
  int i = 0;
  while (s[i] != 0) {
    DEBUG_PRINT("enter loop i = %d\n", i);
    vm->i = VM1_NOP;
       while (s[i] == ' ') i++;
    int lo = i;
    while (s[i] != ' ' && s[i] != 0) i++;
    if (i == lo) break;
    u1 ok = 1;
    u1 num = 0;
    for (int j = lo; j < i; j++) {
      if (s[j] < '0' || '9' < s[j]) ok = 0;
      num = num * 10 + s[j] - '0';
    }
    vm->si = i;
    if (!ok) {
      if (i - lo > 1) {
        if (memcmp("CR", s + lo, 2) == 0) {
          vm->i = VM1_CR;
        } else if (memcmp("LOG", s + lo, 2) == 0) {
          vm->i = VM1_LOG;
        } else {
          vm->i = VM1_NOP;
          printf("multi byte word (%.*s) length (%d)\n",
              i - lo, s + lo, i - lo);
        }
      } else if (i - lo == 1) {
        switch (s[lo]) {
          case '+': vm->i = VM1_ADD_A_A_B;  break;
          case '-': vm->i = VM1_SUB_A_A_B;  break;
          case '*': vm->i = VM1_MUL_A_A_B;  break;
          case '/': vm->i = VM1_DIV_A_A_B;  break;
          case '.': vm->i = VM1_PRINT;      break;
          case ':': vm1_def(vm);            break;
          case ';': vm1_ret(vm);            break;
          case '?': vm->ll = 2;            break;
          default:  vm1_call(vm);           break;
        }
      } else {
        printf("error: (%.*s)\n", i - lo, s + lo);
        continue;
      }
      i = vm->si;
      s = vm1_src_get(vm);
    } else {
      vm->a = num;
      vm->i = VM1_PUSH_S;
    }
    vm1_execute(vm);
    DEBUG_PRINT("leave loop i = %d\n", i);
  }
  vm1_print(vm);
  DEBUG_PRINT("leave\n");
}

void vm1_init(VM1 *vm, u1 *p, char *d, char *src) {
  DEBUG_PRINT("enter\n");
  assert(vm);
  assert(p);
  assert(d);
  assert(src);
  memset(p, 0, 256);
  memset(d, 0, 256);
  vm->p = p;
  vm->d = d;
  vm->src = src;
  vm->si = 0;
  vm->ll = 0;
  vm->a = 0x00;
  vm->b = 0x00;
  vm->i = 0x00;
  vm->s = 0x00;
  vm->r = 0xFF;
  vm->t = 0x0000;
  DEBUG_PRINT("leave\n");
}

void vm1_print(VM1 *vm) {
  assert(vm);
  vm1_mem_print(vm);
  vm1_reg_print(vm);
  vm1_src_print(vm);
}

void vm1_mem_print(VM1 *vm) {
  assert(vm);
  if (vm->ll <= 1) return;
  vm1_print_line(54);
  for (int i = 0; i < 16; i++) {
    if (0 < i && i < 15) {
      if (vm->s < i * 16 && vm->r > 0xFF - i * 16) continue;
    }
    printf("%02hhX | ", (u1) (i * 16));
    for (int j = 0; j < 16; j++) {
      printf("%02hhX ", vm->p[i * 16 + j]);
    }
    printf("| ");
    for (int j = 0; j < 16; j++) {
      u1 a = vm->p[i * 16 + j];
      if (a == 0x00) a = '.';
      else if (a < 0x20 || 0x7E < a) a = '#';
      printf("%c", a);
    }
    printf("\n");
  }
  vm1_print_line(54);
}

char * vm1_src_get(VM1 *vm) {
  return (vm->r == 0xFF) ? vm->src : vm->d;
}

void vm1_src_print(VM1 *vm) {
  assert(vm);
  if (vm->ll <= 0) return;
  char *s = vm1_src_get(vm);
  printf("| %s |\n", s);
  putchar('|');
  if (vm->si > 1000) {
    DEBUG_PRINT("logic error si = (%lu)\n", vm->si);
    exit(EXIT_FAILURE);
  }
  for (u0 i = 0; i < vm->si + 1; i++) {putchar(' ');}
  int n = strlen(s);
  putchar('^');
  for (int i = vm->si; i < n; i++) {putchar(' ');}
  printf("|\n");
}

void vm1_reg_print(VM1 *vm) {
  assert(vm);
  if (vm->ll <= 0) return;
  printf("| A %02hhX | B %02hhX | I %02hhX | S %02hhX \
| R %02hhX | X %02hhX | T %5hd |\n",
      vm->a, vm->b, vm->i, vm->s, vm->r, (u1) vm->si, vm->t);
}

void vm1_print_line(int width) {
  printf("+");
  for (int i = 0; i < width - 2; i++) putchar('-');
  printf("+\n");
}

