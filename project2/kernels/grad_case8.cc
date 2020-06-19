#include "../run2.h"

static float tmp_A[2][16];
static float tmp_B[32];
void grad_case8(float (&A)[2][16], float (&B)[32]) {
  for (int i=0; i<16; ++i) {
    tmp_B[i] = 0;
  }
  for (int i=0; i<16; ++i) {
    tmp_B[i] = tmp_B[i] + A[i / 16][i % 16];
  }
  for (int i=0; i<16; ++i) {
    B[i] = tmp_B[i];
  }
}
