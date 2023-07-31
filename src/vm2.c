#include "vm2.h"


void vm2_init(VM2 *vm) {
  assert(vm);
  trace("enter");
  memset(vm->m, 0, LIM);
  memset(vm->c, I_HLT, LIM);
  memset(vm->d, 0, LIM);
  vm->pc  = 0x00;   // program counter
  vm->ci  = I_NOP;  // current instruction
  vm->sp  = 0x00;   // stack pointer
  vm->rp  = 0xFF;   // return pointer
  vm->a   = 0x00;   // register a
  vm->b   = 0x00;   // register b
  vm->f   = 0x00;   // flags
  memset(vm->dd, 0, LIM);
  vm->ppc = 0;      // pre fetch program counter
  vm->log = 0;      // log level
  vm->t   = 0;      // time
  trace("leave");
}

void vm2_compile(VM2 *vm, u1 *s) {
  assert(vm);
  assert(s);
  trace("enter, s=(%s)", s);

  vm->c[CODE_SIZE_INDEX] = CODE_INITIAL_SIZE;

  vm->d[DICT_COUNT_INDEX] = 0;
  vm->d[DICT_TAIL_INDEX] = DICT_FIRST;
  
  vm2_mc_emit(vm);
  vm2_mc_plus(vm);
  vm2_mc_slash_mod(vm);
  vm2_mc_dup(vm);
  vm2_mc_equal(vm);
  vm2_mc_drop(vm);

  vm->c[CODE_ENTRY_INDEX] = vm->c[CODE_SIZE_INDEX];
  trace("Code entry address: (%d).", vm->c[CODE_ENTRY_INDEX]);

  u1 colon = 0;

  u1 cond[LIM];
  memset(cond, 0, LIM);
  u1 cond_n = 0;
  u1 * const code_n = vm->c + CODE_SIZE_INDEX;
  u1 * const dict_n = vm->d + DICT_TAIL_INDEX;
  u1 i = 0;
  while (s[i] != 0) {
    while (s[i] == ' ') i++;
    u1 word_0 = i;
    u1 num = 0;
    u1 num_ok = 1;
    while (s[i] != ' ' && s[i] != 0 && s[i] != '\n') {
      u1 d = s[i++];
      if (num_ok) {
        if (d < '0' || '9' < d) {
          num_ok = 0;
        } else {
          num = num * 10 + d - '0';
        }
      } 
    }
    if (word_0 == i) break;
    u1 word_n = i - word_0;

    if (word_n == 1 && s[word_0] == ';') {
      if (!colon) {
        err("; must follow a :");
        exit(EXIT_FAILURE);
      }
      trace("pushing colon code_n=(%d), colon=(%d)",
          *code_n, colon);
      vm->c[(*code_n)++] = I_RET;
      vm->c[colon] = *code_n;
      colon = 0;
      vm2_print_all(vm);
      continue;
    } else if (word_n == 1 && s[word_0] == ':') {
      if (colon) {
        err("No nesting of :");
        exit(EXIT_FAILURE);
      }
      vm->c[(*code_n)++] = I_JMP;
      colon = (*code_n);
      vm->c[(*code_n)++] = I_MKR;
      while (s[i] == ' ') i++;
      word_0 = i;
      while (s[i] != ' ' && s[i] != '\n') i++;
      word_n = i - word_0;
      memcpy(vm->d + *dict_n, s + word_0, word_n);
      (*dict_n) += word_n;
      vm->d[(*dict_n)++] = colon + 1;
      vm->d[(*dict_n)++] = word_n;
      vm->d[DICT_COUNT_INDEX]++;
      vm2_print_all(vm);
      continue;
    } else if (word_n == 2 && vm2_cmp(IF, s + word_0, 2) == 0) {
      if (!colon) {
        err("IF must be between : and ;");
        exit(EXIT_FAILURE);
      }
      assert(cond_n != 0xFF);
      vm->c[(*code_n)++] = I_DEC_S;
      vm->c[(*code_n)++] = I_LOAD_A_S;
      vm->c[(*code_n)++] = I_LOAD_B_PC;
      vm->c[(*code_n)++] = BIT_ZERO;
      vm->c[(*code_n)++] = I_CMP;
      vm->c[(*code_n)++] = I_JNE;
      cond[cond_n++] = (*code_n);
      vm->c[(*code_n)++] = I_MKR;
      vm2_print_all(vm);
      continue;
    } else if (word_n == 4 && vm2_cmp(ELSE, s + word_0, 4) == 0) {
      if (!colon) {
        err("ELSE must be between : and ;");
        exit(EXIT_FAILURE);
      }
      if (!cond_n) {
        err("ELSE must follow IF");
        exit(EXIT_FAILURE);
      }
      vm->c[cond[--cond_n]] = (*code_n) + 2;
      vm->c[(*code_n)++] = I_JMP;
      cond[cond_n++] = (*code_n);
      vm->c[(*code_n)++] = I_MKR;
      vm2_print_all(vm);
      continue;
    } else if (word_n == 4 && vm2_cmp(THEN, s + word_0, 4) == 0) {
      if (!colon) {
        err("THEN must be between : and ;");
        exit(EXIT_FAILURE);
      }
      if (!cond_n) {
        err("THEN must follow IF");
        exit(EXIT_FAILURE);
      }
      vm->c[cond[--cond_n]] = (*code_n);
      vm2_print_all(vm);
      continue;
    }

    trace("Word: (%.*s).", word_n, s + word_0);
    u1 addr = 0;
    vm2_dict_find_idx(vm, s + word_0, word_n, &addr);
    trace("Address search done: addr=(%d)", addr);
    if (addr) {
      vm->c[(*code_n)++] = I_CAL;
      vm->c[(*code_n)++] = addr;
      continue;
    }

    if (!num_ok) {
      vm2_print_all(vm);
      err("Word (%.*s) undefined.", word_n, s + word_0);
      exit(EXIT_FAILURE);
    }

    trace("No matching key, number accepted: num=(%d), word=(%.*s).",
        num, word_n, s + word_0);
    vm->c[(*code_n)++] = I_LOAD_A_PC;
    vm->c[(*code_n)++] = num;
    vm->c[(*code_n)++] = I_SAVE_S_A;
    vm->c[(*code_n)++] = I_INC_S;
    vm2_print_all(vm);
  }

  // cleanup
  vm->c[0] = I_NOP; // c[0], c[1] are unused after compilation,
  vm->c[1] = I_NOP; // and can be used to hook execution.
  vm->c[2] = I_JMP; // Jump unconditionally to [c[3]].
  trace("leave");
  vm2_print_all(vm);
}

