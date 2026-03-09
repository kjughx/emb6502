#include "lib.h"
#include "test.h"

void adc();
void sbc();

bool arithmetic(void) {
  reset();

  /* 0 + 0 (+0) == 0 */
  cpu.ac = 0;
  cpu.sr = FLAGS_DEF & ~FLAG_CARRY;
  exec(adc, IMM, 0);
  expect_ac(0);
  expect_flags(FLAGS_DEF & ~FLAG_OF & ~FLAG_CARRY & ~FLAG_NEG | FLAG_ZERO);

  /* 0 + 0 (+1) = 1*/
  cpu.ac = 0;
  cpu.sr = FLAGS_DEF | FLAG_CARRY;
  exec(adc, IMM, 0);
  expect_ac(1);
  expect_flags(FLAGS_DEF & ~FLAG_OF & ~FLAG_CARRY & ~FLAG_NEG & ~FLAG_ZERO);

  /* 0 + -1 (+1) = 0 */
  cpu.ac = 0;
  cpu.sr = FLAGS_DEF | FLAG_CARRY;
  exec(adc, IMM, 0xff);
  expect_ac(0x0);
  expect_flags(FLAGS_DEF & ~FLAG_NEG & ~FLAG_OF | FLAG_CARRY | FLAG_ZERO);

  /* -86 + -69 (+0) == -17 */
  cpu.ac = 0xaa;
  cpu.sr = FLAGS_DEF & ~FLAG_CARRY;
  exec(adc, IMM, 0xbb);
  expect_ac(0x65);
  expect_flags(FLAGS_DEF & ~FLAG_NEG & ~FLAG_ZERO | FLAG_CARRY | FLAG_OF);

  /* 127 + 127 (+0) = -2 */
  cpu.ac = 0x7f;
  cpu.sr = FLAGS_DEF & ~FLAG_CARRY;
  exec(adc, IMM, 0x7f);
  expect_ac(0xfe);
  expect_flags(FLAGS_DEF & ~FLAG_ZERO & ~FLAG_CARRY | FLAG_NEG | FLAG_OF);

  /* 5 + 32 (+1) = 38 */
  cpu.ac = 0x5;
  cpu.sr = FLAGS_DEF | FLAG_CARRY;
  exec(adc, IMM, 0x20);
  expect_ac(0x26);
  expect_flags(FLAGS_DEF & ~FLAG_ZERO & ~FLAG_CARRY & ~FLAG_NEG & ~FLAG_OF);

  /* -123 + -112 (+1) = 22 */
  cpu.ac = 0x85;
  cpu.sr = FLAGS_DEF | FLAG_CARRY;
  exec(adc, IMM, 0x90);
  expect_ac(0x16);
  expect_flags(FLAGS_DEF & ~FLAG_ZERO & ~FLAG_NEG | FLAG_CARRY | FLAG_OF);

  /* 127 + -128 (+0) = -1 */
  cpu.ac = 0x7f;
  cpu.sr = FLAGS_DEF & ~FLAG_CARRY;
  exec(adc, IMM, 0x80);
  expect_ac(0xff);
  expect_flags(FLAGS_DEF & ~FLAG_OF & ~FLAG_ZERO & ~FLAG_CARRY | FLAG_NEG);

  /* 0 - 0 (+0) = 0 */
  cpu.ac = 0;
  cpu.sr = FLAGS_DEF | FLAG_CARRY;
  exec(sbc, IMM, 0);
  expect_ac(0);
  expect_flags(FLAGS_DEF & ~FLAG_OF & ~FLAG_NEG | FLAG_CARRY | FLAG_ZERO);

  /* 0 - 0 (-1) = 0 */
  cpu.ac = 0;
  cpu.sr = FLAGS_DEF & ~FLAG_CARRY;
  exec(sbc, IMM, 0);
  expect_ac(0xff);
  expect_flags(FLAGS_DEF & ~FLAG_CARRY & ~FLAG_ZERO | FLAG_NEG | FLAG_OF);

  /* 0 - -1 (+0) = 1 */
  cpu.ac = 0;
  cpu.sr = FLAGS_DEF | FLAG_CARRY;
  exec(sbc, IMM, 0xff);
  expect_ac(0x1);
  expect_flags(FLAGS_DEF & ~FLAG_NEG & ~FLAG_OF & ~FLAG_ZERO | FLAG_CARRY);

  /* -86 - -69 (+0) = -17 */
  cpu.ac = 0xaa;
  cpu.sr = FLAGS_DEF | FLAG_CARRY;
  exec(sbc, IMM, 0xbb);
  expect_ac(0xef);
  expect_flags(FLAGS_DEF & ~FLAG_OF & ~FLAG_ZERO | FLAG_NEG);

  printf("Unit/arithmetic: PASSED\n");
}
