/** Copyright (C) Mateusz Machalica, 2014. */

#ifndef NDEBUG
#pragma message "Assertions enabled!"
#endif

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "./matgen.h"
#define MICROPROF_ENABLE
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
#define ORIG_ARR(_i_, _j_) (MATRIX_ARR(_i_, _j_) - MATRIX_ARR(_i_ - 1, _j_)   \
    - MATRIX_ARR(_i_, _j_ - 1) + MATRIX_ARR(_i_ - 1, _j_ - 1))

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

static int msp_horizontal_I, msp_horizontal_J;

/* - s(i-1, j-1) */
static inline long long msp_horizontal_A1(int j, int i) {
  return - MATRIX_ARR(msp_horizontal_I + i - 1, msp_horizontal_J + j - 1);
}

/* + s(i-1, l) */
static inline long long msp_horizontal_B1(int i, int l) {
  return MATRIX_ARR(msp_horizontal_I + i - 1, msp_horizontal_J + l);
}

/* + s(k, j-1) */
static inline long long msp_horizontal_A2(int j, int k) {
  return MATRIX_ARR(msp_horizontal_I + k, msp_horizontal_J + j - 1);
}

/* - s(k, l) */
static inline long long msp_horizontal_B2(int k, int l) {
  return - MATRIX_ARR(msp_horizontal_I + k, msp_horizontal_J + l);
}

/*  max_{j=J..L, l=j..L}{ - (
 *    min_{i=I..mid_abs}{- s(i-1, j-1) + s(i-1, l)}
 *    min_{k=mid_abs+1..K}{s(k, j-1) - s(k, l)}
 *  )} */
static struct PartialSum msp_horizontal(int I, int J, int K, int L, int
    mid_abs) {
  struct PartialSum best = { ORIG_ARR(I, J), I, J, I, J };
  const int M = K - I + 1, N = L - J + 1, mid = mid_abs - I + 1;
  /*  max_{j=0..N, l=j..N}{ - (
   *    min_{i=I..mid}{- s(i-1, j-1) + s(i-1, l)}
   *    min_{k=mid+1..M}{s(k, j-1) - s(k, l)}
   *  )} */
  assert(N <= M);
  assert(0 < mid && mid < M);
  /* Zero-cost allocations. */
  int* list1 = temp_ptr;
  int* list2 = list1 + M * N;
  long long* res_sum1 = (long long*) (list2 + M * N);
  int* res_l1 = (int*) (res_sum1 + N);
  long long* res_sum2 = (long long*) (res_l1 + N);
  int* res_l2 = (int*) (res_sum2 + N);
  /* Prepare lists. */
  msp_horizontal_I = I;
  msp_horizontal_J = J;
  minsum_prepare(msp_horizontal_A1, msp_horizontal_B1, N, mid, N, list1);
  msp_horizontal_I += mid;
  minsum_prepare(msp_horizontal_A2, msp_horizontal_B2, N, M - mid, N, list2);
  /* For each source... */
  for (int j = 0; j < N; ++j) {
    /* Optimize first component. */
    msp_horizontal_I = I;
    minsum_find_one(msp_horizontal_A1, msp_horizontal_B1, j, mid, N, list1,
        res_sum1, res_l1);
    /* Optimize second component. */
    msp_horizontal_I += mid;
    minsum_find_one(msp_horizontal_A2, msp_horizontal_B2, j, M - mid, N,
        list2, res_sum2, res_l2);
    /* Find best sum of components. */
    int best_l = j;
    assert(best_l < N);
    long long best_sum = - res_sum1[best_l] - res_sum2[best_l];
    for (int l = j + 1; l < N; ++l) {
      long long sum = - res_sum1[l] - res_sum2[l];
      if (best_sum < sum) {
        best_l = l;
        best_sum = sum;
      }
    }
    UPDATE_BEST(best_sum,
        I + res_l1[best_l], J + j,
        I + mid + res_l2[best_l], J + best_l);
  }
  return best;
}

static int msp_vertical_I, msp_vertical_J;

