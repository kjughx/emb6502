#include <stdint.h>
#include <stddef.h>

typedef uint8_t byte_t;
typedef uint16_t word_t;

struct {
    word_t pc;
    byte_t ac, x, y, sp, sr;
    uint64_t clk;
} cpu;

byte_t mode;
word_t arg;
word_t res;
word_t t1, t2;

#define FLAG_SET(flag) \
    cpu.sr |= (1 << (flag));

#define FLAG_CLR(flag) \
    cpu.sr &= ~(1 << (flag));

#define FLAG_GET(flag) \
    (!!(cpu.sr & (1 << (flag))))

#define SIGN_BIT 0b10000000

enum {
    FLAG_CARRY,
    FLAG_ZERO,
    FLAG_INT,
    FLAG_DEC,
    FLAG_BRK,
    UNUSED,
    FLAG_OF,
    FLAG_NEG,
};

enum {
    MODE_ACC,
    MODE_IMPL,
    MODE_IMM,
    MODE_REL,
    MODE_ZPG,
    MODE_ZPGX,
    MODE_ZPGY,
    MODE_ABS,
    MODE_ABSX,
    MODE_ABSY,
    MODE_IND,
    MODE_INDX,
    MODE_INDY,
};

byte_t memory[1 << 16] = {0};

void trap() {
    while(1) {}
}

byte_t read(word_t addr) {
    return memory[addr];
}

word_t write(word_t addr, byte_t val) {
    memory[addr] = val;
}

void pushb(byte_t b) {
    memory[cpu.sp--] = b;
}

void pushw(word_t w) {
    pushb(w >> 8);
    pushb(w & 0x00ff);
}

byte_t pullb() {
    return memory[cpu.sp++];
}

word_t pullw() {
    return (pullb() | (pullb() << 8));
}

byte_t next_pc() { return memory[cpu.pc++];}

void impl() {mode = MODE_IMPL;}

void *xxx = &trap;

void imm() {
    mode = MODE_IMM;
    arg = next_pc();
}

void zpg() {
    mode = MODE_ZPG;
    arg = next_pc();
}

void zpgx() {
    mode = MODE_ZPGX;
    arg = (next_pc() + cpu.x) & 0xff;
}
void zpgy() {
    mode = MODE_ZPGY;
    arg = (next_pc() + cpu.y) & 0xff;
}
void acc() {
    mode = MODE_ACC;
}
void rel() {
    mode = MODE_REL;
    arg = next_pc();
}
void abs() {
    mode = MODE_ABS;
    arg = (uint16_t)next_pc() | ((uint16_t)next_pc() << 8);
}
void absx() {
    mode = MODE_ABSX;
    arg = ((uint16_t)next_pc() | ((uint16_t)next_pc() << 8)) + cpu.x;
}

void absy() {
    mode = MODE_ABSY;
    arg = ((uint16_t)next_pc() | ((uint16_t)next_pc() << 8)) + cpu.y;
}
void ind() {
    mode = MODE_IND;
    arg = memory[memory[next_pc()] | (memory[next_pc()] << 8)];
}
void indx() {
    mode = MODE_INDX;
    byte_t ll = next_pc();
    arg = memory[(memory[ll + cpu.x]) | (memory[ll + cpu.x + 1] << 8)];
}
void indy() {
    mode = MODE_INDY;
    byte_t ll = next_pc();
    arg = (uint16_t)memory[memory[ll] | (memory[ll + 1] << 8)] + cpu.y;
}

word_t getvalue() {
    if (mode == MODE_ACC)
        return (word_t)cpu.ac;
    else
        return memory[arg];
}

void setvalue(byte_t v) {
    if (mode == MODE_ACC)
        cpu.ac = v;
    else
        memory[arg] = v;
}

