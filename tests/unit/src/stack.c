#include "lib.h"
#include "test.h"

void pha();
void php();
void pla();
void plp();

#define SP 0x55
bool stack(void) {
  reset();
  byte_t val = 0xaa;
  cpu.ac = val;

  cpu.sp = SP;
  cpu.sr = FLAGS_DEF;
  exec_expect(pla, IMPL, 0,
              .ac = Some(0),
              .sp = Some(SP + 1),
              .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_ZERO));

  cpu.sp = SP;
  cpu.sr = FLAGS_DEF;
  cpu.ac = val;
  exec_expect(pha, IMPL, 0,
              .sp = Some(SP - 1),
              .sr = Some(FLAGS_DEF),
              .mem = {
                [STACK_START + SP] = Some(val),
              });

  cpu.ac = 0;
  cpu.sr = FLAGS_DEF;
  exec_expect(pla, IMPL, 0,
              .ac = Some(val),
              .sp = Some(SP),
              .sr = Some(FLAGS_DEF & ~FLAG_ZERO | FLAG_NEG));

  cpu.sp = SP;
  cpu.sr = FLAGS_DEF;
  exec_expect(php, IMPL, 0,
              .sp = Some(SP - 1),
              .sr = Some(FLAGS_DEF),
              .mem = {
                [STACK_START + SP] = Some(FLAGS_DEF | FLAG_BRK | FLAG_UNUSED),
              });

  cpu.sr = 0;
  exec_expect(plp, IMPL, 0,
              .sp = Some(SP),
              .sr = Some(FLAGS_DEF));

  printf("Unit/stack: PASSED\n");
}