void vm2_dict_app(VM2 *vm, u1 *word, u1 word_n, u1 *code, u1 code_n) {
  assert(vm);
  trace("enter, code_n=(%d), count=(%d), word=(%.*s), code=(%d)",
      vm->c[CODE_SIZE_INDEX], vm->d[DICT_COUNT_INDEX], word_n, word, code_n);
  u1 lo_c = vm->c[CODE_SIZE_INDEX];
  u1 lo_d = vm->d[DICT_TAIL_INDEX];
  memcpy(vm->d + lo_d, word, word_n);
  vm->d[lo_d + word_n] = lo_c;
  vm->d[lo_d + word_n + 1] = word_n;
  vm->d[DICT_COUNT_INDEX]++;
  vm->d[DICT_TAIL_INDEX] += word_n + 2;
  memcpy(vm->c + lo_c, code, code_n);
  vm->c[CODE_SIZE_INDEX] += code_n;
  trace("leave, code_n=(%d), count=(%d), last=(%d), tail=(%d)",
      vm->c[CODE_SIZE_INDEX], vm->d[DICT_COUNT_INDEX],
      vm->d[DICT_LAST], vm->d[DICT_TAIL_INDEX]);
}

void vm2_dict_find_idx(VM2 *vm, u1 *word, u1 word_n, u1 *code_i) {
  assert(vm);
  assert(word);
  assert(code_i);
  trace("Enter: word=(%.*s).", word_n, word);
  u1 idx = vm->d[DICT_TAIL_INDEX];
  while (DICT_FIRST < idx) {
    idx--;
    u1 key_n = vm->d[idx];
    if (key_n == 0) {
      vm2_print_all(vm);
      err("Invalid key length: key_n=%d == 0, idx=%d, word=(%.*s).",
          key_n, idx, word_n, word);
      exit(EXIT_FAILURE);
    }
    if (key_n > idx) {
      vm2_print_all(vm);
      err("Invalid key length: key_n=%d > idx=%d, word=(%.*s).",
          key_n, idx, word_n, word);
      exit(EXIT_FAILURE);
    }
    idx -= key_n + 1;
    if (key_n != word_n) {
      trace("Not equal length: key_n=%d != word_n=%d, word=(%.*s).",
          key_n, word_n, word_n, word);
      continue;
    }
    if (memcmp(vm->d + idx, word, word_n) != 0) {
      trace("Key mismatch: word=(%.*s) != key=(%.*s).",
          word_n, word, key_n, vm->d + idx);
      continue;
    }
    u1 addr = vm->d[idx + word_n];
    *code_i = addr;
    trace("Key match; word=(%s), addr=(%d).", word, addr);
  }
}

