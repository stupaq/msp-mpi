/** Copyright (C) Mateusz Machalica, 2014. */

#ifndef MINSUM_H_
#define MINSUM_H_

#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include "./ranking.h"

#define LIST_ARR(_i_, _j_) list_ptr[(_i_) * j_count + (_j_)]

static int minsum_k;
static long long (*minsum_A) (int, int);
static long long (*minsum_B) (int, int);

static inline int minsum_prepare_list_cmp(const int* j1, const int* j2) {
  long long x1 = minsum_B(minsum_k, *j1), x2 = minsum_B(minsum_k, *j2);
  return x1 == x2 ? 0 : (x1 < x2 ? -1 : 1);
}

static inline void minsum_prepare(
    long long (*A) (int, int),              /* i_count x k_count */
    long long (*B) (int, int),              /* k_count x j_count */
    const int i_count, const int k_count, const int j_count,
    int* restrict list_ptr                  /* k_count x j_count */
    ) {
  SUPPRESS_UNUSED(i_count);
  assert(i_count > 0);
  assert(k_count > 0);
  assert(j_count > 0);
  minsum_A = A;
  minsum_B = B;
  for (int k = 0; k < k_count; ++k) {
    for (int j = 0; j < j_count; ++j) {
      LIST_ARR(k, j) = j;
    }
    minsum_k = k;
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
    int* restrict result_k,                 /* j_count */
    void* const temp_ptr /* k_count * sizeof(int) + j_count * sizeof(bool) */
    ) {
  assert(k_count > 0);
  assert(j_count > 0);
  int* cand = temp_ptr;                               /* k_count */
  memset(cand, 0, k_count * sizeof(int));
#define CAND_GET(k) LIST_ARR(k, cand[k])
#define CAND_NEXT(k) LIST_ARR(k, ++cand[k])
#define CAND_HAS(k) (cand[k] < j_count)
#define CAND_HAS_NEXT(k) (cand[k] < j_count - 1)
  bool* solved = (bool*) (cand + k_count);            /* j_count */
  memset(solved, 0, j_count * sizeof(bool));
  struct Ranking queue;                               /* k_count */
  ranking_create(&queue, k_count);
  queue.size_ = k_count;
  for (int k = 0; k < k_count; ++k) {
    assert(cand[k] == 0);
    queue.key_[k] = k;
    queue.value_[k] = A(i_ind, k) + B(k, CAND_GET(k));
  }
  ranking_rebuild(&queue);
  int solved_count = 0;
  const double the_count = j_count,
        adjustment = the_count + the_count / log(the_count);
  while (solved_count < j_count) {
    assert(!ranking_empty(&queue));
    const int k = ranking_min_key(&queue);
    assert(CAND_HAS(k));
    int j = CAND_GET(k);
    const long long d_k = ranking_min_value(&queue);
    assert(d_k == A(i_ind, k) + B(k, j));
    if (!solved[j]) {
      ++solved_count;
      solved[j] = true;
      result_sum[j] = d_k;
      result_k[j] = k;
    }
    int limit = (int) (the_count / (adjustment - solved_count)) + 1;
    assert(limit  > 0);
    assert(j == CAND_GET(k));
    while(true) {
      assert(CAND_HAS(k));
      assert(k == ranking_min_key(&queue));
      if (!solved[j] || limit == 0) {
        long long new_dist = A(i_ind, k) + B(k, j);
        ranking_increase_min(&queue, new_dist);
        break;
      } else if (!CAND_HAS_NEXT(k)) {
        ranking_pop(&queue);
        break;
      } else {
        assert(solved[j]);
        assert(limit > 0);
        assert(CAND_HAS_NEXT(k));
        j = CAND_NEXT(k);
        --limit;
      }
    }
  }
#undef CAND_NEXT
#undef CAND_GET
  ranking_free(&queue);
}

#undef LIST_ARR

#endif  // MINSUM_H_
