#include "lib.h"
#include "test.h"

static uint8_t _memory[MEMORY_SIZE] = {
  [STACK_START + 0x66] = 0x00,
  [STACK_START + 0x67] = 0x57, 0x11,
  [IRQ_VEC] = 0x55, 0x11,
};

void brk();
void jmp();
void jsr();
void rti();
void rts();

bool jump_call(void) {
  load_mem(_memory);

  reset();

  cpu.sr = FLAGS_DEF;
  cpu.sp = 0x55;
  cpu.pc = 0xaabb;
  exec_expect(brk, IMPL, 0,
              .sr = Some(FLAGS_DEF | FLAG_INT),
              .pc = Some(0x1155),
              .sp = Some(0x52),
              .mem = {
                [STACK_START + 0x55] = Some(0xaa),
                [STACK_START + 0x54] = Some(0xbc),
                [STACK_START + 0x53] = Some(FLAGS_DEF | FLAG_BRK | FLAG_UNUSED),
                });

  exec_expect(jmp, ABS, 0x42,
              .pc = Some(0x42));

  cpu.sr = FLAGS_DEF;
  cpu.pc = 0x1155;
  cpu.sp = 0x55;
  exec_expect(jsr, ABS, 0x42,
              .pc = Some(0x42),
              .sp = Some(0x53),
              .mem = {
                [STACK_START + 0x55] = Some(0x11),
                [STACK_START + 0x54] = Some(0x54),
              });

  cpu.sr = FLAGS_DEF;
  cpu.sp = 0x65;
  cpu.pc = 0x1679;
  cpu.sr = FLAG_BRK;
  exec_expect(rti, IMPL, 0,
              .sp = Some(0x68),
              .pc = Some(0x1157),
              .sr = Some(0));

  cpu.sr = FLAGS_DEF;
  cpu.sp = 0x65;
  cpu.pc = 0x1679;
  cpu.sr = FLAG_INT;
  exec_expect(rti, IMPL, 0,
              .sp = Some(0x68),
              .pc = Some(0x1157),
              .sr = Some(0));

  cpu.sr = FLAGS_DEF;
  cpu.sp = 0x66;
  cpu.sr = FLAGS_DEF & ~FLAG_BRK & ~FLAG_UNUSED;
  exec_expect(rts, IMPL, 0,
              .pc = Some(0x1158),
              .sr = Some(FLAGS_DEF),
              .sp = Some(0x68),
              );

  printf("Unit/jump_call: PASSED\n");
}