void vm2_mc_emit(VM2 *vm) {
  assert(vm);
  u1 word[] = "EMIT";
  u1 code[] = {
    I_DEC_S,
    I_LOAD_A_S,
    I_PUT,
    I_RET,
  };
  vm2_dict_app(vm, word, sizeof(word) - 1, code, sizeof(code));
}

void vm2_mc_plus(VM2 *vm) {
  assert(vm);
  u1 word[] = "+";
  u1 code[] = {
    I_DEC_S,
    I_LOAD_A_S,
    I_DEC_S,
    I_LOAD_B_S,
    I_ADD_A_A_B,
    I_SAVE_S_A,
    I_INC_S,
    I_RET,
  };
  vm2_dict_app(vm, word, sizeof(word) - 1, code, sizeof(code));
}

void vm2_mc_slash_mod(VM2 *vm) {
  assert(vm);
  u1 word[] = "/MOD";
  u1 code[] = {
    I_DEC_S,
    I_LOAD_B_S,
    I_DEC_S,
    I_LOAD_A_S,
    I_DIV_A_A_B,
    I_SAVE_S_B,
    I_INC_S,
    I_SAVE_S_A,
    I_INC_S,
    I_RET,
  };
  vm2_dict_app(vm, word, sizeof(word) - 1, code, sizeof(code));
}

void vm2_mc_dup(VM2 *vm) {
  assert(vm);
  u1 word[] = "DUP";
  u1 code[] = {
    I_DEC_S,
    I_LOAD_A_S,
    I_INC_S,
    I_SAVE_S_A,
    I_INC_S,
    I_RET,
  };
  vm2_dict_app(vm, word, sizeof(word) - 1, code, sizeof(code));
}

void vm2_mc_equal(VM2 *vm) {
  assert(vm);
  u1 word[] = "=";
  u1 code[] = {
    I_DEC_S,
    I_LOAD_A_S,
    I_DEC_S,
    I_LOAD_B_S,
    I_CMP,
    I_SAVE_S_F,
    I_LOAD_A_S,
    I_LOAD_B_PC,
    BIT_ZERO,
    I_AND_A_A_B,
    I_SAVE_S_A,
    I_INC_S,
    I_RET,
  };
  vm2_dict_app(vm, word, sizeof(word) - 1, code, sizeof(code));
}

void vm2_mc_drop(VM2 *vm) {
  assert(vm);
  u1 word[] = "DROP";
  u1 code[] = {
    I_DEC_S,
    I_RET,
  };
  vm2_dict_app(vm, word, sizeof(word) - 1, code, sizeof(code));
}

void vm2_run(VM2 *vm) {
  assert(vm);
  trace("enter");
  vm2_print_all(vm);
  while (vm->ci != I_HLT && vm->t < 500) {
    vm2_cycle(vm);
    vm2_print_all(vm);
//    break;
  }
  trace("leave");
}

void vm2_cycle(VM2 *vm) {
  assert(vm);
  trace("enter");
  vm2_fetch(vm);
  vm2_decode(vm);
  vm2_execute(vm);
  trace("leave");
}

void vm2_fetch(VM2 *vm) {
  assert(vm);
  trace("enter, PC = %02hhX", vm->pc);
  vm->ppc = vm->pc; // diagnostic
  vm->ci = vm->c[vm->pc++];
  trace("leave, PC = %02hhX, CI = %02hhX", vm->pc, vm->ci);
}

