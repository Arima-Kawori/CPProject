#include "../run2.h"

static float tmp_dB[32];
static float tmp_dA[2][16];
void grad_case8(float (&dB)[32], float (&dA)[2][16]) {
  for (int i=0; i<2; ++i) {
    for (int j=0; j<16; ++j) {
      tmp_dA[i][j] = 0;
    }
  }
  for (int i=0; i<2; ++i) {
    for (int j=0; j<16; ++j) {
      tmp_dA[i][j] = tmp_dA[i][j] + dB[i * 16 + j];
    }
  }
  for (int i=0; i<2; ++i) {
    for (int j=0; j<16; ++j) {
      dA[i][j] = tmp_dA[i][j];
    }
  }
}
