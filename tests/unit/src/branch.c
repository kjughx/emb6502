#include "lib.h"
#include "test.h"

void bcc();
void bcs();
void beq();
void bmi();
void bne();
void bpl();
void bvc();
void bvs();

bool branch(void) {
  reset();

  { /* BCC */
    cpu.sr = FLAGS_DEF & ~FLAG_CARRY;
    cpu.pc = 0x1155;
    exec_expect(bcc, REL, 0x42, .pc = Some(0x1155 + 0x42));

    cpu.sr = FLAGS_DEF | FLAG_CARRY;
    cpu.pc = 0x1155;
    exec_expect(bcc, REL, 0x42, .pc = Some(0x1155));
  }

  { /* BCS */
    cpu.sr = FLAGS_DEF & ~FLAG_CARRY;
    cpu.pc = 0x1155;
    exec(bcs, REL, 0x42);
    exec_expect(bcs, REL, 0x42, .pc = Some(0x1155));

    cpu.sr = FLAGS_DEF | FLAG_CARRY;
    cpu.pc = 0x1155;
    exec_expect(bcs, REL, 0x42, .pc = Some(0x1155 + 0x42));
  }

  { /* BEQ */
    cpu.sr = FLAGS_DEF & ~FLAG_ZERO;
    cpu.pc = 0x1155;
    exec_expect(beq, REL, 0x42, .pc = Some(0x1155));

    cpu.sr = FLAGS_DEF | FLAG_ZERO;
    cpu.pc = 0x1155;
    exec_expect(beq, REL, 0x42, .pc = Some(0x1155 + 0x42));
  }

  { /* BMI */
    cpu.sr = FLAGS_DEF & ~FLAG_NEG;
    cpu.pc = 0x1155;
    exec_expect(bmi, REL, 0x42, .pc = Some(0x1155));

    cpu.sr = FLAGS_DEF | FLAG_NEG;
    cpu.pc = 0x1155;
    exec_expect(bmi, REL, 0x42, .pc = Some(0x1155 + 0x42));
  }

  { /* BNE */
    cpu.sr = FLAGS_DEF & ~FLAG_ZERO;
    cpu.pc = 0x1155;
    exec_expect(bne, REL, 0x42, .pc = Some(0x1155 + 0x42));

    cpu.sr = FLAGS_DEF | FLAG_ZERO;
    cpu.pc = 0x1155;
    exec_expect(bne, REL, 0x42, .pc = Some(0x1155));
  }

  { /* BPL */
    cpu.sr = FLAGS_DEF & ~FLAG_NEG;
    cpu.pc = 0x1155;
    exec_expect(bpl, REL, 0x42, .pc = Some(0x1155 + 0x42));

    cpu.sr = FLAGS_DEF | FLAG_NEG;
    cpu.pc = 0x1155;
    exec_expect(bpl, REL, 0x42, .pc = Some(0x1155));
  }

  { /* BVC */
    cpu.sr = FLAGS_DEF & ~FLAG_OF;
    cpu.pc = 0x1155;
    exec_expect(bvc, REL, 0x42, .pc = Some(0x1155 + 0x42));

    cpu.sr = FLAGS_DEF | FLAG_OF;
    cpu.pc = 0x1155;
    exec_expect(bvc, REL, 0x42, .pc = Some(0x1155));
  }

  { /* BVS */
    cpu.sr = FLAGS_DEF & ~FLAG_OF;
    cpu.pc = 0x1155;
    exec_expect(bvs, REL, 0x42, .pc = Some(0x1155));

    cpu.sr = FLAGS_DEF | FLAG_OF;
    cpu.pc = 0x1155;
    exec_expect(bvs, REL, 0x42, .pc = Some(0x1155 + 0x42));
  }

  printf("Unit/branch: PASSED\n");
}