void vm2_decode(VM2 *vm) {
  assert(vm);
  trace("enter, PC = %02hhX, CI = %02hhX", vm->pc, vm->ci);
  u1 i = vm->ci;
  if ((i & OPC_MATH_HEADER_MASK) == OPC_MATH_HEADER) {
  } else if ((i & OPC_MEMO_HEADER_MASK) == OPC_MEMO_HEADER) {
    switch (i) {
      case I_LOAD_A_PC:
        vm->a = vm->c[vm->pc++];
        vm->t++;
        break;
      case I_LOAD_B_PC:
        vm->b = vm->c[vm->pc++];
        vm->t++;
        break;
      case I_LOAD_A_S:
      case I_LOAD_B_S:
      case I_SAVE_S_A:
      case I_SAVE_S_B:
      case I_SAVE_S_F:
        break;
      default:
        err("memo (%s)", vm2_ins_str(i));
        exit(EXIT_FAILURE);
       break;
    }
  } else if ((i & OPC_JUMP_HEADER_MASK) == OPC_JUMP_HEADER) {
    switch (i) {
      case I_JMP:
      case I_JEQ:
      case I_JNE:
        vm->b = vm->c[vm->pc++];
        vm->t++;
        break;
      default:
        err("jump (%s)", vm2_ins_str(i));
        exit(EXIT_FAILURE);
        break;
    }
  } else if ((i & OPC_MOVE_HEADER_MASK) == OPC_MOVE_HEADER) {
    switch (i) {
      default:
        err("move (%s)", vm2_ins_str(i));
        exit(EXIT_FAILURE);
        break;
    }
  } else if ((i & OPC_SPEC_HEADER_MASK) == OPC_SPEC_HEADER) {
    switch (i) {
      case I_CAL:
        vm->m[vm->rp--] = vm->pc + 1;
        vm->t++;
        break;
      case I_RET:
        vm->rp++;
        break;
      case I_HLT:
        vm->pc--;
        break;
      case I_PUT:
      case I_CMP:
      case I_NOP:
        break;
      default:
        err("spec (%s)", vm2_ins_str(i));
        exit(EXIT_FAILURE);
        break;
    }
  } else {
    err("logic error (%s)", vm2_ins_str(i));
    exit(EXIT_FAILURE);
  }
  trace("leave, PC = %02hhX, CI = %02hhX", vm->pc, vm->ci);
}

void vm2_execute(VM2 *vm) {
  assert(vm);
  trace("enter, CI = %02hhX", vm->ci);
  u1 i = vm->ci;
  u1 temp;
  if ((i & OPC_MATH_HEADER_MASK) == OPC_MATH_HEADER) {
    switch (i) {
      case I_INC_S: vm->sp++; break;
      case I_DEC_S: vm->sp--; break;
      case I_ADD_A_A_B: vm->a = vm->a + vm->b; break;
      case I_DIV_A_A_B:
        temp = vm->a / vm->b;
        vm->b = vm->a % vm->b;
        vm->a = temp;
        temp = vm->a / vm->b;
        break;
      case I_AND_A_A_B: vm->a = vm->a & vm->b; break;
      default:
        err("math unhandled (%s)", vm2_ins_str(i));
        exit(EXIT_FAILURE);
        break;
    }
  } else if ((i & OPC_MEMO_HEADER_MASK) == OPC_MEMO_HEADER) {
    switch (i) {
      case I_LOAD_A_S:  vm->a = vm->m[vm->sp];  break;
      case I_LOAD_B_S:  vm->b = vm->m[vm->sp];  break;
      case I_LOAD_A_PC: /* done in decode */    break;
      case I_LOAD_B_PC: /* done in decode */    break;
      case I_SAVE_S_A:  vm->m[vm->sp] = vm->a;  break;
      case I_SAVE_S_B:  vm->m[vm->sp] = vm->b;  break;
      case I_SAVE_S_F:  vm->m[vm->sp] = vm->f;  break;
      default:
        err("memo unhandled (%s)", vm2_ins_str(i));
        exit(EXIT_FAILURE);
        break;
    }
  } else if ((i & OPC_JUMP_HEADER_MASK) == OPC_JUMP_HEADER) {
    switch (i) {
      case I_JMP:
        vm->pc = vm->b;
        break;
      case I_JEQ:
        if ((vm->f & BIT_ZERO) == BIT_ZERO) vm->pc = vm->b;
        break;
      case I_JNE:
        if ((vm->f & BIT_ZERO) != BIT_ZERO) vm->pc = vm->b;
        break;
      default:
        err("jump unhandled (%s)", vm2_ins_str(i));
        exit(EXIT_FAILURE);
        break;
    }
  } else if ((i & OPC_MOVE_HEADER_MASK) == OPC_MOVE_HEADER) {
    switch (i) {
      default:
        err("move unhandled (%s)", vm2_ins_str(i));
        exit(EXIT_FAILURE);
        break;
    }
  } else if ((i & OPC_SPEC_HEADER_MASK) == OPC_SPEC_HEADER) {
    switch (i) {
      case I_CAL: vm->pc = vm->c[vm->pc]; break;
      case I_PUT: putchar(vm->a);         break;
      case I_RET: vm->pc = vm->m[vm->rp]; break;
      case I_HLT: /* do nothing */        break;
      case I_NOP: /* do nothing */        break;
      case I_CMP:
        vm->f = (vm->a == vm->b ? BIT_ZERO : 0);
        break;
      default:
        err("spec unhandled (%s)", vm2_ins_str(i));
        exit(EXIT_FAILURE);
        break;
    }
  } else {
    err("logic error (%02hhX) (%s)\n", i, vm2_ins_str(i));
    exit(EXIT_FAILURE);
  }
  vm->t++;
  trace("leave");
}

