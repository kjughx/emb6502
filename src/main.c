#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include "lib.h"

struct cpu cpu;
uint8_t memory[MEMORY_SIZE];

void usage() {
  printf("Usage: ./emu <rom> [<entry>]");
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

  ssize_t filesize = lseek(fd, 0, SEEK_END);
  if (filesize > MEMORY_SIZE) {
      fprintf(stderr, "ERROR: Program size exceeds memory size, aborting\n");
      return 1;
  }
  lseek(fd, 0, SEEK_SET);

  ssize_t n = read(fd, memory, MEMORY_SIZE);
  if (n < 0) {
      fprintf(stderr, "ERROR: Could not read file %s: %s\n", filename, strerror(errno));
  }

  reset();
  if (argc > 2) {
    char *endptr;
    long value = strtol(argv[2], &endptr, 16);

    if (*endptr != '\0') {
      fprintf(stderr, "ERROR: Invalid entry point: %s\n", argv[2]);
      return 1;
    }
    cpu.pc = value;
  }

  trace_enabled = true;
  while (cpu.pc != 0x10A5) {
    if (step()) return 1;
  }

  printf("INFO: Program halted at 0x%04X\n", cpu.pc);

  return 0;
}
