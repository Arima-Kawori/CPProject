#include "../run.h"

static float tmp_B[16][32][8];
static float tmp_C[32][32];
static float tmp_D[8][32];
static float tmp_A[16][32];
void kernel_case9(float (&B)[16][32][8], float (&C)[32][32], float (&D)[8][32], float (&A)[16][32]) {
  for (int i=0; i<16; ++i) {
    for (int j=0; j<32; ++j) {
      tmp_A[i][j] = 0;
    }
  }
  for (int i=0; i<16; ++i) {
    for (int j=0; j<32; ++j) {
      tmp_A[i][j] = tmp_A[i][j] + A[i][j];
    }
  }
  for (int i=0; i<16; ++i) {
    for (int j=0; j<32; ++j) {
      for (int k=0; k<32; ++k) {
        for (int l=0; l<8; ++l) {
          tmp_A[i][j] = tmp_A[i][j] + B[i][k][l] * C[k][j] * D[l][j];
        }
      }
    }
  }
  for (int i=0; i<16; ++i) {
    for (int j=0; j<32; ++j) {
      A[i][j] = tmp_A[i][j];
    }
  }
}
