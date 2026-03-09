#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "lib.h"

bool trace_enabled = false;
#define trace(fmt, ...) do {                    \
    if (trace_enabled)                          \
      printf(fmt, ##__VA_ARGS__);     \
  } while(0)

#define trace_ins(pc, opc) do {                              \
    trace("0x%04x: %3s (%02X)", (pc), ins[(opc)], (opc));    \
    if (mode != MODE_IMPL && mode != MODE_ACC) {             \
      if (mode == MODE_IMM)                                  \
        trace(" #");                                         \
      else                                                   \
        trace(" $");                                         \
      trace("0x%04x (%s)", arg, modes_str[mode]);                \
    }                                                                   \
    trace("\n");                                                        \
    trace("\tA: 0x%02x, X: 0x%02x, Y: 0x%02x, SP: 0x%02x\n", cpu.ac, cpu.x, cpu.y, cpu.sp); \
    trace("\tN: %b, V: %b, D: %b, I: %b, Z: %b, C: %b\n",               \
          FLAG_GET(FLAG_NEG), FLAG_GET(FLAG_OF), FLAG_GET(FLAG_DEC),    \
          FLAG_GET(FLAG_INT), FLAG_GET(FLAG_ZERO),                      \
          FLAG_GET(FLAG_CARRY));                                        \
  } while(0)                                                            \

void abort(void);

extern struct cpu cpu;
extern byte_t memory[65536];

enum Mode mode;
word_t arg;
word_t res;
word_t t1, t2;

const char *modes_str[] = {
  "acc",
  "impl",
  "imm",
  "rel",
  "zpg",
  "zpgx",
  "zpgy",
  "abs",
  "absx",
  "absy",
  "ind",
  "indx",
  "indy",
};

void trap() {
  trace("TRAP: cpu.pc: 0x%2x\n", cpu.pc);
  abort();
}

byte_t readb(word_t addr) { return memory[addr]; }
word_t readw(word_t addr) { return (memory[addr] | (memory[addr + 1] << 8)); }

void write(word_t addr, byte_t val) { memory[addr] = val; }

void pushb(byte_t b) { write(STACK_START + cpu.sp--, b); }
void pushw(word_t w) {
  pushb(w >> 8);
  pushb(w & 0x00ff);
}

byte_t pullb() { return readb(STACK_START + ++cpu.sp); }
word_t pullw() { return (pullb() | (pullb() << 8)); }

byte_t next_pc() { return readb(cpu.pc++); }

void impl() { mode = MODE_IMPL; }

void imm() {
 mode = MODE_IMM;
 arg = next_pc();
}

void zpg() {
  mode = MODE_ZPG;
  arg = next_pc() & 0xff;
}

void zpgx() {
  mode = MODE_ZPGX;
  arg = ((uint8_t)next_pc() + (uint8_t)cpu.x) & 0xff; // wrap-around
}
void zpgy() {
  mode = MODE_ZPGY;
  arg = ((uint8_t)next_pc() + (uint8_t)cpu.y) & 0xff; // wrap-around
}
void acc() { mode = MODE_ACC; }
void rel() {
  mode = MODE_REL;
  arg = cpu.pc + (int8_t)next_pc() + 1;
}
void abs() {
  mode = MODE_ABS;
  arg = (uint16_t)next_pc() | ((uint16_t)next_pc() << 8);
}
void _abs() { abs(); }; // For testing
void absx() {
  mode = MODE_ABSX;
  arg = ((uint16_t)next_pc() | ((uint16_t)next_pc() << 8)) + (uint16_t)cpu.x;
}

void absy() {
  mode = MODE_ABSY;
  arg = ((uint16_t)next_pc() | ((uint16_t)next_pc() << 8)) + (uint16_t)cpu.y;
}
void ind() {
  mode = MODE_IND;
  word_t indi = next_pc() | (next_pc() << 8);
  arg = readw(indi);
}
void indx() {
  mode = MODE_INDX;
  byte_t ll = next_pc();
  arg = memory[(byte_t)(ll + cpu.x)] | (memory[ll + cpu.x + 1] << 8);
}
void indy() {
  mode = MODE_INDY;
  byte_t ll = next_pc();
  arg = (memory[ll] | (memory[ll + 1] << 8)) + cpu.y;
}

