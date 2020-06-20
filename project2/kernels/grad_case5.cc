#include "../run2.h"

static float tmp_C[32][32];
static float tmp_D[4][32];
static float tmp_dA[16][32];
static float tmp_dB[16][32][4];
void grad_case5(float (&C)[32][32], float (&D)[4][32], float (&dA)[16][32], float (&dB)[16][32][4]) {
  for (int i=0; i<16; ++i) {
    for (int k=0; k<32; ++k) {
      for (int l=0; l<4; ++l) {
        for (int j=0; j<32; ++j) {
          dB[i][k][l] = dB[i][k][l] + dA[i][j] * (C[k][j] * D[l][j]);
        }
      }
    }
  }
}
