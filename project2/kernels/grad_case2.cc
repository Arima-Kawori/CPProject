#include "../run2.h"

static float tmp_A[4][16];
static float tmp_dB[4][16];
static float tmp_dA[4][16];
void grad_case2(float (&A)[4][16], float (&dB)[4][16], float (&dA)[4][16]) {
  for (int i=0; i<4; ++i) {
    for (int j=0; j<16; ++j) {
      dA[i][j] = dA[i][j] + dB[i][j] * (A[i][j]);
    }
  }
  for (int i=0; i<4; ++i) {
    for (int j=0; j<16; ++j) {
      dA[i][j] = dA[i][j] + dB[i][j] * (A[i][j]);
    }
  }
}
