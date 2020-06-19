#include "../run2.h"

static float tmp_B[10][8];
static float tmp_A[8][8];
void grad_case10(float (&B)[10][8], float (&A)[8][8]) {
  for (int i=0; i<8; ++i) {
    for (int j=0; j<8; ++j) {
      tmp_A[i][j] = 0;
    }
  }
  for (int i=0; i<8; ++i) {
    for (int j=0; j<8; ++j) {
      tmp_A[i][j] = tmp_A[i][j] + (B[i][j] + B[i + 1][j] + B[i + 2][j]) / 3;
    }
  }
  for (int i=0; i<8; ++i) {
    for (int j=0; j<8; ++j) {
      A[i][j] = tmp_A[i][j];
    }
  }
}
