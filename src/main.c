#include "main.h"

int main(int argc, char **argv) {
  VM1 vm;

  u1 p[256];
  char d[256];

  int n = argc + 1;
  for (int i = 1; i < argc; i++) {
    n += strlen(argv[i]);
  }
  if (n >= 256) {
    fprintf(stderr,
        "too much input (%d), only 256 bytes allowed.\n", n);
    return EXIT_FAILURE;
  }

  char str[256];
  memset(str, 0, 256);
  int di = 0;
  for (int i = 1; i < argc; i++) {
    char *s = argv[i];
    for (int j = 0; s[j] != 0; j++) str[di++] = s[j];
    str[di++] = ' ';
  }

  vm1_init(&vm, p, d, str);
  vm1_outer(&vm);

  return EXIT_SUCCESS;
}

