#ifndef _LIB_H_
#define _LIB_H_

typedef uint8_t byte_t;
typedef uint16_t word_t;

struct cpu {
  word_t pc;
  byte_t ac, x, y, sp, sr, irq, nmi;
  uint64_t clk;
};

extern bool trace_enabled;

int step();
void reset();
void irq();
void nmi();

#endif // _LIB_H_