static void (*get)()[] = {
 impl, indx, xxx, xxx, xxx, zpg, zpg, xxx, impl, imm, acc, xxx, xxx, abs, abs, xxx,
 rel, indy, xxx, xxx, xxx, zpgx, zpgx, xxx, impl, absy, xxx, xxx, xxx, absx, absx, xxx,
 abs, indx, xxx, xxx, zpg, zpg, zpg, xxx, impl, imm, acc, xxx, abs, abs, abs, xxx,
 rel, indy, xxx, xxx, xxx, zpgx, zpgx, xxx, impl, absy, xxx, xxx, xxx, absx, absx, xxx,
 impl, indx, xxx, xxx, xxx, zpg, zpg, xxx, impl, imm, acc, xxx, abs, abs, abs, xxx,
 rel, indy, xxx, xxx, xxx, zpgx, zpgx, xxx, impl, absy, xxx, xxx, xxx, absx, absx, xxx,
 impl, indx, xxx, xxx, xxx, zpg, zpg, xxx, impl, imm, acc, xxx, ind, abs, abs, xxx,
 rel, indy, xxx, xxx, xxx, zpgx, zpgx, xxx, impl, absy, xxx, xxx, xxx, absx, absx, xxx,
 xxx, indx, xxx, xxx, zpg, zpg, zpg, xxx, impl, xxx, impl, xxx, abs, abs, abs, xxx,
 rel, indy, xxx, xxx, zpgx, zpgx, zpgy, xxx, impl, absy, impl, xxx, xxx, absx, xxx, xxx,
 imm, indx, imm, xxx, zpg, zpg, zpg, xxx, impl, imm, impl, xxx, abs, abs, abs, xxx,
 rel, indy, xxx, xxx, zpgx, zpgx, zpgy, xxx, impl, absy, impl, xxx, absx, absx, absy, xxx,
 imm, indx, xxx, xxx, zpg, zpg, zpg, xxx, impl, imm, impl, xxx, abs, abs, abs, xxx,
 rel, indy, xxx, xxx, xxx, zpgx, zpgx, xxx, impl, absy, xxx, xxx, xxx, absx, absx, xxx,
 imm, indx, xxx, xxx, zpg, zpg, zpg, xxx, impl, imm, impl, xxx, abs, abs, abs, xxx,
 rel, indy, xxx, xxx, xxx, zpgx, zpgx, xxx, impl, absy, xxx, xxx, xxx, absx, absx, xxx,
};

#define checkneg(n) \
    cpu.sr = (cpu.sr & ~FLAG_NEG) | (!!((n) & SIGN_BIT) << FLAG_NEG);
#define checkzero(n) \
    cpu.sr = (cpu.sr & ~FLAG_ZERO) | (!!((n) == 0) << FLAG_ZERO);
#define checkcarry(n) \
    cpu.sr = (cpu.sr & ~FLAG_CARRY) | (!!((n) & 0xff00) << FLAG_CARRY);
#define checkof(r, l, n) \
    cpu.sr = (cpu.sr & ~FLAG_OF) | \
        (((!((r) & SIGN_BIT) & ((l) & SIGN_BIT)) ^ ((n) & SIGN_BIT)) << FLAG_OF);

void brk() {
    pushw(cpu.pc);
    pushb(cpu.sr | FLAG_BRK);
    // clk += 7;
}

void ora() {
    cpu.ac |= getvalue();
    checkneg(cpu.ac);
    checkzero(cpu.ac);
}

void asl() {
    res = getvalue() << 1;
    checkneg(res);
    checkzero(res);
    checkcarry(res);

    setvalue(res);
}

void php() {
    pushw(cpu.sr);
}

void bpl() {
    if (!FLAG_GET(FLAG_NEG))
        cpu.pc = arg;
}
void clc() {
    FLAG_CLR(FLAG_CARRY);
}
void jsr() {
    pushw(cpu.pc);
    cpu.pc = arg;
}
void and() {
    res = (word_t)cpu.ac & getvalue();
    checkneg(res);
    checkzero(res);
    setvalue(res);
}
void bit() {
    res = getvalue() & 0xff;
    cpu.sr = (cpu.sr & ~(FLAG_NEG | FLAG_OF)) | (res & FLAG_NEG) | (res & FLAG_OF);
    res &= (word_t)cpu.ac;
    checkzero(res);
}
void rol() {
    res = (word_t)getvalue() << 1 | FLAG_GET(FLAG_CARRY);
    checkneg(res);
    checkzero(res);
    checkcarry(res);
    setvalue(res & 0xff);
}
void plp() {
    cpu.sr = pullb();
}
void bmi() {
    if (FLAG_GET(FLAG_NEG))
        cpu.pc = arg;
}
void sec() {
    FLAG_SET(FLAG_CARRY);
}
void rti() {
    cpu.sr = pullb();
    cpu.pc = pullw();
}
void eor() {
    res = (word_t)cpu.ac ^ getvalue();
    checkneg(res);
    checkzero(res);
}
void lsr() {
    res = getvalue() >> 1;
    FLAG_CLR(FLAG_NEG);
    checkzero(res);
    checkcarry(res);
}
void pha() {
    pushb(cpu.ac);
}
void jmp() {
    cpu.pc = arg;
}
void bvc() {
    if (!FLAG_GET(FLAG_OF))
        cpu.pc = arg;
}
void cli() {
    FLAG_SET(FLAG_INT);
}
void rts() {
    cpu.pc = pullw();
}
void adc() {
    t1 = getvalue();
    t2 = cpu.ac;
    res = t1 + t2 + FLAG_GET(FLAG_CARRY);
    checkneg(res);
    checkzero(res);
    checkof(t1, t2, res);
    setvalue(res);
}
void ror() {
    res = (word_t)getvalue() >> 1 | FLAG_GET(FLAG_CARRY);
    checkneg(res);
    checkzero(res);
    checkcarry(res);
    setvalue(res & 0xff);
}

