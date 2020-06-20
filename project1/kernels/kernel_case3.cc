#include "../run.h"

static int tmp_B[16][32];
static int tmp_C[16][32];
static int tmp_A[16][32];
void kernel_case3(int (&B)[16][32], int (&C)[16][32], int (&A)[16][32]) {
  for (int i=0; i<16; ++i) {
    for (int j=0; j<32; ++j) {
      tmp_A[i][j] = 0;
    }
  }
  for (int i=0; i<16; ++i) {
    for (int j=0; j<32; ++j) {
      tmp_A[i][j] = tmp_A[i][j] + B[i][j];
    }
  }
  for (int i=0; i<16; ++i) {
    for (int j=0; j<32; ++j) {
      tmp_A[i][j] = tmp_A[i][j] + C[i][j];
    }
  }
  for (int i=0; i<16; ++i) {
    for (int j=0; j<32; ++j) {
      A[i][j] = tmp_A[i][j];
    }
  }
}
