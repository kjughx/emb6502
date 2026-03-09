#ifndef _TEST_H_
#define _TEST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "lib.h"

extern struct cpu cpu;
extern byte_t memory[MEMORY_SIZE];
extern enum Mode mode;
extern word_t arg;
static byte_t __memory[MEMORY_SIZE];

enum {
  ACC  = MODE_ACC,
  IMPL = MODE_IMPL,
  IMM  = MODE_IMM,
  REL  = MODE_REL,
  ZPG  = MODE_ZPG,
  ZPGX = MODE_ZPGX,
  ZPGY = MODE_ZPGY,
  ABS  = MODE_ABS,
  ABSX = MODE_ABSX,
  ABSY = MODE_ABSY,
  IND  = MODE_IND,
  INDX = MODE_INDX,
  INDY = MODE_INDY,
};

#define MASK_SOME (~(uint32_t)0xffff)
typedef struct {
  union {
    byte_t byte;
    word_t word;
    uint32_t data;
  };
} Option;

typedef struct {
  Option pc, ac, x, y, sp, sr, irq, nmi;
  Option mem[65536];
} State;

#define is_some(o) (assert(((o).data & MASK_SOME) == MASK_SOME || ((o).data & MASK_SOME) == 0), (o).data & MASK_SOME)
#define Some(v) (Option) { .data = v | MASK_SOME }

#define save_mem() memcpy(__memory, memory, sizeof(memory));
#define load_mem(where) assert(sizeof(memory) == sizeof((where))), memcpy(memory, (where), sizeof(memory))

#define FLAGS_ALL 0xff & ~FLAG_UNUSED

void writeb(word_t addr, byte_t val);
void writew(word_t addr, word_t val);

#define __expect_ac(a, file, line) do { \
    if (cpu.ac != (a)) { \
      printf("%s:%d: Expected A == 0x%02X, found 0x%02X\n", file, line, (a), cpu.ac); \
      exit(1);                                                  \
    }                                                           \
  } while(0)
#define expect_ac(a) __expect_ac(a, __FILE__, __LINE__)

#define __expect_x(_x, file, line) do { \
    if (cpu.x != (_x)) { \
      printf("%s:%d: Expected X == 0x%02X, found 0x%02X\n", file, line, (_x), cpu.x); \
      exit(1);                                                  \
    }                                                           \
  } while(0)
#define expect_x(_x) __expect_x(_x, __FILE__, __LINE__)

#define __expect_y(_y, file, line) do { \
    if (cpu.y != (_y)) { \
      printf("%s:%d: Expected Y == 0x%02X, found 0x%02X\n", file, line, (_y), cpu.y); \
      exit(1);                                                  \
    }                                                           \
  } while(0)
#define expect_y(_y) __expect_y(_y, __FILE__, __LINE__)

#define __expect_sp(_sp, file, line) do { \
    if (cpu.sp != (_sp)) { \
      printf("%s:%d: Expected SP == 0x%02X, found 0x%02X\n", file, line, (_sp), cpu.sp); \
      exit(1);                                                  \
    }                                                           \
  } while(0)
#define expect_sp(_sp) __expect_sp(_sp, __FILE__, __LINE__)

#define __expect_pc(_pc, file, line) do { \
    if (cpu.pc != (_pc)) { \
      printf("%s:%d: Expected PC == 0x%04X, found 0x%04X\n", file, line, (_pc), cpu.pc); \
      exit(1);                                                  \
    }                                                           \
  } while(0)
#define expect_pc(_pc) __expect_pc(_pc, __FILE__, __LINE__)

#define __expect_flags(flags, file, line) do { \
    if (cpu.sr != (flags)) { \
      printf("%s:%d: Expected Flags to be %08b, found %08b\n", file, line, (flags), cpu.sr); \
      exit(1);                                                  \
    }                                                           \
  } while(0)
#define expect_flags(flags) __expect_flags(flags, __FILE__, __LINE__)

#define __expect_flag(flag, v, file, line) do { \
    if (FLAG_GET((flag)) != (v)) {                                      \
      printf("%s:%d: Expected Flag %s to be %s\n", file, line, #flag, (v) ? "set" : "cleared"); \
      exit(1);                                                          \
    }                                                                   \
  } while(0)
#define expect_flag(flag, v) __expect_flag((flag), (v), __FILE__, __LINE__)

#define __expect_mem(addr, val, file, line) do { \
    if (memory[(addr)] != (val)) { \
      printf("%s:%d: Expected memory[0x%04X] == 0x%02X, found 0x%02X\n", file, line, (addr), (val), memory[(addr)]); \
      exit(1);                                                  \
    }                                                           \
  } while(0)
#define expect_mem(addr, val) __expect_mem((addr), (val), __FILE__, __LINE__)

#define __expect(a, b, op, file, line) do {      \
    if (!(a op b)) {                                      \
      printf("%s:%d: Expected %s (%d) %s %s (%d)\n", file, line, #a, a, #op, #b, b); \
      exit(1);                                                          \
    }                                                                   \
  } while(0)
#define expect(a, b, op) __expect_eq((a), (b), (op), __FILE__, __LINE__)

#define __expect_eq(a, b, file, line) __expect((a), (b), ==, file, line)
#define expect_eq(a, b) __expect_eq(a, b, __FILE__, __LINE__)

#define FLAGS_DEF 0xff & ~FLAG_BRK & ~FLAG_UNUSED

static void exec(void (*ins)(void), enum Mode _mode, word_t _arg) {
  mode = _mode;
  switch (mode) {
  case MODE_ACC: arg = cpu.ac; break;
  case MODE_IMPL: break;
  case MODE_REL: arg = cpu.pc + _arg; break;
  case MODE_IMM:
  case MODE_ZPG:
  case MODE_ABS: arg = _arg; break;
  default:
    printf("ERROR: Don't test mode %d here\n", mode);
    exit(1);
  }

  (*ins)();
}

static void compare_state(struct cpu cpu_cp, State state, const char* file, int line) {
  if (is_some(state.pc)) __expect_pc(state.pc.word, file, line);
  else __expect_pc(cpu_cp.pc, file, line);
  if (is_some(state.ac)) __expect_ac(state.ac.byte, file, line);
  else __expect_ac(cpu_cp.ac, file, line);
  if (is_some(state.x)) __expect_x(state.x.byte, file, line);
  else __expect_x(cpu_cp.x, file, line);
  if (is_some(state.y)) __expect_y(state.y.byte, file, line);
  else __expect_y(cpu_cp.y, file, line);
  if (is_some(state.sp)) __expect_sp(state.sp.byte, file, line);
  else __expect_sp(cpu_cp.sp, file, line);
  if (is_some(state.sr)) __expect_flags(state.sr.byte, file, line);
  else __expect_flags(cpu_cp.sr, file, line);
  for (size_t i = 0; i < MEMORY_SIZE; ++i) {
    if(is_some(state.mem[i])) __expect_mem(i, state.mem[i].byte, file, line);
    else __expect_mem(i, __memory[i], file, line);
  }
}

#define exec_expect(ins, mode, arg, ...) __exec_expect((ins), (mode), (arg), (State) {__VA_ARGS__}, __FILE__, __LINE__)
static void __exec_expect(void (*ins)(void), enum Mode _mode, word_t _arg, State state, const char *file, int line) {
  struct cpu cpu_cp = cpu;
  save_mem();
  exec(ins, _mode, _arg);
  compare_state(cpu_cp, state, file, line);
}


#endif // _TEST_H_