void vm2_print(VM2 *vm) {
  assert(vm);
  if (vm->log == 0) return;
  u0 n_s = 1 << 14;
  char s[n_s];
  memset(s, 0, n_s);
  u0 n = 0;
  for (u1 i = 0; i < 21; i++) {
    if (i != 0) n += sprintf(s + n, " ");
    n += sprintf(s + n, "%2d", i);
  }
  n += sprintf(s + n, "\n");
  for (u1 i = 0; i <= vm->d[DICT_TAIL_INDEX]; i++) {
    if (i) n += sprintf(s + n, " ");
    const char *cc = ANSI_RESET;
    if (i < DICT_FIRST) cc = ANSI_BLUE;
    else if (i == vm->d[DICT_TAIL_INDEX] - 1) cc = ANSI_GREEN;
    else if (i == vm->d[DICT_TAIL_INDEX]) cc = ANSI_YELLOW;
    else if (vm->d[i] == 0) cc = ANSI_RED;
    n += sprintf(s + n, "%s%02hhX%s", cc, vm->d[i],
        ANSI_RESET_ALL);
  }
  u1 hi = 255;
  while (vm->c[hi] == I_HLT) hi--;
  if (hi + 2 > hi) hi = hi + 2;
  n += sprintf(s + n, "\n");
  for (u2 i = 0; i < hi; i++) {
    if (i != 0) n += sprintf(s + n, " ");
    const char *cc = ANSI_RESET;
    if (i == vm->ppc && i != vm->pc) cc = ANSI_GREEN;
    else if (i == vm->pc) cc = ANSI_BLUE;
    else if (vm->c[i] == I_RET) cc = ANSI_RED;
    else if (i == vm->c[CODE_ENTRY_INDEX]) cc = ANSI_YELLOW;
    n += sprintf(s + n, "%s%02hhX", cc, vm->c[i]);
    n += sprintf(s + n, "%s", ANSI_RESET_ALL);
  }
  if (hi == vm->pc) {
    n += sprintf(s + n, " %s%02hhX%s", ANSI_BLUE, vm->c[hi],
        ANSI_RESET_ALL);
  }
  n += sprintf(s + n, "\nSP [%02hhX] (", vm->sp);
  for (u1 i = 0; i < vm->sp; i++) {
    if (i != 0) n += sprintf(s + n, " ");
    n += sprintf(s + n, "%02hhX", vm->m[i]);
  }
  n += sprintf(s + n, ") | RP [%02hhX] (", vm->rp);
  for (u2 i = vm->rp + 1; i < 256; i++) {
    if (i != vm->rp + 1) n += sprintf(s + n, " ");
    n += sprintf(s + n, "%02hhX", vm->m[i]);
  }
  n += sprintf(s + n, ") | %s%s%s |\n",
      ANSI_GREEN, vm2_ins_str(vm->ci), ANSI_RESET_ALL);
  sprintf(s + n,
      "PC %02hhX | CI %02hhX | SP %02hhX | RP %02hhX \
| A %02hhX | B %02hhX | F %02hhX | T %5hd |",
      vm->pc, vm->ci, vm->sp, vm->rp, vm->a, vm->b, vm->f,
      vm->t);
  puts(s);
}