static void (*get[])(void) = {
    impl, indx, trap, trap, trap, zpg,  zpg,  trap, impl, imm,  acc,  trap,
    trap, abs,  abs,  trap, rel,  indy, trap, trap, trap, zpgx, zpgx, trap,
    impl, absy, trap, trap, trap, absx, absx, trap, abs,  indx, trap, trap,
    zpg,  zpg,  zpg,  trap, impl, imm,  acc,  trap, abs,  abs,  abs,  trap,
    rel,  indy, trap, trap, trap, zpgx, zpgx, trap, impl, absy, trap, trap,
    trap, absx, absx, trap, impl, indx, trap, trap, trap, zpg,  zpg,  trap,
    impl, imm,  acc,  trap, abs,  abs,  abs,  trap, rel,  indy, trap, trap,
    trap, zpgx, zpgx, trap, impl, absy, trap, trap, trap, absx, absx, trap,
    impl, indx, trap, trap, trap, zpg,  zpg,  trap, impl, imm,  acc,  trap,
    ind,  abs,  abs,  trap, rel,  indy, trap, trap, trap, zpgx, zpgx, trap,
    impl, absy, trap, trap, trap, absx, absx, trap, trap, indx, trap, trap,
    zpg,  zpg,  zpg,  trap, impl, trap, impl, trap, abs,  abs,  abs,  trap,
    rel,  indy, trap, trap, zpgx, zpgx, zpgy, trap, impl, absy, impl, trap,
    trap, absx, trap, trap, imm,  indx, imm,  trap, zpg,  zpg,  zpg,  trap,
    impl, imm,  impl, trap, abs,  abs,  abs,  trap, rel,  indy, trap, trap,
    zpgx, zpgx, zpgy, trap, impl, absy, impl, trap, absx, absx, absy, trap,
    imm,  indx, trap, trap, zpg,  zpg,  zpg,  trap, impl, imm,  impl, trap,
    abs,  abs,  abs,  trap, rel,  indy, trap, trap, trap, zpgx, zpgx, trap,
    impl, absy, trap, trap, trap, absx, absx, trap, imm,  indx, trap, trap,
    zpg,  zpg,  zpg,  trap, impl, imm,  impl, trap, abs,  abs,  abs,  trap,
    rel,  indy, trap, trap, trap, zpgx, zpgx, trap, impl, absy, trap, trap,
    trap, absx, absx, trap,
};

#define checkneg(n)                                                            \
  cpu.sr = (cpu.sr & ~FLAG_NEG) | (BIT_GET((n), SIGN_BIT) << BIT_NEG);
#define checkzero(n)                                                           \
  cpu.sr = (cpu.sr & ~FLAG_ZERO) | (!!((n) == 0) << BIT_ZERO);
#define checkcarry(n)                                                          \
  cpu.sr = (cpu.sr & ~FLAG_CARRY) | (!!((n) & 0xff00) << BIT_CARRY);
#define checkof(r, l, n)                                                       \
  cpu.sr = (cpu.sr & ~FLAG_OF) |                                               \
           (BIT_GET(~((l) ^ (r)) & ((l) ^ (n)), SIGN_BIT) << BIT_OF);

word_t getvalue() {
  if (mode == MODE_ACC)
    return (word_t)cpu.ac;
  else if (mode == MODE_IMM)
    return (word_t)arg;
  else
    return memory[arg];
}

void setvalue(byte_t v) {
  if (mode == MODE_ACC)
    cpu.ac = v;
  else
    memory[arg] = v;
}