static void (*ops)()[] = {
    brk,  ora,  xxx,  xxx,  xxx,  ora,  asl,  xxx,  php,  ora,  asl,  xxx,  xxx,  ora,  asl,  xxx,
    bpl,  ora,  xxx,  xxx,  xxx,  ora,  asl,  xxx,  clc,  ora,  xxx,  xxx,  xxx,  ora,  asl,  xxx,
    jsr,  and,  xxx,  xxx,  bit,  and,  rol,  xxx,  plp,  and,  rol,  xxx,  bit,  and,  rol,  xxx,
    bmi,  and,  xxx,  xxx,  xxx,  and,  rol,  xxx,  sec,  and,  xxx,  xxx,  xxx,  and,  rol,  xxx,
    rti,  eor,  xxx,  xxx,  xxx,  eor,  lsr,  xxx,  pha,  eor,  lsr,  xxx,  jmp,  eor,  lsr,  xxx,
    bvc,  eor,  xxx,  xxx,  xxx,  eor,  lsr,  xxx,  cli,  eor,  xxx,  xxx,  xxx,  eor,  lsr,  xxx,
    rts,  adc,  xxx,  xxx,  xxx,  adc,  ror,  xxx,  pla,  adc,  ror,  xxx,  jmp,  adc,  ror,  xxx,
    bvs,  adc,  xxx,  xxx,  xxx,  adc,  ror,  xxx,  sei,  adc,  xxx,  xxx,  xxx,  adc,  ror,  xxx,
    xxx,  sta,  xxx,  xxx,  sty,  sta,  stx,  xxx,  dey,  xxx,  txa,  xxx,  sty,  sta,  stx,  xxx,
    bcc,  sta,  xxx,  xxx,  sty,  sta,  stx,  xxx,  tya,  sta,  txs,  xxx,  xxx,  sta,  xxx,  xxx,
    ldy,  lda,  ldx,  xxx,  ldy,  lda,  ldx,  xxx,  tay,  lda,  tax,  xxx,  ldy,  lda,  ldx,  xxx,
    bcs,  lda,  xxx,  xxx,  ldy,  lda,  ldx,  xxx,  clv,  lda,  tsx,  xxx,  ldy,  lda,  ldx,  xxx,
    cpy,  cmp,  xxx,  xxx,  cpy,  cmp,  dec,  xxx,  iny,  cmp,  dex,  xxx,  cpy,  cmp,  dec,  xxx,
    bne,  cmp,  xxx,  xxx,  xxx,  cmp,  dec,  xxx,  cld,  cmp,  xxx,  xxx,  xxx,  cmp,  dec,  xxx,
    cpx,  sbc,  xxx,  xxx,  cpx,  sbc,  inc,  xxx,  inx,  sbc,  nop,  xxx,  cpx,  sbc,  inc,  xxx,
    beq,  sbc,  xxx,  xxx,  xxx,  sbc,  inc,  xxx,  sed,  sbc,  xxx,  xxx,  xxx,  sbc,  inc,  xxx,
};

int main(void) {
    int opc;
    while(1) {
        opc = next_pc();
        (*get[op])();
        (*ops[op])();
    }

    main();
}
