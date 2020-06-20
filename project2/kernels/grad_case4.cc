#include "../run2.h"

static float tmp_B[16][32];
static float tmp_C[32][32];
static float tmp_dA[16][32];
static float tmp_dB[16][32];
static float tmp_dC[32][32];
void grad_case4(float (&B)[16][32], float (&C)[32][32], float (&dA)[16][32], float (&dB)[16][32], float (&dC)[32][32]) {
  for (int i=0; i<16; ++i) {
    for (int k=0; k<32; ++k) {
      tmp_dB[i][k] = 0;
    }
  }
  for (int i=0; i<16; ++i) {
    for (int k=0; k<32; ++k) {
      for (int j=0; j<32; ++j) {
        tmp_dB[i][k] = tmp_dB[i][k] + dA[i][j] * C[k][j];
      }
    }
  }
  for (int i=0; i<16; ++i) {
    for (int k=0; k<32; ++k) {
      dB[i][k] = tmp_dB[i][k];
    }
  }
  for (int k=0; k<32; ++k) {
    for (int j=0; j<32; ++j) {
      tmp_dC[k][j] = 0;
    }
  }
  for (int k=0; k<32; ++k) {
    for (int j=0; j<32; ++j) {
      for (int i=0; i<16; ++i) {
        tmp_dC[k][j] = tmp_dC[k][j] + dA[i][j] * B[i][k];
      }
    }
  }
  for (int k=0; k<32; ++k) {
    for (int j=0; j<32; ++j) {
      dC[k][j] = tmp_dC[k][j];
    }
  }
}
