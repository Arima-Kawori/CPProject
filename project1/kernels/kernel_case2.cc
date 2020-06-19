#include "../run.h"

static float tmp_A[16][8];
void kernel_case2(float (&A)[16][8]) {
  for (int i=0; i<16; ++i) {
    for (int j=0; j<8; ++j) {
      tmp_A[i][j] = 0;
    }
  }
  for (int i=0; i<16; ++i) {
    for (int j=0; j<8; ++j) {
      tmp_A[i][j] = tmp_A[i][j] + A[i][j];
    }
  }
  for (int i=0; i<16; ++i) {
    for (int j=0; j<8; ++j) {
      tmp_A[i][j] = tmp_A[i][j] + 2;
    }
  }
  for (int i=0; i<16; ++i) {
    for (int j=0; j<8; ++j) {
      A[i][j] = tmp_A[i][j];
    }
  }
}
