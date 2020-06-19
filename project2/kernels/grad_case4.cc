#include "../run2.h"

static float tmp_B[16][32];
static float tmp_C[32][32];
static float tmp_A[16][32];
void grad_case4(float (&B)[16][32], float (&C)[32][32], float (&A)[16][32]) {
  for (int i=0; i<16; ++i) {
    for (int j=0; j<32; ++j) {
      tmp_A[i][j] = 0;
    }
  }
  for (int i=0; i<16; ++i) {
    for (int j=0; j<32; ++j) {
      for (int k=0; k<32; ++k) {
        tmp_A[i][j] = tmp_A[i][j] + B[i][k] * C[k][j];
      }
    }
  }
  for (int i=0; i<16; ++i) {
    for (int j=0; j<32; ++j) {
      A[i][j] = tmp_A[i][j];
    }
  }
}
