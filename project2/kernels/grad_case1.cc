#include "../run2.h"

static float tmp_B[4][16];
static float tmp_dC[4][16];
static float tmp_dA[4][16];
void grad_case1(float (&B)[4][16], float (&dC)[4][16], float (&dA)[4][16]) {
  for (int i=0; i<4; ++i) {
    for (int j=0; j<16; ++j) {
      dA[i][j] = dA[i][j] + dC[i][j] * (B[i][j]);
    }
  }
}