const char * vm2_ins_str(u1 i) {
  switch (i) {
    case I_ADD_A_A_B: return "add a a b";
    case I_DIV_A_A_B: return "div a a b";
    case I_AND_A_A_B: return "and a a b";
    case I_INC_S:     return "inc s";
    case I_DEC_S:     return "dec s";
    case I_LOAD_A_PC: return "load a [pc]";
    case I_LOAD_B_PC: return "load b [pc]";
    case I_LOAD_A_S:  return "load a [s]";
    case I_LOAD_B_S:  return "load b [s]";
    case I_SAVE_S_A:  return "save [s] a";
    case I_SAVE_S_B:  return "save [s] b";
    case I_SAVE_S_PC: return "save [s] pc";
    case I_SAVE_S_F:  return "save [s] f";
    case I_JMP:       return "jmp";
    case I_JEQ:       return "jeq";
    case I_JNE:       return "jne";
    case I_CMP:       return "cmp";
    case I_HLT:       return "hlt";
    case I_NOP:       return "nop";
    case I_RET:       return "ret";
    case I_CAL:       return "cal";
    case I_PUT:       return "putchar";
    default: err("(%02hhX)", i); return "todo";
  }
}

void vm2_print_dict_list(VM2 *vm) {
  assert(vm);
  u1 n = vm->d[DICT_COUNT_INDEX];
  u1 max_key_n = vm->dd[DICT_DUMP_MAX_KEY_N_INDEX];
  printf("%-*s  Type  Addr\n",
      max_key_n, "Name");
  u1 i = DICT_DUMP_FIRST;
  for (u1 j = 0; j < n; j++) {
    u1 key_n = vm->dd[i++];
    u1 key_0 = vm->dd[i++];
    u1 value = vm->dd[i++];
    printf("%-*.*s  FUNC   %02hhX\n",
        max_key_n, key_n, vm->d + key_0, value);
  }
}

void vm2_dict_parse(VM2 *vm) {
  assert(vm);
  trace("enter");
  vm->dd[DICT_DUMP_COUNT_INDEX] = vm->d[DICT_COUNT_INDEX];
  vm->dd[DICT_DUMP_MAX_KEY_N_INDEX] = 4;
  u1 n = vm->d[DICT_COUNT_INDEX];
  u1 i = vm->d[DICT_TAIL_INDEX];
  u1 a_i = DICT_DUMP_FIRST + n * 3;
  for (int j = 0; j < n; j++) {
    u1 key_n = vm->d[--i]; // length
    u1 value = vm->d[--i]; // value
    u1 key_0 = i -= key_n; // addr
    vm->dd[--a_i] = value;
    vm->dd[--a_i] = key_0;
    vm->dd[--a_i] = key_n;
    if (vm->dd[DICT_DUMP_MAX_KEY_N_INDEX] < key_n)
      vm->dd[DICT_DUMP_MAX_KEY_N_INDEX] = key_n;
  }
  trace("leave");
}

void vm2_print_dict_bytes(VM2 *vm) {
  assert(vm);
  u1 n = vm->d[DICT_TAIL_INDEX];
  u1 j = DICT_DUMP_FIRST;
  u1 ttl = DICT_FIRST + 1;
  u1 phase = 0;
  printf("%s", ANSI_BLUE);
  for (u1 i = 0; i < n; i++) {
    trace("i=%d, j=%d, dd[j]=%d, ttl=%d", i, j, vm->dd[j], ttl);
    if (i) printf(" ");
    if (!--ttl) {
      switch (phase) {
        case 0:
        case 3:
          phase = 1;
          ttl = vm->dd[j];
          printf("%s", ANSI_GREEN);
          j += 3;
          break;
        case 1:
          phase = 2;
          ttl = 1;
          printf("%s", ANSI_YELLOW);
          break;
        case 2:
          phase = 3;
          ttl = 1;
          printf("%s", ANSI_RED);
          break;
        default:
          err("Logic error.");
          break;
      }
    }
    printf("%02hhX", vm->d[i]);
  }
  printf("%s\n", ANSI_RESET_ALL);
}

void vm2_print_dict_dump(VM2 *vm) {
  assert(vm);
  u1 dd_n = vm->dd[DICT_DUMP_COUNT_INDEX];
  const char *color = "";
  printf("%s", ANSI_BLUE);
  for (u1 i = 0; i < dd_n * 3 + 2; i++) {
    if (i != 0) printf(" ");
    if (i == DICT_DUMP_FIRST) color = ANSI_RESET;
    printf("%s%02hhX", color, vm->dd[i]);
    color = "";
  }
  printf("%s\n", ANSI_RESET_ALL);
}

