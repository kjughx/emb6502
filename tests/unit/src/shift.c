#include "lib.h"
#include "test.h"

void asl();
void lsr();
void rol();
void ror();


bool shift(void) {
  reset();

  cpu.ac = 0b0;
  cpu.sr = FLAGS_DEF;
  exec_expect(asl, ACC, 0,
              .ac = Some(0),
              .sr = Some(FLAGS_DEF & ~FLAG_CARRY & ~FLAG_NEG | FLAG_ZERO));

  cpu.ac = 0b01;
  cpu.sr = FLAGS_DEF;
  exec_expect(asl, ACC, 0,
              .ac = Some(0b10),
              .sr = Some(FLAGS_DEF & ~FLAG_CARRY & ~FLAG_NEG & ~FLAG_ZERO));

  cpu.ac = 0b10000000;
  cpu.sr = FLAGS_DEF;
  exec_expect(asl, ACC, 0,
              .ac = Some(0),
              .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_CARRY | FLAG_ZERO));

  cpu.ac = 0b0;
  cpu.sr = FLAGS_DEF;
  exec_expect(lsr, ACC, 0,
              .ac = Some(0),
              .sr = Some(FLAGS_DEF & ~FLAG_NEG & ~FLAG_CARRY | FLAG_ZERO));

  cpu.ac = 0b1;
  cpu.sr = FLAGS_DEF;
  exec_expect(lsr, ACC, 0,
              .ac = Some(0),
              .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_ZERO | FLAG_CARRY));

  cpu.ac = 0b10;
  cpu.sr = FLAGS_DEF;
  exec_expect(lsr, ACC, 0,
              .ac = Some(0b1),
              .sr = Some(FLAGS_DEF & ~FLAG_NEG & ~FLAG_ZERO & ~FLAG_CARRY));

  cpu.ac = 0b0;
  cpu.sr = FLAGS_DEF & ~FLAG_CARRY;
  exec_expect(rol, ACC, 0,
              .ac = Some(0),
              .sr = Some(FLAGS_DEF & ~FLAG_NEG & ~FLAG_CARRY | FLAG_ZERO));

  cpu.ac = 0b1;
  cpu.sr = FLAGS_DEF | FLAG_CARRY;
  exec_expect(rol, ACC, 0,
              .ac = Some(0b11),
              .sr = Some(FLAGS_DEF & ~FLAG_NEG & ~FLAG_ZERO & ~FLAG_CARRY));

  cpu.ac = 0b01000000;
  cpu.sr = FLAGS_DEF | FLAG_CARRY;
  exec_expect(rol, ACC, 0,
              .ac = Some(0b10000001),
              .sr = Some(FLAGS_DEF & ~FLAG_ZERO & ~FLAG_CARRY | FLAG_NEG));

  cpu.ac = 0b10000000;
  cpu.sr = FLAGS_DEF & ~FLAG_CARRY;
  exec_expect(rol, ACC, 0,
              .ac = Some(0),
              .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_ZERO | FLAG_CARRY));

  cpu.ac = 0;
  cpu.sr = FLAGS_DEF & ~FLAG_CARRY;
  exec_expect(ror, ACC, 0,
              .ac = Some(0),
              .sr = Some(FLAGS_DEF & ~FLAG_NEG & ~FLAG_CARRY | FLAG_ZERO));

  cpu.ac = 0b10;
  cpu.sr = FLAGS_DEF | FLAG_CARRY;
  exec_expect(ror, ACC, 0,
              .ac = Some(0b10000001),
              .sr = Some(FLAGS_DEF & ~FLAG_ZERO & ~FLAG_CARRY | FLAG_NEG));

  cpu.ac = 0b1;
  cpu.sr = FLAGS_DEF | FLAG_CARRY;
  exec_expect(ror, ACC, 0,
              .ac = Some(0b10000000),
              .sr = Some(FLAGS_DEF & ~FLAG_ZERO | FLAG_NEG | FLAG_CARRY));

  cpu.ac = 0b1;
  cpu.sr = FLAGS_DEF & ~FLAG_CARRY;
  exec_expect(ror, ACC, 0,
              .ac = Some(0),
              .sr = Some(FLAGS_DEF & ~FLAG_NEG | FLAG_ZERO | FLAG_CARRY));

  printf("Unit/shift: PASSED\n");
}
