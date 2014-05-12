/** Copyright (C) Mateusz Machalica, 2014. */

#ifndef MINSUM_H_
#define MINSUM_H_

#include "./ranking.h"

#define LIST_ARR(_i_, _j_) list_ptr[(_i_) * j_count + (_j_)]

static int minsum_k;
static long long (*minsum_A) (int, int);
static long long (*minsum_B) (int, int);

static inline int minsum_prepare_list_cmp(const int* j1, const int* j2) {
  long long x1 = minsum_A(minsum_k, *j1), x2 = minsum_B(minsum_k, *j2);
  return x1 == x2 ? 0 : (x1 < x2 ? -1 : 1);
}

static inline void minsum_prepare(
    long long (*A) (int, int),              /* i_count x k_count */
    long long (*B) (int, int),              /* k_count x j_count */
    const int i_count, const int k_count, const int j_count,
    int* restrict list_ptr                  /* k_count x j_count */
    ) {
  SUPPRESS_UNUSED(i_count);
  minsum_A = A;
  minsum_B = B;
  for (int k = 0; k < k_count; ++k) {
    minsum_k = k;
    for (int j = 0; j < j_count; ++j) {
      LIST_ARR(k, j) = j;
    }
    qsort(&LIST_ARR(k, 0), j_count, sizeof(int),
        (int (*)(const void*, const void*)) minsum_prepare_list_cmp);
  }
}

static inline void minsum_find_one(
    long long (*A) (int, int),              /* i_count x k_count */
    long long (*B) (int, int),              /* k_count x j_count */
    const int i_ind, const int k_count, const int j_count,
    int* restrict list_ptr,                 /* k_count x j_count */
    long long* restrict result_sum,         /* j_count */
    int* restrict result_k                  /* j_count */
    ) {
  // TODO
}

#undef LIST_ARR

#endif  // MINSUM_H_