void vm2_print_code_bytes(VM2 *vm) {
  assert(vm);
  u1 n = 255;
  while (vm->c[n] == I_HLT) n--;
  n += 2;
  u1 j = DICT_DUMP_FIRST + 2;
  const char *c0 = "";
  const char *c1 = "";
  printf("%s", ANSI_RESET);
  for (u1 i = 0; i < n; i++) {
    trace("i=%d, j=%d, dd[j]=%d, ttl=%d", i, j, vm->dd[j], ttl);
    if (i) printf(" ");
    if (i == vm->dd[j]) {
      c0 = ANSI_GREEN;
      c1 = ANSI_RESET;
      j += 3;
    } else if (vm->c[i] == I_RET) {
      c0 = ANSI_RED;
      c1 = ANSI_RESET;
    } else if (vm->c[i] == I_HLT) {
      c0 = ANSI_RED;
      c1 = ANSI_RESET;
    } else if (vm->c[i] == I_CAL) {
      c0 = ANSI_MAGENTA;
      c1 = ANSI_RESET;
    } else if ((vm->c[i] & OPC_JUMP_HEADER_MASK) == OPC_JUMP_HEADER) {
      c0 = ANSI_YELLOW;
      c1 = ANSI_RESET;
    } else if (vm->c[i] == I_MKR) {
      c0 = ANSI_BLUE;
      c1 = ANSI_RESET;
    } else if (vm->c[i] == I_NOP) {
      c0 = ANSI_BLUE;
      c1 = ANSI_RESET;
    }
    printf("%s%02hhX%s", c0, vm->c[i], c1);
    c0 = "", c1 = "";
  }
  printf("%s\n", ANSI_RESET_ALL);
}

void vm2_print_reg(VM2 *vm) {
  printf("PC %02hhX | CI %02hhX | SP %02hhX | RP %02hhX \
| A %02hhX | B %02hhX | F %02hhX | T %5hd | %s\n",
      vm->pc, vm->ci, vm->sp, vm->rp, vm->a, vm->b, vm->f,
      vm->t, vm2_ins_str(vm->ci));
}

void vm2_print_mem_bytes(VM2 *vm) {
  const u1 w = 16;
  for (u1 i = 0; i < w; i++) {
    if (i >= vm->sp && i < (vm->rp + 1) / w) continue;
    for (u1 j = 0; j < LIM / w; j++) {
      u1 a = i * w + j;
      if (j != 0) printf(" ");
      if (a == 0) printf("%s", ANSI_GREEN);
      if (a == vm->sp) printf("%s", ANSI_RESET_ALL);
      if (a - 1 == vm->rp) printf("%s", ANSI_YELLOW);
      printf("%02hhX", vm->m[a]);
    }
    printf("\n");
  }
  printf("%s", ANSI_RESET_ALL);
}

void vm2_print_line_hex(u1 n) {
  for (u1 i = 0; i < n; i++) {
    if (i != 0) printf(" ");
    printf("%02hhX", i);
  }
  printf("\n");
}

void vm2_print_line_dec(u1 n) {
  for (u1 i = 0; i < n; i++) {
    if (i != 0) printf(" ");
    printf("%02hhd", i);
  }
  printf("\n");
}

int vm2_cmp(const char *x, u1* y, u1 n) {
  for (int i = 0; i < n; i++) {
    u1 a = x[i];
    u1 b = y[i];
    if ('a' <= a && a <= 'z') a = a + 'A' - 'a';
    if ('a' <= b && b <= 'z') b = b + 'A' - 'a';
    if (a < b) return -1;
    if (b < a) return 1;
  }
  return 0;
}

void vm2_print_all(VM2 *vm) {
  assert(vm);
  if (!vm->log) return;
  trace("enter");
  vm2_dict_parse(vm);
  vm2_print_dict_list(vm);
  vm2_print_line_dec(43);
  vm2_print_line_hex(43);
  vm2_print_dict_bytes(vm);
  vm2_print_dict_dump(vm);
  vm2_print_code_bytes(vm);
  vm2_print_reg(vm);
  vm2_print_mem_bytes(vm);
  trace("leave");
}

