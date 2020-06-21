#include "../run2.h"

static float tmp_dA[8][8];
static float tmp_dB[10][8];
void grad_case10(float (&dA)[8][8], float (&dB)[10][8]) {
  for (int i=0; i<8; ++i) {
    for (int j=0; j<8; ++j) {
      dB[i][j] = dB[i][j] + dA[i][j] * (((1)) * (3.0) / ((3.0) * (3.0)));
    }
  }
  for (int i=0; i<8; ++i) {
    for (int j=0; j<8; ++j) {
      dB[i + 1][j] = dB[i + 1][j] + dA[i][j] * (((1)) * (3.0) / ((3.0) * (3.0)));
    }
  }
  for (int i=0; i<8; ++i) {
    for (int j=0; j<8; ++j) {
      dB[i + 2][j] = dB[i + 2][j] + dA[i][j] * (((1)) * (3.0) / ((3.0) * (3.0)));
    }
  }
}
