#include "../run2.h"

static float tmp_dA[8][8];
static float tmp_dB[10][8];
void grad_case10(float (&dA)[8][8], float (&dB)[10][8]) {
  for (int i=0; i<8; ++i) {
    for (int j=0; j<8; ++j) {
      dB[i][j] = dB[i][j] + dA[i][j] * (((1)) * (3.0) / ((3.0) * (3.0)));
    }
  }
  for (int iadd1=0; iadd1<10; ++iadd1) {
    for (int j=0; j<8; ++j) {
      dB[iadd1][j] = dB[iadd1][j] + dA[iadd1 - 1][j] * (((1)) * (3.0) / ((3.0) * (3.0)));
    }
  }
  for (int iadd2=0; iadd2<10; ++iadd2) {
    for (int j=0; j<8; ++j) {
      dB[iadd2][j] = dB[iadd2][j] + dA[iadd2 - 2][j] * (((1)) * (3.0) / ((3.0) * (3.0)));
    }
  }
}
