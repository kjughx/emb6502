#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "lib.h"

struct cpu cpu;
uint8_t memory[1 << 16];

void usage() {
  printf("Usage: ./emu <rom>");
}

int main(int argc, const char* argv[]) {
  if (argc == 1) {
    usage();
    return 1;
  }

  const char *filename = argv[1];

  int fd = open(filename, O_RDONLY);
  if (fd < 0) {
      fprintf(stderr, "ERROR: Could not open file %s: %s\n", filename, strerror(errno));
  }
  ssize_t n = read(fd, memory, 1 << 16);
  if (n < 0) {
      fprintf(stderr, "ERROR: Could not read file %s: %s\n", filename, strerror(errno));
  }

  reset();

  while (step())
    ;

  return 0;
}
