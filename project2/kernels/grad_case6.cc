#include "../run2.h"

static float tmp_C[8][16][3][3];
static float tmp_dA[2][8][5][5];
static float tmp_dB[2][16][7][7];
void grad_case6(float (&C)[8][16][3][3], float (&dA)[2][8][5][5], float (&dB)[2][16][7][7]) {
  for (int n=0; n<2; ++n) {
    for (int c=0; c<16; ++c) {
      for (int paddr=0; paddr<7; ++paddr) {
        for (int qadds=0; qadds<7; ++qadds) {
          for (int k=0; k<8; ++k) {
            for (int r=0; r<3; ++r) {
              for (int s=0; s<3; ++s) {
                dB[n][c][paddr][qadds] = dB[n][c][paddr][qadds] + dA[n][k][paddr - r][qadds - s] * (C[k][c][r][s]);
              }
            }
          }
        }
      }
    }
  }
}