/* - s(i-1, j-1) */
static inline long long msp_vertical_A1(int i, int j) {
  return - MATRIX_ARR(msp_vertical_I + i - 1, msp_vertical_J + j - 1);
}

/* + s(k, j-1) */
static inline long long msp_vertical_B1(int j, int k) {
  return MATRIX_ARR(msp_vertical_I + k, msp_vertical_J + j - 1);
}

/* + s(i-1, l) */
static inline long long msp_vertical_A2(int i, int l) {
  return MATRIX_ARR(msp_vertical_I + i - 1, msp_vertical_J + l);
}

/* - s(k, l) */
static inline long long msp_vertical_B2(int l, int k) {
  return - MATRIX_ARR(msp_vertical_I + k, msp_vertical_J + l);
}

/*  max_{i=I..K, k=i..K}{ - (
 *    min_{j=J..mid_abs}{- s(i-1, j-1) + s(k, j-1)}
 *    min_{l=mid_abs+1..L}{s(i-1, l) - s(k, l)}
 *  )} */
static struct PartialSum msp_vertical(int I, int J, int K, int L, int mid_abs) {
  struct PartialSum best = { ORIG_ARR(I, J), I, J, I, J };
  const int M = K - I + 1, N = L - J + 1, mid = mid_abs - J + 1;
  /*  max_{i=0..M, k=i..M}{ - (
   *    min_{j=J..mid}{- s(i-1, j-1) + s(k, j-1)}
   *    min_{l=mid+1..N}{s(i-1, l) - s(k, l)}
   *  )} */
  assert(M <= N);
  assert(0 < mid && mid < N);
  /* Zero-cost allocations. */
  int* list1 = temp_ptr;
  int* list2 = list1 + M * N;
  long long* res_sum1 = (long long*) (list2 + M * N);
  int* res_k1 = (int*) (res_sum1 + M);
  long long* res_sum2 = (long long*) (res_k1 + M);
  int* res_k2 = (int*) (res_sum2 + M);
  /* Prepare lists. */
  msp_vertical_I = I;
  msp_vertical_J = J;
  minsum_prepare(msp_vertical_A1, msp_vertical_B1, M, mid, M, list1);
  msp_vertical_J += mid;
  minsum_prepare(msp_vertical_A2, msp_vertical_B2, M, N - mid, M, list2);
  /* For each source... */
  for (int i = 0; i < M; ++i) {
    /* Optimize first component. */
    msp_vertical_J = J;
    minsum_find_one(msp_vertical_A1, msp_vertical_B1, i, mid, M, list1,
        res_sum1, res_k1);
    /* Optimize second component. */
    msp_vertical_J += mid;
    minsum_find_one(msp_vertical_A2, msp_vertical_B2, i, N - mid, M, list2,
        res_sum2, res_k2);
    /* Find best sum of components. */
    int best_k = i;
    assert(best_k < M);
    long long best_sum = - res_sum1[best_k] - res_sum2[best_k];
    for (int k = i + 1; k < M; ++k) {
      long long sum = - res_sum1[k] - res_sum2[k];
      if (best_sum < sum) {
        best_k = k;
        best_sum = sum;
      }
    }
    UPDATE_BEST(best_sum,
        I + i, J + res_k1[best_k],
        I + best_k, J + mid + res_k2[best_k]);
  }
  return best;
}

static struct PartialSum msp_solve(int I, int J, int K, int L) {
  assert(I >= 0 && K >= I);
  assert(J >= 0 && L >= J);
  struct PartialSum best = { ORIG_ARR(I, J), I, J, I, J }, other;
  if (K == I && L == J) {
    return best;
  }
  if (K - I > L - J) {
    int mid = (I + K) / 2;
    best = msp_horizontal(I, J, K, L, mid);
    other = msp_solve(I, J, mid, L);
    UPDATE_BEST1(other);
    other = msp_solve(mid + 1, J, K, L);
    UPDATE_BEST1(other);
  } else {
    int mid = (J + L) / 2;
    best = msp_vertical(I, J, K, L, mid);
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
        sum += ORIG_ARR(i, j);
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