void brk() {
  pushw(cpu.pc + 1);
  pushb(cpu.sr | FLAG_BRK | FLAG_UNUSED);
  FLAG_SET(FLAG_INT);
  cpu.pc = readw(IRQ_VEC);
}
void ora() {
  cpu.ac |= getvalue();
  checkneg(cpu.ac);
  checkzero(cpu.ac);
}
void asl() {
  res = getvalue() << 1;
  BIT_SET(cpu.sr, BIT_GET(getvalue(), SIGN_BIT), BIT_CARRY);
  checkneg(res & 0xff);
  checkzero(res & 0xff);
  setvalue(res);
}
void php() { pushb(cpu.sr | FLAG_BRK | FLAG_UNUSED); }
void bpl() {
  if (!FLAG_GET(FLAG_NEG))
    cpu.pc = arg;
}
void clc() { FLAG_CLR(FLAG_CARRY); }
void jsr() {
  pushw(cpu.pc - 1);
  cpu.pc = arg;
}
void and () {
  cpu.ac &= getvalue();
  checkzero(cpu.ac);
  checkneg(cpu.ac);
}
void bit() {
  res = getvalue() & 0xff;
  cpu.sr =
      (cpu.sr & ~(FLAG_NEG | FLAG_OF)) | (res & FLAG_NEG) | (res & FLAG_OF);
  res &= (word_t)cpu.ac;
  checkzero(res & 0xff);
}
void rol() {
  res = (word_t)getvalue() << 1 | FLAG_GET(FLAG_CARRY);
  checkneg(res);
  checkzero(res & 0xff);
  checkcarry(res);
  setvalue(res & 0xff);
}
void plp() { cpu.sr = pullb() & ~FLAG_BRK & ~FLAG_UNUSED; }
void bmi() {
  if (FLAG_GET(FLAG_NEG))
    cpu.pc = arg;
}
void sec() { FLAG_SET(FLAG_CARRY); }
void rti() {
  cpu.sr = pullb() & ~FLAG_BRK & ~FLAG_UNUSED;
  cpu.pc = pullw();
  if (cpu.nmi) cpu.nmi = 0;
  if (!cpu.nmi && cpu.irq) cpu.irq = 0;
}
void eor() {
  res = (byte_t)cpu.ac ^ getvalue();
  checkneg(res);
  checkzero(res & 0xff);
  cpu.ac = res;
}
void lsr() {
  res = getvalue() >> 1;
  FLAG_CLR(FLAG_NEG);
  checkzero(res & 0xff);
  BIT_SET(cpu.sr, BIT_GET(getvalue(), BIT_CARRY), BIT_CARRY);
  setvalue(res);
}
void pha() { pushb(cpu.ac); }
void jmp() { cpu.pc = arg; }
void bvc() {
  if (!FLAG_GET(FLAG_OF))
    cpu.pc = arg;
}
void cli() { FLAG_CLR(FLAG_INT); }
void rts() { cpu.pc = pullw() + 1; }
void adc() {
  res = cpu.ac + getvalue() + FLAG_GET(FLAG_CARRY);
  checkcarry(res);
  checkneg(res & 0xff);
  checkzero(res & 0xff);
  checkof(cpu.ac, getvalue(), res & 0xff);
  cpu.ac = res;
}
void ror() {
  res = (word_t)getvalue() >> 1 | (FLAG_GET(FLAG_CARRY) << 7);
  checkneg(res & 0xff);
  checkzero(res & 0xff);
  BIT_SET(cpu.sr, BIT_GET(getvalue(), BIT_CARRY), BIT_CARRY);
  setvalue(res & 0xff);
}
void pla() {
  cpu.ac = pullb();
  checkneg(cpu.ac);
  checkzero(cpu.ac);
}
void bvs() {
  if (FLAG_GET(FLAG_OF))
    cpu.pc = arg;
}
void sei() { FLAG_SET(FLAG_INT); }
void sta() { setvalue(cpu.ac); }
void sty() { setvalue(cpu.y); }
void stx() { setvalue(cpu.x); }
void dey() {
  cpu.y--;
  checkneg(cpu.y);
  checkzero(cpu.y);
}
void txa() {
  cpu.ac = cpu.x;
  checkneg(cpu.ac);
  checkzero(cpu.ac);
}
void bcc() {
  if (!FLAG_GET(FLAG_CARRY))
    cpu.pc = arg;
}
void tya() {
  cpu.ac = cpu.y;
  checkneg(cpu.ac);
  checkzero(cpu.ac);
}
void txs() { cpu.sp = cpu.x; }
void ldy() {
  cpu.y = getvalue();
  checkneg(cpu.y);
  checkzero(cpu.y);
}
void lda() {
  cpu.ac = getvalue();
  checkneg(cpu.ac);
  checkzero(cpu.ac);
}
void ldx() {
  cpu.x = getvalue();
  checkneg(cpu.x);
  checkzero(cpu.x);
}
void tay() {
  cpu.y = cpu.ac;
  checkneg(cpu.y);
  checkzero(cpu.y);
}
void tax() {
  cpu.x = cpu.ac;
  checkneg(cpu.x);
  checkzero(cpu.x);
}
void bcs() {
  if (FLAG_GET(FLAG_CARRY))
    cpu.pc = arg;
}
void clv() { FLAG_CLR(FLAG_OF); }
void tsx() {
  cpu.x = cpu.sp;
  checkneg(cpu.x);
  checkzero(cpu.x);
}
void cpy() {
  res = cpu.y - getvalue();
  checkneg(res);
  BIT_SET(cpu.sr, cpu.y >= getvalue(), BIT_CARRY);
  BIT_SET(cpu.sr, cpu.y == getvalue(), BIT_ZERO);
}
void cmp() {
  res = cpu.ac - getvalue();
  checkneg(res);
  BIT_SET(cpu.sr, cpu.ac >= getvalue(), BIT_CARRY);
  BIT_SET(cpu.sr, cpu.ac == getvalue(), BIT_ZERO);
}
void dec() {
  res = getvalue() - 1;
  checkneg(res);
  checkzero(res & 0xff);
  setvalue(res);
}
void iny() {
  cpu.y++;
  checkneg(cpu.y);
  checkzero(cpu.y);
}
void dex() {
  cpu.x--;
  checkneg(cpu.x);
  checkzero(cpu.x);
}
void bne() {
  if (!FLAG_GET(FLAG_ZERO))
    cpu.pc = arg;
}
void cld() { FLAG_CLR(FLAG_DEC); }
void cpx() {
  res = cpu.x - getvalue();
  BIT_SET(cpu.sr, BIT_GET(res, SIGN_BIT), BIT_NEG);
  BIT_SET(cpu.sr, (int8_t)cpu.x >= (int8_t)getvalue(),      BIT_CARRY);
  BIT_SET(cpu.sr, cpu.x == getvalue(),      BIT_ZERO);
}
void sbc() {
  res = cpu.ac - getvalue() - !FLAG_GET(FLAG_CARRY);
  checkneg(res);
  checkzero(res);
  checkof(cpu.ac, getvalue(), res);
  cpu.ac = res;
}
void inc() {
  res = (getvalue() + 1) & 0xff;
  checkneg(res);
  checkzero(res);
  setvalue(res);
}
void inx() {
  cpu.x++;
  checkneg(cpu.x);
  checkzero(cpu.x);
}
void nop() {}
void beq() {
  if (FLAG_GET(FLAG_ZERO)) {
    cpu.pc = arg;
  }
}
void sed() { FLAG_SET(FLAG_DEC); }

