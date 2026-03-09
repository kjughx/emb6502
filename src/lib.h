#ifndef _LIB_H_
#define _LIB_H_
#include <stdint.h>

typedef uint8_t byte_t;
typedef uint16_t word_t;

#define STACK_START 0x100
#define NMI_VEC 0xfffa
#define RST_VEC 0xfffc
#define IRQ_VEC 0xfffe

enum Mode {
  MODE_ACC,
  MODE_IMPL,
  MODE_IMM,
  MODE_REL,
  MODE_ZPG,
  MODE_ZPGX,
  MODE_ZPGY,
  MODE_ABS,
  MODE_ABSX,
  MODE_ABSY,
  MODE_IND,
  MODE_INDX,
  MODE_INDY,
};

enum {
  BIT_CARRY,
  BIT_ZERO,
  BIT_INT,
  BIT_DEC,
  BIT_BRK,
  BIT_UNUSED,
  BIT_OF,
  BIT_NEG,
};

enum Flags {
  FLAG_CARRY = 1 << BIT_CARRY,
  FLAG_ZERO = 1 << BIT_ZERO,
  FLAG_INT = 1 << BIT_INT,
  FLAG_DEC = 1 << BIT_DEC,
  FLAG_BRK = 1 << BIT_BRK,
  FLAG_UNUSED = 1 << BIT_UNUSED,
  FLAG_OF = 1 << BIT_OF,
  FLAG_NEG = 1 << BIT_NEG,
};

#define SIGN_BIT BIT_NEG

#define BIT_GET(v, b) (!!((v) & (1 << (b))))
#define BIT_SET(l, v, b) ((l) = (l) & ~(1 << (b)) | ((v) << (b)))
#define FLAG_SET(flag) cpu.sr |= (flag);
#define FLAG_CLR(flag) cpu.sr &= ~(flag);
#define FLAG_GET(flag) (!!(cpu.sr & (flag)))
struct cpu {
  word_t pc;
  byte_t ac, x, y, sp, sr, irq, nmi;
  uint64_t clk;
};

#define MEMORY_SIZE 1 << 16

extern bool trace_enabled;

int step();
void reset();
void irq();
void nmi();

#endif // _LIB_H_
