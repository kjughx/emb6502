#include "lib.h"
#include "test.h"

void and();
void bit();
void cmp();
void cpx();
void cpy();
void eor();
void ora();

bool logical(void) {
  { /* AND */
    reset();

    cpu.sr = FLAGS_DEF;
    cpu.ac = 0b10101010;
    exec_expect(and, IMM, 0,
                .ac = Some(0),
                .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_ZERO));

    cpu.sr = FLAGS_DEF | FLAG_ZERO;
    cpu.ac = 0b11111111;
    exec_expect(and, IMM, FLAG_NEG,
                .ac = Some(FLAG_NEG),
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO | FLAG_NEG));
  }

  { /* BIT */
    reset();

    cpu.ac = 0b10101010;

    cpu.sr = FLAGS_DEF;
    exec_expect(bit, IMM, 0b01010101,
                .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_ZERO | FLAG_OF));

    exec_expect(bit, IMM, 0b11010101,
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO | FLAG_OF | FLAG_NEG));
  }

  { /* CMP */
    reset();

    cpu.sr = FLAGS_DEF;
    cpu.ac = 0x1;
    exec_expect(cmp, IMM, 0x2,
                .sr = Some(FLAGS_DEF & ~FLAG_CARRY & ~FLAG_ZERO | FLAG_NEG));

    cpu.sr = FLAGS_DEF;
    cpu.ac = 0x2;
    exec_expect(cmp, IMM, 0x1,
                .sr = Some(FLAGS_DEF & ~FLAG_NEG & ~FLAG_ZERO | FLAG_CARRY));

    cpu.sr = FLAGS_DEF;
    cpu.ac = 0x1;
    exec_expect(cmp, IMM, 0x1,
                .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_CARRY | FLAG_ZERO));

    cpu.sr = FLAGS_DEF;
    cpu.ac = 0xa;
    exec_expect(cmp, IMM, 0xb,
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO & ~FLAG_CARRY | FLAG_NEG));

    cpu.sr = FLAGS_DEF;
    cpu.ac = 0xb;
    exec_expect(cmp, IMM, 0xa,
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO & ~FLAG_NEG | FLAG_CARRY));

    cpu.sr = FLAGS_DEF;
    cpu.ac = 0xa;
    exec_expect(cmp, IMM, 0xa,
                .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_CARRY | FLAG_ZERO));
  }

  { /* CPX */
    reset();

    cpu.sr = FLAGS_DEF;
    cpu.x = 0x1;
    exec_expect(cpx, IMM, 0x2,
                .sr = Some(FLAGS_DEF & ~FLAG_CARRY & ~FLAG_ZERO | FLAG_NEG));

    cpu.sr = FLAGS_DEF;
    cpu.x = 0x2;
    exec_expect(cpx, IMM, 0x1,
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO & ~FLAG_NEG | FLAG_CARRY));

    cpu.sr = FLAGS_DEF;
    cpu.x = 0x1;
    exec_expect(cpx, IMM, 0x1,
                .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_CARRY | FLAG_ZERO));

    cpu.sr = FLAGS_DEF;
    cpu.x = 0xa;
    exec_expect(cpx, IMM, 0xb,
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO & ~FLAG_CARRY | FLAG_NEG));

    cpu.sr = FLAGS_DEF;
    cpu.x = 0xb;
    exec_expect(cpx, IMM, 0xa,
                .sr = Some(FLAGS_DEF & ~FLAG_NEG & ~FLAG_ZERO | FLAG_CARRY));

    cpu.sr = FLAGS_DEF;
    cpu.x = 0xa;
    exec_expect(cpx, IMM, 0xa,
                .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_CARRY | FLAG_ZERO));
  }

  { /* CPY */
    reset();

    cpu.sr = FLAGS_DEF;
    cpu.y = 0x1;
    exec_expect(cpy, IMM, 0x2,
                .sr = Some(FLAGS_DEF & ~FLAG_CARRY & ~FLAG_ZERO | FLAG_NEG));

    cpu.sr = FLAGS_DEF;
    cpu.y = 0x2;
    exec_expect(cpy, IMM, 0x1,
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO & ~FLAG_NEG | FLAG_CARRY));

    cpu.sr = FLAGS_DEF;
    cpu.y = 0x1;
    exec_expect(cpy, IMM, 0x1,
                .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_CARRY | FLAG_ZERO));

    cpu.sr = FLAGS_DEF;
    cpu.y = 0xa;
    exec_expect(cpy, IMM, 0xb,
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO & ~FLAG_CARRY | FLAG_NEG));

    cpu.sr = FLAGS_DEF;
    cpu.y = 0xb;
    exec_expect(cpy, IMM, 0xa,
                .sr = Some(FLAGS_DEF & ~FLAG_NEG & ~FLAG_ZERO | FLAG_CARRY));

    cpu.sr = FLAGS_DEF;
    cpu.y = 0xa;
    exec_expect(cpy, IMM, 0xa,
                .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_CARRY | FLAG_ZERO));
  }

  { /* EOR */
    reset();

    cpu.sr = FLAGS_DEF;
    cpu.ac = 0b10101010;
    exec_expect(eor, IMM, 0b01010101,
                .ac = Some(0b11111111),
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO | FLAG_NEG));

    cpu.sr = FLAGS_DEF;
    cpu.ac = 0b11100011;
    exec_expect(eor, IMM, 0b11100011,
                .ac = Some(0),
                .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_ZERO));
  }

  { /* ORA */
    reset();

    cpu.sr = FLAGS_DEF;
    cpu.ac = 0b10101010;
    exec_expect(ora, IMM, 0b01010101,
                .ac = Some(0b11111111),
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO | FLAG_NEG));

    cpu.sr = FLAGS_DEF;
    cpu.ac = 0b11100011;
    exec_expect(ora, IMM, 0b11100011,
                .ac = Some(0b11100011),
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO | FLAG_NEG));

    cpu.sr = FLAGS_DEF;
    cpu.ac = 0;
    exec_expect(ora, IMM, 0b11111111,
                .ac = Some(0b11111111),
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO | FLAG_NEG));

    cpu.sr = FLAGS_DEF;
    cpu.ac = 0;
    exec_expect(ora, IMM, 0,
                .ac = Some(0),
                .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_ZERO));
  }

  printf("Unit/logical: PASSED\n");
}