static void (*ops[])() = {
    brk,  ora, trap, trap, trap, ora,  asl,  trap, php,  ora, asl,  trap,
    trap, ora, asl,  trap, bpl,  ora,  trap, trap, trap, ora, asl,  trap,
    clc,  ora, trap, trap, trap, ora,  asl,  trap, jsr,  and, trap, trap,
    bit,  and, rol,  trap, plp,  and,  rol,  trap, bit,  and, rol,  trap,
    bmi,  and, trap, trap, trap, and,  rol,  trap, sec,  and, trap, trap,
    trap, and, rol,  trap, rti,  eor,  trap, trap, trap, eor, lsr,  trap,
    pha,  eor, lsr,  trap, jmp,  eor,  lsr,  trap, bvc,  eor, trap, trap,
    trap, eor, lsr,  trap, cli,  eor,  trap, trap, trap, eor, lsr,  trap,
    rts,  adc, trap, trap, trap, adc,  ror,  trap, pla,  adc, ror,  trap,
    jmp,  adc, ror,  trap, bvs,  adc,  trap, trap, trap, adc, ror,  trap,
    sei,  adc, trap, trap, trap, adc,  ror,  trap, trap, sta, trap, trap,
    sty,  sta, stx,  trap, dey,  trap, txa,  trap, sty,  sta, stx,  trap,
    bcc,  sta, trap, trap, sty,  sta,  stx,  trap, tya,  sta, txs,  trap,
    trap, sta, trap, trap, ldy,  lda,  ldx,  trap, ldy,  lda, ldx,  trap,
    tay,  lda, tax,  trap, ldy,  lda,  ldx,  trap, bcs,  lda, trap, trap,
    ldy,  lda, ldx,  trap, clv,  lda,  tsx,  trap, ldy,  lda, ldx,  trap,
    cpy,  cmp, trap, trap, cpy,  cmp,  dec,  trap, iny,  cmp, dex,  trap,
    cpy,  cmp, dec,  trap, bne,  cmp,  trap, trap, trap, cmp, dec,  trap,
    cld,  cmp, trap, trap, trap, cmp,  dec,  trap, cpx,  sbc, trap, trap,
    cpx,  sbc, inc,  trap, inx,  sbc,  nop,  trap, cpx,  sbc, inc,  trap,
    beq,  sbc, trap, trap, trap, sbc,  inc,  trap, sed,  sbc, trap, trap,
    trap, sbc, inc,  trap,
};

