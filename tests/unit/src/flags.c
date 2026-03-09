#include "lib.h"
#include "test.h"

void clc();
void cld();
void cli();
void clv();
void sec();
void sed();
void sei();

bool flags(void) {
  reset();

  { /* CLC */
    byte_t flags_def = 0xff & ~FLAG_BRK & ~FLAG_UNUSED & ~FLAG_CARRY;
    cpu.sr = flags_def | FLAG_CARRY;
    exec_expect(clc, IMPL, 0, .sr = Some(flags_def));
  }

  { /* CLD */
    byte_t flags_def = 0xff & ~FLAG_BRK & ~FLAG_UNUSED & ~FLAG_DEC;
    cpu.sr = flags_def | FLAG_DEC;
    exec_expect(cld, IMPL, 0, .sr = Some(flags_def));
  }

  { /* CLI */
    byte_t flags_def = 0xff & ~FLAG_BRK & ~FLAG_UNUSED & ~FLAG_INT;
    cpu.sr = flags_def | FLAG_INT;
    exec_expect(cli, IMPL, 0, .sr = Some(flags_def));
  }

  { /* CLV */
    byte_t flags_def = 0xff & ~FLAG_BRK & ~FLAG_UNUSED & ~FLAG_OF;
    cpu.sr = flags_def | FLAG_OF;
    exec_expect(clv, IMPL, 0, .sr = Some(flags_def));
  }

  { /* SEC */
    byte_t flags_def = 0xff & ~FLAG_BRK & ~FLAG_UNUSED;
    cpu.sr = flags_def;
    exec(sec, IMPL, 0);
    expect_flags(flags_def | FLAG_CARRY);
  }

  { /* SED */
    byte_t flags_def = 0xff & ~FLAG_BRK & ~FLAG_UNUSED;
    cpu.sr = flags_def;
    exec_expect(sed, IMPL, 0, .sr = Some(flags_def | FLAG_DEC));
  }

  { /* SEI */
    byte_t flags_def = 0xff & ~FLAG_BRK & ~FLAG_UNUSED;
    cpu.sr = flags_def;
    exec_expect(sei, IMPL, 0, .sr = Some(flags_def | FLAG_INT));
  }

  printf("unit/flags: PASSED\n");
  return 0;
}
