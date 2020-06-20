#include "../run.h"

static float tmp_B[10][10];
static float tmp_A[8][8];
void kernel_case10(float (&B)[10][10], float (&A)[8][8]) {
  for (int i=0; i<10; ++i) {
    for (int j=0; j<10; ++j) {
      tmp_A[i][j] = 0;
    }
  }
  for (int i=0; i<10; ++i) {
    for (int j=0; j<10; ++j) {
      tmp_A[i][j] = tmp_A[i][j] + (B[i][j] + B[i + 1][j] + B[i + 2][j]) / 3;
    }
  }
  for (int i=0; i<10; ++i) {
    for (int j=0; j<10; ++j) {
      A[i][j] = tmp_A[i][j];
    }
  }
}