static char* ins[] = {
    "brk",  "ora", "trap", "trap", "trap", "ora",  "asl",  "trap", "php",  "ora", "asl",  "trap",
    "trap", "ora", "asl",  "trap", "bpl",  "ora",  "trap", "trap", "trap", "ora", "asl",  "trap",
    "clc",  "ora", "trap", "trap", "trap", "ora",  "asl",  "trap", "jsr",  "and", "trap", "trap",
    "bit",  "and", "rol",  "trap", "plp",  "and",  "rol",  "trap", "bit",  "and", "rol",  "trap",
    "bmi",  "and", "trap", "trap", "trap", "and",  "rol",  "trap", "sec",  "and", "trap", "trap",
    "trap", "and", "rol",  "trap", "rti",  "eor",  "trap", "trap", "trap", "eor", "lsr",  "trap",
    "pha",  "eor", "lsr",  "trap", "jmp",  "eor",  "lsr",  "trap", "bvc",  "eor", "trap", "trap",
    "trap", "eor", "lsr",  "trap", "cli",  "eor",  "trap", "trap", "trap", "eor", "lsr",  "trap",
    "rts",  "adc", "trap", "trap", "trap", "adc",  "ror",  "trap", "pla",  "adc", "ror",  "trap",
    "jmp",  "adc", "ror",  "trap", "bvs",  "adc",  "trap", "trap", "trap", "adc", "ror",  "trap",
    "sei",  "adc", "trap", "trap", "trap", "adc",  "ror",  "trap", "trap", "sta", "trap", "trap",
    "sty",  "sta", "stx",  "trap", "dey",  "trap", "txa",  "trap", "sty",  "sta", "stx",  "trap",
    "bcc",  "sta", "trap", "trap", "sty",  "sta",  "stx",  "trap", "tya",  "sta", "txs",  "trap",
    "trap", "sta", "trap", "trap", "ldy",  "lda",  "ldx",  "trap", "ldy",  "lda", "ldx",  "trap",
    "tay",  "lda", "tax",  "trap", "ldy",  "lda",  "ldx",  "trap", "bcs",  "lda", "trap", "trap",
    "ldy",  "lda", "ldx",  "trap", "clv",  "lda",  "tsx",  "trap", "ldy",  "lda", "ldx",  "trap",
    "cpy",  "cmp", "trap", "trap", "cpy",  "cmp",  "dec",  "trap", "iny",  "cmp", "dex",  "trap",
    "cpy",  "cmp", "dec",  "trap", "bne",  "cmp",  "trap", "trap", "trap", "cmp", "dec",  "trap",
    "cld",  "cmp", "trap", "trap", "trap", "cmp",  "dec",  "trap", "cpx",  "sbc", "trap", "trap",
    "cpx",  "sbc", "inc",  "trap", "inx",  "sbc",  "nop",  "trap", "cpx",  "sbc", "inc",  "trap",
    "beq",  "sbc", "trap", "trap", "trap", "sbc",  "inc",  "trap", "sed",  "sbc", "trap", "trap",
    "trap", "sbc", "inc",  "trap",
};

int step() {
  if (cpu.nmi)
    cpu.pc = readw(NMI_VEC);
  else if (cpu.irq && FLAG_GET(FLAG_INT))
    cpu.pc = readw(IRQ_VEC);

  word_t oldpc = cpu.pc;
  byte_t opc = next_pc();
  (*get[opc])();
  (*ops[opc])();

  /* It's important that the macro `trace_ins` is here because it depends on the globals
     - mode
     - arg
     and the state of `cpu` after the operation
  */
  trace_ins(oldpc, opc);

  if (cpu.pc == oldpc) return 1;
  return 0;
}

void irq() {
  cpu.irq = true;
}

void nmi() {
  cpu.nmi = true;
}

void reset() {
  cpu.pc = readw(RST_VEC);
  cpu.sp = 0xff;
  cpu.sr = 0;
}
