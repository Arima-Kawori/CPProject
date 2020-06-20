#include "../run.h"

static float tmp_A[32][16];
void kernel_case1(float (&A)[32][16]) {
  for (int i=0; i<32; ++i) {
    for (int j=0; j<16; ++j) {
      tmp_A[i][j] = 0;
    }
  }
  for (int i=0; i<32; ++i) {
    for (int j=0; j<16; ++j) {
      tmp_A[i][j] = tmp_A[i][j] + 2;
    }
  }
  for (int i=0; i<32; ++i) {
    for (int j=0; j<16; ++j) {
      A[i][j] = tmp_A[i][j];
    }
  }
}
