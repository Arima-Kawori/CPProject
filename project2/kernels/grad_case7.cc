#include "../run2.h"

static float tmp_A[32][16];
static float tmp_B[16][32];
void grad_case7(float (&A)[32][16], float (&B)[16][32]) {
  for (int i=0; i<16; ++i) {
    for (int j=0; j<32; ++j) {
      tmp_B[i][j] = 0;
    }
  }
  for (int i=0; i<16; ++i) {
    for (int j=0; j<32; ++j) {
      tmp_B[i][j] = tmp_B[i][j] + A[j][i];
    }
  }
  for (int i=0; i<16; ++i) {
    for (int j=0; j<32; ++j) {
      B[i][j] = tmp_B[i][j];
    }
  }
}
