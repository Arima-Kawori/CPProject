#include "../run2.h"

static float tmp_A[4][16];
static float tmp_B[4][16];
void grad_case2(float (&A)[4][16], float (&B)[4][16]) {
  for (int i=0; i<4; ++i) {
    for (int j=0; j<16; ++j) {
      tmp_B[i][j] = 0;
    }
  }
  for (int i=0; i<4; ++i) {
    for (int j=0; j<16; ++j) {
      tmp_B[i][j] = tmp_B[i][j] + A[i][j] * A[i][j];
    }
  }
  for (int i=0; i<4; ++i) {
    for (int j=0; j<16; ++j) {
      tmp_B[i][j] = tmp_B[i][j] + 1;
    }
  }
  for (int i=0; i<4; ++i) {
    for (int j=0; j<16; ++j) {
      B[i][j] = tmp_B[i][j];
    }
  }
}
