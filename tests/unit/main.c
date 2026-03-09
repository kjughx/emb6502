#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "lib.h"

#define MEMORY_SIZE 1 << 16

struct cpu cpu;
byte_t memory[MEMORY_SIZE];

#define TESTS \
  X(arithmetic)                                 \
    X(branch)                                   \
    X(flags)                                    \
    X(increment_decrement)                      \
    X(jump_call)                                \
    X(loadstore)                                \
    X(logical)                                  \
    X(macros)                                   \
    X(shift)                                    \
    X(stack)                                    \
  X(modes)

#define X(name) bool name(void);
TESTS
#undef X
#define X(name) name,
bool (*tests[])(void) = {
  TESTS
};
#undef X

int main(void) {
  for (size_t i = 0; i < sizeof(tests) / sizeof(*tests); ++i) {
    memset(memory, 0, sizeof(memory));
    tests[i]();
  }
}
