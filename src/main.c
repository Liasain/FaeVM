#include "main.h"

int main(int argc, char **argv) {
  VM2 vm;
  vm2_init(&vm);
//  vm2_tests();

  int n = argc + 1;
  for (int i = 1; i < argc; i++) {
    n += strlen(argv[i]);
  }
  if (n >= 256) {
    err("too much input, %d > 255", n);
    return EXIT_FAILURE;
  }

  u1 str[256];
  memset(str, 0, 256);
  int di = 0;
  for (int i = 1; i < argc; i++) {
    if (i != 1) str[di++] = ' ';
    char *s = argv[i];
    for (int j = 0; s[j] != 0; j++) str[di++] = s[j];
  }

  vm2_compile(&vm, str);
  vm2_run(&vm);

  return EXIT_SUCCESS;
}

