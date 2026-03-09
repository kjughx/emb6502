#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include "lib.h"

#define MEMORY_SIZE 1 << 16

struct cpu cpu;
uint8_t memory[MEMORY_SIZE];

struct Test {
  const char *name;
  word_t success;
};

struct Test tests[] = {
  {"arithmetic",  0x459},
  {"branch", 0x718},
  {"flag", 0x49A},
  {"increment_decrement", 0xA17},
  {"jump_call", 0x533},
  {"load_store", 0x10A5},
  {"logical", 0x10E9},
  {"shift", 0xB50},
  {"stack", 0x518},
};

void usage() {
  printf("Usage: ./test [<testname> <success>]\n\n");
}

word_t parse_hex(const char* arg) {
  char *endptr;
  long value = strtol(arg, &endptr, 16);

  if (*endptr != '\0') {
    fprintf(stderr, "ERROR: Invalid hex value: %s\n", arg);
    exit(1);
  }

  return value;
}

bool run_test(const char *name, word_t entry, word_t success) {
  char path[64] = {0};
  snprintf(path, 64, "build/%s.bin", name);

  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "ERROR: Could not open file %s: %s\n", path, strerror(errno));
  }

  ssize_t filesize = lseek(fd, 0, SEEK_END);
  if (filesize > MEMORY_SIZE) {
    fprintf(stderr, "ERROR: Program size exceeds memory size, aborting\n");
    return false;
  }
  lseek(fd, 0, SEEK_SET);

  ssize_t n = read(fd, memory, MEMORY_SIZE);
  if (n < 0) {
    fprintf(stderr, "ERROR: Could not read file %s: %s\n", path, strerror(errno));
    return false;
  }
  close(fd);

  reset();
  cpu.pc = entry;

  while (cpu.pc != success) {
    if (step()) {
      printf("functional/%s: Trap encountered at 0x%04X\n", name, cpu.pc);
      return false;
    }
  }

  return true;
}

int main(int argc, const char *argv[]) {
  if (argc != 1 && argc != 3) {
    fprintf(stderr, "ERROR: Incorrect amount of arguments!\n");
    usage();
    return 1;
  }

  if (argc == 3) {
    /* trace_enabled = true; */
    run_test(argv[1], 0x400, parse_hex(argv[2]));
  } else {
    for (size_t i = 0; i < sizeof(tests) / sizeof(struct Test); ++i) {
        struct Test test = tests[i];
        if (run_test(test.name, 0x400, test.success))
          printf("functional/%s: PASSSED\n", test.name);
    }
  }
}
