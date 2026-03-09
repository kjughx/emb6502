#include <string.h>

#include "lib.h"
#include "test.h"

#define CANARY 0xaa
static uint8_t _memory[MEMORY_SIZE] = {CANARY};

void sta();
void sty();
void stx();
void lda();
void ldx();
void ldy();
void tax();
void tay();
void txa();
void tya();

/* LDA, LDX, LDY */
void load() {
  reset();
  byte_t flags_def = 0xff & ~FLAG_BRK & ~FLAG_UNUSED & ~FLAG_NEG & ~FLAG_ZERO;

  word_t addr = 0x114;
  byte_t val = 0x11;
  memory[addr] = val;

  /* FLAG_ZERO = 1, FLAG_NEG = 0 */
  cpu.sr = flags_def;
  exec_expect(lda, IMM, 0,
              .sr = Some(flags_def | FLAG_ZERO),
              .ac = Some(0),
              );

  cpu.sr = flags_def;
  exec_expect(lda, ABS, addr,
              .ac = Some(val));

  cpu.sr = flags_def;
  exec_expect(lda, IMM, CANARY,
              .ac = Some(CANARY),
              .sr = Some(flags_def | FLAG_NEG));

  /* FLAG_ZERO = 1, FLAG_NEG = 0 */
  cpu.sr = flags_def;
  exec_expect(ldx, IMM, 0,
              .x = Some(0),
              .sr = Some(flags_def | FLAG_ZERO));

  cpu.sr = flags_def;
  exec_expect(ldx, ABS, addr,
              .x = Some(val));

  cpu.sr = flags_def;

  exec_expect(ldx, IMM, 0xff,
              .x = Some(0xff),
              .sr = Some(flags_def | FLAG_NEG));

  /* FLAG_ZERO = 1, FLAG_NEG = 0 */
  cpu.sr = flags_def;
  exec_expect(ldy, IMM, 0,
              .y = Some(0),
              .sr = Some(flags_def | FLAG_ZERO));

  cpu.sr = flags_def;
  exec_expect(ldy, ABS, addr,
              .y = Some(val));

  cpu.sr = flags_def;

  exec_expect(ldy, IMM, 0xff,
              .y = Some(0xff),
              .sr = Some(flags_def | FLAG_NEG));
}

/* STA, STY, STX */
#define addr 0x114
void store() {
  byte_t val = 0x42;
  cpu.ac = cpu.x = cpu.y = val;
  byte_t flags_def = 0xff & ~FLAG_BRK & ~FLAG_UNUSED;
  cpu.sr = flags_def;

  exec_expect(sta, ABS, addr,
              .mem = {
                [0x114] = Some(cpu.ac)
              });

  exec_expect(stx, ABS, addr + 1,
              .mem = {
                [0x114 + 1] = Some(cpu.x)
              });

  exec_expect(sty, ABS, addr + 2,
              .mem = {
                [0x114 + 2] = Some(cpu.y)
              });
}

/* TAX, TAY, TXA, TYA */
void transfer() {
  reset();
  byte_t flags_def = 0xff & ~FLAG_BRK & ~FLAG_UNUSED & ~FLAG_NEG & ~FLAG_ZERO;
  byte_t val = CANARY;

  cpu.sr = flags_def | FLAG_ZERO;
  exec_expect(lda, IMM, val,
              .ac = Some(val),
              .sr = Some(flags_def | FLAG_NEG));

  cpu.sr = flags_def | FLAG_ZERO;
  exec_expect(tax, IMPL, 0xff,
              .x = Some(val),
              .sr = Some(flags_def | FLAG_NEG));

  cpu.sr = flags_def | FLAG_ZERO;
  exec_expect(tay, IMPL, 0xff,
              .y = Some(val),
              .sr = Some(flags_def | FLAG_NEG));

  val = 0;
  exec(lda, IMM, val);

  cpu.sr = flags_def | FLAG_NEG;
  exec_expect(tax, IMPL, 0xff,
              .x = Some(val),
              .sr = Some(flags_def | FLAG_ZERO));

  cpu.sr = flags_def | FLAG_NEG;
  exec_expect(tay, IMPL, 0xff,
              .y = Some(val),
              .sr = Some(flags_def | FLAG_ZERO));
}

bool loadstore(void) {
  load_mem(_memory);

  load();
  store();
  transfer();

  printf("Unit/load_store: PASSED\n", __FILE__);

  return 0;
}
