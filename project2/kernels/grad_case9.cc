#include "../run2.h"

static float tmp_dB[4][6];
static float tmp_dA[4];
void grad_case9(float (&dB)[4][6], float (&dA)[4]) {
  for (int i=0; i<4; ++i) {
    tmp_dA[i] = 0;
  }
  for (int i=0; i<4; ++i) {
    for (int j=0; j<6; ++j) {
      tmp_dA[i] = tmp_dA[i] + dB[i][j];
    }
  }
  for (int i=0; i<4; ++i) {
    dA[i] = tmp_dA[i];
  }
}
