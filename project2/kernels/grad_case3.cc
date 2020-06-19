#include "../run2.h"

static float tmp_A[4][16];
static float tmp_B[16][16];
static float tmp_C[4][16];
void grad_case3(float (&A)[4][16], float (&B)[16][16], float (&C)[4][16]) {
  for (int i=0; i<4; ++i) {
    for (int j=0; j<16; ++j) {
      tmp_C[i][j] = 0;
    }
  }
  for (int i=0; i<4; ++i) {
    for (int j=0; j<16; ++j) {
      for (int k=0; k<16; ++k) {
        tmp_C[i][j] = tmp_C[i][j] + A[i][k] * B[k][j];
      }
    }
  }
  for (int i=0; i<4; ++i) {
    for (int j=0; j<16; ++j) {
      C[i][j] = tmp_C[i][j];
    }
  }
}
