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
#include "./minsum.h"

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
/* Temporary data (big enough for everything). */
static void* temp_ptr = NULL;
static int matrix_height, matrix_width;
/* The matrix is laid out in row-major format and indexed starting from 1. */
#define MATRIX_ARR(_i_, _j_) matrix_ptr[(_i_) * matrix_width + (_j_)]

#define UPDATE_BEST(_sum_, _i_, _j_, _k_, _l_)    \
  if (best.sum < _sum_) {                         \
    best.sum = _sum_;                             \
    best.i = _i_; best.j = _j_;                   \
    best.k = _k_; best.l = _l_;                   \
  }
#define UPDATE_BEST1(_other_)                \
  if (best.sum < _other_.sum) {             \
    best.sum = _other_.sum;                 \
    best.i = _other_.i; best.j = _other_.j; \
    best.k = _other_.k; best.l = _other_.l; \
  }
static struct PartialSum msp_horizontal(int I, int J, int K, int L, int mid) {
  struct PartialSum best = { MATRIX_ARR(I, J), I, J, I, J };
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
  return best;
}

static int msp_vertical_I, msp_vertical_J;

static inline long long msp_vertical_A1(int i, int l) {
  return MATRIX_ARR(msp_vertical_I + i, msp_vertical_J + l);
}

static inline long long msp_vertical_B1(int l, int k) {
  return - MATRIX_ARR(msp_vertical_I + k, msp_vertical_J + l);
}

static inline long long msp_vertical_A2(int k, int j) {
  return MATRIX_ARR(msp_vertical_I + k, msp_vertical_J + j);
}

static inline long long msp_vertical_B2(int j, int i) {
  return - MATRIX_ARR(msp_vertical_I + i, msp_vertical_J + j);
}

static struct PartialSum msp_vertical(int I, int J, int K, int L, int mid_abs) {
  struct PartialSum best = { MATRIX_ARR(I, J), I, J, I, J };
  const int M = K - I + 1, N = L - J + 1, mid = mid_abs - J;
  int* list1 = temp_ptr;
  int* list2 = list1 + M * N;
  /* Prepare lists. */
  msp_vertical_I = I;
  msp_vertical_J = J;
  minsum_prepare(msp_vertical_A1, msp_vertical_B1, M, mid, M, list1);
  msp_vertical_J +=  2;
  minsum_prepare(msp_vertical_A2, msp_vertical_B2, M, N - mid, M, list2);
  /* For each source... */
  for (int i = 0; i < M; ++i) {
    /* Optimize first component. */
    long long* res_sum1 = (long long*) (list2 + M * N);
    int* res_k1 = (int*) (res_sum1 + M);
    msp_vertical_J = J;
    minsum_find_one(msp_vertical_A1, msp_vertical_B1, i, mid, M, list1,
        res_sum1, res_k1);
    /* Optimize second component. */
    long long* res_sum2 = (long long*) (res_k1 + M);
    int* res_k2 = (int*) (res_sum2 + M);
    msp_vertical_J += mid;
    minsum_find_one(msp_vertical_A2, msp_vertical_B2, i, N - mid, M,
        list2, res_sum2, res_k2);
    /* Find best sum of components. */
    for (int k = 0; k < M; ++k) {
      UPDATE_BEST(- res_sum1[k] - res_sum2[k], I + i, J + res_k1[k], I + k,
          J + mid + res_k2[k]);
    }
  }
  return best;
}

static struct PartialSum msp_solve(int I, int J, int K, int L) {
  assert(I >= 0 && K >= I);
  assert(J >= 0 && L >= J);
  struct PartialSum best = { MATRIX_ARR(I, J), I, J, I, J }, other;
  if (K == I && L == J) {
    return best;
  }
  if (K - I > L - J) {
    int mid = (I + K) / 2;
    other = msp_horizontal(I, J, K, L, mid);
    UPDATE_BEST1(other);
    other = msp_solve(I, J, mid, L);
    UPDATE_BEST1(other);
    other = msp_solve(mid + 1, J, K, L);
    UPDATE_BEST1(other);
  } else {
    int mid = (J + L) / 2;
    other = msp_vertical(I, J, K, L, mid);
    UPDATE_BEST1(other);
    other = msp_solve(I, J, K, mid);
    UPDATE_BEST1(other);
    other = msp_solve(I, mid + 1, K, L);
    UPDATE_BEST1(other);
  }
  return best;
}
#undef UPDATE_BEST1
#undef UPDATE_BEST

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

  /* We asssert that num_rows <= num_columns for the rest of the algorithm and
   * appropriately transpose input matrix (durign generation process). */
  const bool transpose = num_rows > num_columns;
  if (transpose) {
    SWAP_ASSIGN(int, num_rows, num_columns);
  }
  assert(num_rows <= num_columns);

  matrix_height = num_rows + 1;
  matrix_width = num_columns + 1;
  assert(matrix_height >= num_rows);
  assert(matrix_width >= num_columns);
  matrix_ptr = (long long*) malloc(matrix_height * matrix_width * sizeof(long
        long));
  temp_ptr = malloc(2 * matrix_height * matrix_width * sizeof(int) +
      2 * (matrix_height + matrix_width) * (sizeof(long long) + sizeof(int)));
  if (matrix_ptr == NULL) {
    fprintf(stderr, "ERROR: Unable to create the matrix!\n");
    err = 2;
    goto exit;
  }
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
  free(temp_ptr);
  free(matrix_ptr);
  if (err == 1) {
    print_usage(argv[0]);
  }

  return err;
}
