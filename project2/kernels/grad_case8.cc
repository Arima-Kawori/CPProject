#include "../run2.h"

static float tmp_dB[32];
static float tmp_dA[2][16];
void grad_case8(float (&dB)[32], float (&dA)[2][16]) {
  for (int i=0; i<32; ++i) {
    dA[i / 16][i % 16] = dA[i / 16][i % 16] + dB[i] * (1);
  }
}
