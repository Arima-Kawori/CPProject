#include "../run2.h"

static float tmp_dA[8][8];
static float tmp_dB[10][8];
void grad_case10(float (&dA)[8][8], float (&dB)[10][8]) {
  for (int i=0; i<10; ++i) {
    for (int j=0; j<8; ++j) {
      tmp_dB[i][j] = 0;
    }
  }
  for (int i=0; i<10; ++i) {
    for (int j=0; j<8; ++j) {
      tmp_dB[i][j] = tmp_dB[i][j] + ((((i >= 0) && (i < 8)) ? (dA[i][j]) : (0)) + (((i >= 1) && (i < 9)) ? (dA[i - 1][j]) : (0)) + (((i >= 2) && (i < 10)) ? (dA[i - 2][j]) : (0))) / 3;
    }
  }
  for (int i=0; i<10; ++i) {
    for (int j=0; j<8; ++j) {
      dB[i][j] = tmp_dB[i][j];
    }
  }
}
