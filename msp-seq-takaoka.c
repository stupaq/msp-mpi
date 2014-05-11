/** Copyright (C) Mateusz Machalica, 2014. */

#if OPTIMIZE >= 1
#define NDEBUG
#endif
#if OPTIMIZE <= 1
#define MICROPROF_ENABLE
#endif

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "./matgen.h"
#include "./microprof.h"
#include "./ranking.h"

#define SWAP_ASSIGN(_type_, _x_, _y_) do {  \
  _type_ swap_temp = _x_;                   \
  _x_ = _y_;                                \
  _y_ = swap_temp;                          \
} while(0)

struct PartialSum {
  long long sum;
  int i, j, k, l;
};

static void print_usage(char const* prog) {
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "    %s <num_rows> <num_colums> <seed>\n\n", prog);
}

/* PART OF THE ALGORITHM */
static long long* matrix_ptr = NULL;
static int matrix_height, matrix_width;
/* The matrix is laid out in row-major format and indexed starting from 1. */
#define MATRIX_ARR(_i_, _j_) matrix_ptr[(_i_) * matrix_width + (_j_)]

static struct PartialSum msp_solve(int I, int J, int K, int L) {
  assert(I >= 0 && K >= I);
  assert(J >= 0 && L >= J);
  struct PartialSum best = { MATRIX_ARR(I, J), I, J, I, J }, other;
#define UPDATE_BEST(_current_, _i_, _j_, _k_, _l_) \
  if (best.sum < _current_) {                         \
    best.sum = _current_;                             \
    best.i = _i_; best.j = _j_;                       \
    best.k = _k_; best.l = _l_;                       \
  }
#define UPDATE_BEST1(_other_)            \
  if (best.sum < _other_.sum) {             \
    best.sum = _other_.sum;                 \
    best.i = _other_.i; best.j = _other_.j; \
    best.k = _other_.k; best.l = _other_.l; \
  }
  if (K == I && L == J) {
    return best;
  }

  // FIXME(stupaq) this is not Takaoka's algorithm...
  if (K - I > L - J) {
    int mid = (I + K) / 2;
    // FIXME(stupaq) here!
#define COLUMN_SUM(_j_) (MATRIX_ARR(k, _j_) - MATRIX_ARR(i - 1, _j_) \
    - MATRIX_ARR(k, _j_ - 1) + MATRIX_ARR(i - 1, _j_ - 1))
    for (int i = I; i <= mid; ++i) {
      for (int k = mid + 1; k <= K; ++k) {
        long long current = -1, nextDiff = COLUMN_SUM(1);
        for (int j = J, l = J; l <= L; ++l) {
          if (current < 0) {
            current = 0;
            j = l;
          }
          current += nextDiff;
          if (l == L || (nextDiff = COLUMN_SUM(l + 1)) < 0) {
            UPDATE_BEST(current, i, j, k, l);
          }
        }
      }
    }
#undef COLUMN_SUM
    other = msp_solve(I, J, mid, L);
    UPDATE_BEST1(other);
    other = msp_solve(mid + 1, J, K, L);
    UPDATE_BEST1(other);
  } else {
    int mid = (J + L) / 2;
    // FIXME(stupaq) here!
#define ROW_SUM(_i_) (MATRIX_ARR(_i_, l) - MATRIX_ARR(_i_, j - 1) \
    - MATRIX_ARR(_i_ - 1, l) + MATRIX_ARR(_i_ - 1, j - 1))
    for (int j = J; j <= mid; ++j) {
      for (int l = mid + 1; l <= L; ++l) {
        long long current = -1, nextDiff = ROW_SUM(1);
        for (int i = I, k = I; k <= K; ++k) {
          if (current < 0) {
            current = 0;
            i = k;
          }
          current += nextDiff;
          if (k == K || (nextDiff = ROW_SUM(k + 1)) < 0) {
            UPDATE_BEST(current, i, j, k, l);
          }
        }
      }
    }
#undef ROW_SUM
    other = msp_solve(I, J, K, mid);
    UPDATE_BEST1(other);
    other = msp_solve(I, mid + 1, K, L);
    UPDATE_BEST1(other);
  }

