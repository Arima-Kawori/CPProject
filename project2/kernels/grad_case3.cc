#include "../run2.h"

static float tmp_B[16][16];
static float tmp_dC[4][16];
static float tmp_dA[4][16];
void grad_case3(float (&B)[16][16], float (&dC)[4][16], float (&dA)[4][16]) {
  for (int i=0; i<4; ++i) {
    for (int k=0; k<16; ++k) {
      for (int j=0; j<16; ++j) {
        dA[i][k] = dA[i][k] + dC[i][j] * (B[k][j]);
      }
    }
  }
}
