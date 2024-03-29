/** Copyright (C) Mateusz Machalica, 2014. */

#ifndef RANKING_H_
#define RANKING_H_

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#define SWAP_ASSIGN(_type_, _x_, _y_) do {  \
  _type_ swap_temp = _x_;                   \
  _x_ = _y_;                                \
  _y_ = swap_temp;                          \
} while(0)
#define PARENT(i) ((i - 1) / 2)
#define LEFT(i) (2 * i + 1)
#define RIGHT(i) (2 * i + 2)
#define RESET_HEAP(heap) do { \
  heap->size_ = -1;           \
  heap->key_ = NULL;          \
  heap->value_ = NULL;        \
} while (false);

typedef long long RankingValue;

struct Ranking {
  int size_;
  int* key_;
  RankingValue* value_;
};

static inline bool ranking_empty(struct Ranking* restrict heap) {
  assert(heap->size_ >= 0);
  return heap->size_ == 0;
}

static inline int ranking_min_key(struct Ranking* restrict heap) {
  assert(heap->size_ >= 0);
  return heap->key_[0];
}

static inline RankingValue ranking_min_value(struct Ranking* restrict heap) {
  assert(heap->size_ >= 0);
  return heap->value_[0];
}

static inline void ranking_swap_entries(struct Ranking* restrict heap, int i,
    int j) {
  assert(heap->size_ > i && i >= 0);
  assert(heap->size_ > j && j >= 0);
  SWAP_ASSIGN(int, heap->key_[i], heap->key_[j]);
  SWAP_ASSIGN(int, heap->value_[i], heap->value_[j]);
}

static inline void ranking_heapify(struct Ranking* restrict heap, const int i) {
  assert(heap->size_ > i && i >= 0);
  const int l = LEFT(i), r = RIGHT(i);
  int j = i;
  if (l < heap->size_ && heap->value_[l] < heap->value_[j]) {
    j = l;
  }
  if (r < heap->size_ && heap->value_[r] < heap->value_[j]) {
    j = r;
  }
  if (j != i) {
    ranking_swap_entries(heap, i, j);
    ranking_heapify(heap, j);
  }
}

static inline void ranking_free(struct Ranking* restrict heap) {
  free(heap->key_);
  RESET_HEAP(heap);
}

static inline int ranking_create(struct Ranking* restrict heap, const int
    capacity) {
  unsigned char* ptr = (unsigned char*) malloc(capacity * sizeof(int) +
      capacity * sizeof(RankingValue));
  if (!ptr) {
    RESET_HEAP(heap);
    return -1;
  }
  heap->size_ = 0;
  heap->key_ = (int*) ptr;
  heap->value_ = (long long*) (heap->key_ + capacity);
  return 0;
}

static inline void ranking_rebuild(struct Ranking* restrict heap) {
  assert(heap->size_ >= 0);
  for (int i = heap->size_ / 2; i >= 0; --i) {
    ranking_heapify(heap, i);
  }
}

static inline void ranking_push(struct Ranking* restrict heap, int key,
    RankingValue value) {
  assert(heap->size_ >= 0);
  int i = heap->size_;
  heap->key_[i] = key;
  heap->value_[i] = value;
  ++heap->size_;
  while (i > 0) {
    int p = PARENT(i);
    if (heap->value_[i] >= heap->value_[p]) {
      break;
    }
    ranking_swap_entries(heap, i, p);
    i = p;
  }
}

static inline void ranking_pop(struct Ranking* restrict heap) {
  assert(heap->size_ > 0);
  --heap->size_;
  if (heap->size_ > 0) {
    heap->key_[0] = heap->key_[heap->size_];
    heap->value_[0] = heap->value_[heap->size_];
    ranking_heapify(heap, 0);
  }
}

static inline void ranking_increase_min(struct Ranking* restrict heap,
    RankingValue new_value) {
  assert(heap->size_ > 0);
  assert(new_value >= ranking_min_value(heap));
  heap->value_[0] = new_value;
  ranking_heapify(heap, 0);
}

static inline void ranking_fprintf(FILE* filep, const struct Ranking* restrict
    heap) {
  assert(heap->size_ > 0);
  fprintf(filep, "Ranking: %p\n\tkeys:\t", (void*) heap);
  int max_key = -1;
  for (int i = 0; i < heap->size_; ++i) {
    fprintf(filep, "\t%d", heap->key_[i]);
    if (max_key < heap->key_[i]) {
      max_key = heap->key_[i];
    }
  }
  fprintf(filep, "\n\tvalues:\t");
  for (int i = 0; i < heap->size_; ++i) {
    fprintf(filep, "\t%lld", heap->value_[i]);
  }
  fprintf(filep, "\n");
}

#undef SWAP_ASSIGN
#undef PARENT
#undef LEFT
#undef RIGHT
#undef RESET_HEAP

#endif  // RANKING_H_