#undef UPDATE_BEST1
#undef UPDATE_BEST
  return best;
}

int main(int argc, char * argv[]) {
  int err = 0;
  matgen_t* generator = NULL;

  /* PARSING ARGUMENTS */
  if (argc != 4) {
    fprintf(stderr, "ERROR: Invalid arguments!\n");
    err = 1;
    goto exit;
  }
  int num_rows = atoi(argv[1]),
      num_columns = atoi(argv[2]),
      seed = atoi(argv[3]);
  if (num_rows <= 0 || num_columns <= 0 || seed <= 0) {
    fprintf(stderr, "ERROR: Invalid arguments: %s %s %s!\n", argv[1],
        argv[2], argv[3]);
    err = 1;
    goto exit;
  }

  /* GENERATING INPUT */
  generator = matgenNew(num_rows, num_columns, seed);
  if (generator == NULL) {
    fprintf(stderr, "ERROR: Unable to create the matrix generator!\n");
    err = 2;
    goto exit;
  }

  matrix_height = num_rows + 1;
  matrix_width = num_columns + 1;
  assert(matrix_height >= num_rows);
  assert(matrix_width >= num_columns);
  matrix_ptr = (long long*) malloc(matrix_height * matrix_width * sizeof(long
        long));
  if (matrix_ptr == NULL) {
    fprintf(stderr, "ERROR: Unable to create the matrix!\n");
    err = 2;
    goto exit;
  }
  for (int i = 1; i <= num_rows; ++i) {
    for (int j = 1; j <= num_columns; ++j) {
      MATRIX_ARR(i, j) = matgenGenerate(generator);
    }
  }
  matgenDestroy(generator);
  generator = NULL;

  struct timeval start_time;
  if (gettimeofday(&start_time, NULL)) {
    fprintf(stderr, "ERROR: Gettimeofday failed!\n");
    err = 2;
    goto exit;
  }
  /* ACTUAL COMPUTATION */

  /* We need prefix sums to reduce the problem to distance matrix
   * multiplication. */
  for (int j = 1; j <= num_columns; ++j) {
    MATRIX_ARR(0, j) = 0;
  }
  for (int i = 1; i <= num_rows; ++i) {
    MATRIX_ARR(i, 0) = 0;
    for (int j = 1; j <= num_columns; ++j) {
      MATRIX_ARR(i, j) += MATRIX_ARR(i - 1, j) + MATRIX_ARR(i, j - 1) -
        MATRIX_ARR(i - 1, j - 1);
    }
  }

  /* Let's roll... */
  struct PartialSum best = msp_solve(1, 1, num_rows, num_columns);

#ifndef NDEBUG
  {
    assert(0 < best.i && best.i <= best.k);
    assert(0 < best.j && best.j <= best.l);
    long long sum = 0;
    for (int i = best.i; i <= best.k; ++i) {
      for (int j = best.j; j <= best.l; ++j) {
        sum += MATRIX_ARR(i, j) - MATRIX_ARR(i - 1, j) - MATRIX_ARR(i, j - 1) +
          MATRIX_ARR(i - 1, j - 1);
      }
    }
    assert(sum == best.sum);
  }
#endif

  /* DONE */
  struct timeval end_time;
  if (gettimeofday(&end_time, NULL)) {
    fprintf(stderr, "ERROR: Gettimeofday failed!\n");
    err = 2;
    goto exit;
  }

  double duration =
    ((double) end_time.tv_sec + ((double) end_time.tv_usec / 1000000.0)) -
    ((double) start_time.tv_sec + ((double) start_time.tv_usec / 1000000.0));

  fprintf(stderr,
      "PWIR2014_Mateusz_Machalica_305678 "
      "Input: (%d,%d,%d) Solution: |(%d,%d),(%d,%d)|=%lld Time: %.10f\n",
      num_rows, num_columns, seed,
      best.i, best.j, best.k, best.l,
      best.sum, duration);

exit:
  /* CLEANUP */
  if (generator) {
    matgenDestroy(generator);
  }
#undef MATRIX_ARR
  free(matrix_ptr);
  if (err == 1) {
    print_usage(argv[0]);
  }

  return err;
}
