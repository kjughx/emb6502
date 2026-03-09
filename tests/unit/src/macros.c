#include "test.h"
#include "lib.h"

bool macros(void) {
  /* Testing macros etc */

  byte_t v = 0b11001100;
  expect_eq(0b0, BIT_GET(v, 0));
  expect_eq(0b0, BIT_GET(v, 1));
  expect_eq(0b1, BIT_GET(v, 2));
  expect_eq(0b1, BIT_GET(v, 3));
  expect_eq(0b0, BIT_GET(v, 4));
  expect_eq(0b0, BIT_GET(v, 5));
  expect_eq(0b1, BIT_GET(v, 6));
  expect_eq(0b1, BIT_GET(v, 7));

  BIT_SET(v, 1, 1);
  expect_eq(0b1, BIT_GET(v, 1 << 1));

  printf("Unit/macros: PASSED\n");
}
