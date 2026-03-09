#include "lib.h"
#include "test.h"

void inc();
void inx();
void iny();
void dec();
void dex();
void dey();

bool increment_decrement(void) {
  reset();

  { /* INC */
    cpu.sr = FLAGS_DEF;
    memory[0xaa] = 0xfe;

    exec_expect(inc, ABS, 0xaa,
                .mem= { [0xaa] = Some(0xff) },
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO | FLAG_NEG));

    exec_expect(inc, ABS, 0xaa,
                .mem= { [0xaa] = Some(0x00) },
                .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_ZERO));
  }

  { /* INX */
    cpu.sr = FLAGS_DEF;
    cpu.x = 0xfe;

    exec_expect(inx, IMPL, 0,
                .x = Some(0xff),
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO | FLAG_NEG));

    exec_expect(inx, IMPL, 0,
                .x = Some(0),
                .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_ZERO));
  }

  { /* INY */
    cpu.sr = FLAGS_DEF;
    cpu.y = 0xfe;

    exec_expect(iny, IMPL, 0,
                .y = Some(0xff),
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO | FLAG_NEG));

    exec_expect(iny, IMPL, 0,
                .y = Some(0),
                .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_ZERO));
  }

  { /* DEC */
    cpu.sr = FLAGS_DEF;
    memory[0xaa] = 0x1;

    exec_expect(dec, ABS, 0xaa,
                .mem= { [0xaa] = Some(0) },
                .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_ZERO));

    exec_expect(dec, ABS, 0xaa,
                .mem= { [0xaa] = Some(0xff) },
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO | FLAG_NEG));
  }

  { /* DEX */
    cpu.sr = FLAGS_DEF;
    cpu.x = 0x1;

    exec_expect(dex, IMPL, 0,
                .x = Some(0),
                .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_ZERO));

    exec_expect(dex, IMPL, 0,
                .x = Some(0xff),
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO | FLAG_NEG));
  }

  { /* DEY */
    cpu.sr = FLAGS_DEF;
    cpu.y = 0x1;

    exec_expect(dey, IMPL, 0,
                .y = Some(0),
                .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_ZERO));

    exec_expect(dey, IMPL, 0,
                .y = Some(0xff),
                .sr = Some(FLAGS_DEF & ~FLAG_ZERO | FLAG_NEG));
  }

  printf("Unit/increment_decrement: PASSED\n");
}
