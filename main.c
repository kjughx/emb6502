#include <stdint.h>
struct cpu {
  uint16_t pc;
  uint8_t ac, x, y, sp, sr;
  uint64_t clk;
};

extern struct cpu cpu;
extern uint8_t memory[1 << 16];

void step();
void reset();

int main(void) {
    return 0;
}
