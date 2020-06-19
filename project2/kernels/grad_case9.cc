#include "../run2.h"

static float tmp_A[4];
static float tmp_B[4][6];
void grad_case9(float (&A)[4], float (&B)[4][6]) {
  for (int i=0; i<4; ++i) {
    for (int j=0; j<6; ++j) {
      tmp_B[i][j] = 0;
    }
  }
  for (int i=0; i<4; ++i) {
    for (int j=0; j<6; ++j) {
      tmp_B[i][j] = tmp_B[i][j] + A[i];
    }
  }
  for (int i=0; i<4; ++i) {
    for (int j=0; j<6; ++j) {
      B[i][j] = tmp_B[i][j];
    }
  }
}
