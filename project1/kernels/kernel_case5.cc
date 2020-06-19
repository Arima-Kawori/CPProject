#include "../run.h"

static float tmp_B[16][32];
static float tmp_C[32][32];
static float tmp_D[16][32];
static float tmp_alpha;
static float tmp_beta;
static float tmp_A[16][32];
void kernel_case5(float (&B)[16][32], float (&C)[32][32], float (&D)[16][32], float &alpha, float &beta, float (&A)[16][32]) {
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
        tmp_A[i][j] = tmp_A[i][j] + alpha * (B[i][k] * C[k][j]);
      }
    }
  }
  for (int i=0; i<16; ++i) {
    for (int j=0; j<32; ++j) {
      A[i][j] = tmp_A[i][j];
    }
  }
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
      tmp_A[i][j] = tmp_A[i][j] + beta * D[i][j];
    }
  }
  for (int i=0; i<16; ++i) {
    for (int j=0; j<32; ++j) {
      A[i][j] = tmp_A[i][j];
    }
  }
}
