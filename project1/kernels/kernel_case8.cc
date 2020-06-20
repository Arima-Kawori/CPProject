#include "../run.h"

static float tmp_B[8][16];
static float tmp_A[8][2][16];
void kernel_case8(float (&B)[8][16], float (&A)[8][2][16]) {
  for (int i=0; i<8; ++i) {
    for (int j=0; j<2; ++j) {
      for (int k=0; k<16; ++k) {
        tmp_A[i][j][k] = 0;
      }
    }
  }
  for (int i=0; i<8; ++i) {
    for (int j=0; j<2; ++j) {
      for (int k=0; k<16; ++k) {
        tmp_A[i][j][k] = tmp_A[i][j][k] + B[i][k];
      }
    }
  }
  for (int i=0; i<8; ++i) {
    for (int j=0; j<2; ++j) {
      for (int k=0; k<16; ++k) {
        A[i][j][k] = tmp_A[i][j][k];
      }
    }
  }
}
