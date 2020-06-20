#include "../run2.h"

static float tmp_dB[32];
static float tmp_dA[2][16];
void grad_case8(float (&dB)[32], float (&dA)[2][16]) {
  for (int idiv16=0; idiv16<2; ++idiv16) {
    for (int imod16=0; imod16<16; ++imod16) {
      dA[idiv16][imod16] = dA[idiv16][imod16] + dB[idiv16 * 16 + imod16] * (1);
    }
  }
}
