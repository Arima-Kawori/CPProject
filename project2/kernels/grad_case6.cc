#include "../run2.h"

static float tmp_C[8][16][3][3];
static float tmp_dA[2][8][5][5];
static float tmp_dB[2][16][7][7];
void grad_case6(float (&C)[8][16][3][3], float (&dA)[2][8][5][5], float (&dB)[2][16][7][7]) {
  for (int n=0; n<2; ++n) {
    for (int c=0; c<16; ++c) {
      for (int h=0; h<3; ++h) {
        for (int w=0; w<3; ++w) {
          tmp_dB[n][c][h][w] = 0;
        }
      }
    }
  }
  for (int n=0; n<2; ++n) {
    for (int c=0; c<16; ++c) {
      for (int h=0; h<3; ++h) {
        for (int w=0; w<3; ++w) {
          for (int p=0; p<3; ++p) {
            for (int q=0; q<3; ++q) {
              for (int k=0; k<8; ++k) {
                tmp_dB[n][c][h][w] = tmp_dB[n][c][h][w] + ((h - p >= 0) && (w - q >= 0) && (h - p < 3) && (w - q < 3)) ? (dA[n][k][p][q] * C[k][c][h - p][w - q]) : (0);
              }
            }
          }
        }
      }
    }
  }
  for (int n=0; n<2; ++n) {
    for (int c=0; c<16; ++c) {
      for (int h=0; h<3; ++h) {
        for (int w=0; w<3; ++w) {
          dB[n][c][h][w] = tmp_dB[n][c][h][w];
        }
      }
    }
  }
}
