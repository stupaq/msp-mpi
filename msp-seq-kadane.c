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

int main(int argc, char * argv[]) {
  int err = 0;
  matgen_t* generator = NULL;
  long long* matrix_ptr = NULL;

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

  /* We asssert that num_rows <= num_columns for the rest of the algorithm and
   * appropriately transpose input matrix (durign generation process). */
  const bool transpose = num_rows > num_columns;
  if (transpose) {
    SWAP_ASSIGN(int, num_rows, num_columns);
  }
  assert(num_rows <= num_columns);

  const int matrix_height = num_rows + 1,
        matrix_width = num_columns;
  assert(matrix_height >= num_rows);
  assert(matrix_width >= num_columns);
  matrix_ptr = (long long*) malloc(matrix_height * matrix_width * sizeof(long
        long));
  if (matrix_ptr == NULL) {
    fprintf(stderr, "ERROR: Unable to create the matrix!\n");
    err = 2;
    goto exit;
  }
  /* The matrix is laid out in row-major format and indexed starting from 1. */
#define MATRIX_ARR(_i_, _j_) matrix_ptr[(_i_) * matrix_width + (_j_) - 1]
  if (transpose) {
    for (int j = 1; j <= num_columns; ++j) {
      for (int i = 1; i <= num_rows; ++i) {
        MATRIX_ARR(i, j) = matgenGenerate(generator);
      }
    }
  } else {
    for (int i = 1; i <= num_rows; ++i) {
      for (int j = 1; j <= num_columns; ++j) {
        MATRIX_ARR(i, j) = matgenGenerate(generator);
      }
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

  /* We will scan subsequence of rows using Kadane's algorithm, we need to
   * detrmine sum of values in corresponding subcolumn. */
  for (int j = 1; j <= num_columns; ++j) {
    MATRIX_ARR(0, j) = 0;
  }
  for (int i = 1; i <= num_rows; ++i) {
    for (int j = 1; j <= num_columns; ++j) {
      MATRIX_ARR(i, j) += MATRIX_ARR(i - 1, j);
    }
  }

  struct PartialSum best = { MATRIX_ARR(1, 1), 1, 1, 1, 1 };
#define UPDATE_BEST(_current_, _i_, _j_, _k_, _l_) \
  if (best.sum < _current_) {                         \
    best.sum = _current_;                             \
    best.i = _i_; best.j = _j_;                       \
    best.k = _k_; best.l = _l_;                       \
  }

  for (int i = 1; i <= num_rows; ++i) {
    for (int k = i; k <= num_rows; ++k) {
      assert(i > 0 && k >= i);
#define COLUMN_SUM(_j_) (MATRIX_ARR(k, _j_) - MATRIX_ARR(i - 1, _j_))
      long long current = -1, nextDiff = COLUMN_SUM(1);
      for (int j = 1, l = 1; l <= num_columns; ++l) {
        assert(j > 0 && l >= j);
        if (current < 0) {
          current = 0;
          j = l;
        }
        current += nextDiff;
        if (l == num_columns || (nextDiff = COLUMN_SUM(l + 1)) < 0) {
          UPDATE_BEST(current, i, j, k, l);
        }
      }
#undef COLUMN_SUM
    }
  }
#undef UPDATE_BEST

#ifndef NDEBUG
  {
    assert(0 < best.i && best.i <= best.k);
    assert(0 < best.j && best.j <= best.l);
    long long sum = 0;
    for (int i = best.i; i <= best.k; ++i) {
      for (int j = best.j; j <= best.l; ++j) {
        sum += MATRIX_ARR(i, j) - MATRIX_ARR(i - 1, j);
      }
    }
    assert(sum == best.sum);
  }
#endif

  if (transpose) {
    SWAP_ASSIGN(int, num_rows, num_columns);
    SWAP_ASSIGN(int, best.i, best.j);
    SWAP_ASSIGN(int, best.k, best.l);
  }

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
