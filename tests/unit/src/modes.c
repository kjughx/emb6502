#include "lib.h"
#include "test.h"

word_t getvalue();

#define None ((Option) { .data = 0 })

#define get_expect(get, v, ...) __get_expect((get), v, ((State) { __VA_ARGS__ }), __FILE__, __LINE__)
void __get_expect(void (*get)(void), Option value, State s, const char *file, int line) {
  struct cpu cpu_cp = cpu;
  save_mem();
  (get)();
  compare_state(cpu_cp, s, file, line);
  if (is_some(value)) {
    __expect_eq(value.byte, getvalue(), file, line);
  }
}

void impl();
void imm();
void zpg();
void zpgx();
void zpgy();
void acc();
void rel();
void _abs();
void absx();
void absy();
void ind();
void indx();
void indy();

static uint8_t _memory[MEMORY_SIZE] = {
  [0xab] = 0x42,
  [0xac] = 0x43,
  [0xad] = 0x44,
  [0x4344] = 0x69,
  [0x4443] = 0x99,
  [0x4444] = 0xaa,
  [0x4645] = 0x66,
  [0x4646] = 0x67,
  [0xaa99] = 0xfb,
  [0xcdab] = 0x45,
  [0xcdac] = 0x46,
  [0xcdad] = 0x47,
  [0x1155] = 0xab,
  [0x1156] = 0xcd,
};

void mode_impl(void) {
  get_expect(impl, None);
}

void mode_imm(void) {
  cpu.pc = 0x1155;
  get_expect(imm, Some(0xab),
             .pc = Some(0x1156));
}

void mode_zpg(void) {
  cpu.pc = 0x1155;
  get_expect(zpg, Some(0x42), .pc = Some(0x1156));
}

void mode_zpgx(void) {
  cpu.pc = 0x1155;
  cpu.x = 1;
  get_expect(zpgx, Some(0x43), .pc = Some(0x1156));
}

void mode_zpgy(void) {
  cpu.pc = 0x1155;
  cpu.y = 2;
  get_expect(zpgy, Some(0x44), .pc = Some(0x1156));
}

void mode_acc(void) {
  cpu.ac = 0xaa;
  get_expect(acc, Some(0xaa));
}

void mode_rel(void) {
  mode_imm();
}

void mode_abs(void) {
  cpu.pc = 0x1155;
  get_expect(_abs, Some(0x45), .pc = Some(0x1157));
}

void mode_absx(void) {
  cpu.pc = 0x1155;
  cpu.x = 1;
  get_expect(absx, Some(0x46), .pc = Some(0x1157));
}

void mode_absy(void) {
  cpu.pc = 0x1155;
  cpu.y = 2;
  get_expect(absy, Some(0x47), .pc = Some(0x1157));
}

void mode_ind(void) {
  cpu.pc = 0x1155;
  get_expect(ind, Some(0x66), .pc = Some(0x1157));
}

void mode_indx(void) {
  cpu.pc = 0x1155;
  cpu.x = 1;
  get_expect(indx, Some(0x99), .pc = Some(0x1156));
}

void mode_indy(void) {
  cpu.pc = 0x1155;
  cpu.y = 2;
  get_expect(indy, Some(0x69), .pc = Some(0x1156));
}

bool modes(void) {
  load_mem(_memory);

  mode_impl();
  mode_imm();
  mode_acc();
  mode_zpg();
  mode_zpgx();
  mode_zpgy();
  mode_rel();
  mode_abs();
  mode_absx();
  mode_absy();
  mode_ind();
  mode_indx();
  mode_indy();

  printf("Unit/modes: PASSED\n");
}
