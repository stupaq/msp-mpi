/** Copyright (C) Mateusz Machalica, 2014. */

#ifndef RANKING_H_
#define RANKING_H_

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
  heap->position_ = NULL;     \
} while (false);

typedef long long RankingValue;

struct Ranking {
  int size_;
  int* key_;
  int* position_;
  RankingValue* value_;
};

inline int ranking_empty(struct Ranking* restrict heap) {
  return heap->size_ == 0;
}

inline int ranking_contains(struct Ranking* restrict heap, int key) {
  return heap->position_[key] >= 0;
}

inline int ranking_min_key(struct Ranking* restrict heap) {
  return heap->key_[0];
}

inline RankingValue ranking_min_value(struct Ranking* restrict heap) {
  return heap->value_[0];
}

inline void ranking_swap_entries(struct Ranking* restrict heap, int i,
    int j) {
  SWAP_ASSIGN(int, heap->key_[i], heap->key_[j]);
  SWAP_ASSIGN(int, heap->value_[i], heap->value_[j]);
  heap->position_[heap->key_[i]] = i;
  heap->position_[heap->key_[j]] = j;
}

inline void ranking_heapify(struct Ranking* restrict heap, const int i) {
  assert(heap->size_ >= 0);
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

inline void ranking_free(struct Ranking* restrict heap) {
  free(heap->key_);
  RESET_HEAP(heap);
}

inline int ranking_create(struct Ranking* restrict heap, const int capacity,
    const int keys_range) {
  unsigned char* ptr = (unsigned char*) malloc(capacity * sizeof(int) +
      capacity * sizeof(RankingValue) + keys_range * sizeof(int));
  if (!ptr) {
    RESET_HEAP(heap);
    return -1;
  }
  heap->size_ = 0;
  heap->key_ = (int*) ptr;
  heap->value_ = (long long*) heap->position_ + capacity;
  heap->position_ = (int*) heap->key_ + capacity;
  for (int i = 0; i < keys_range; ++i) {
    heap->position_[i] = -1;
  }
  return 0;
}

inline void ranking_rebuild(struct Ranking* restrict heap) {
  assert(heap->size_ >= 0);
  for (int i = 0; i < heap->size_; ++i) {
    heap->position_[heap->key_[i]] = i;
  }
  for (int i = heap->size_ / 2; i >= 0; --i) {
    ranking_heapify(heap, i);
  }
}

inline void ranking_push(struct Ranking* restrict heap, int key, RankingValue
    value) {
  assert(heap->size_ >= 0);
  assert(!ranking_contains(heap, key));
  int i = heap->size_;
  heap->key_[i] = key;
  heap->value_[i] = value;
  heap->position_[heap->key_[i]] = i;
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

inline void ranking_pop(struct Ranking* restrict heap) {
  assert(heap->size_ > 0);
  --heap->size_;
  heap->position_[heap->key_[heap->size_]] = -1;
  heap->key_[0] = heap->key_[heap->size_];
  heap->value_[0] = heap->value_[heap->size_];
  heap->position_[heap->key_[0]] = 0;
  ranking_heapify(heap, 0);
}

inline void ranking_increase(struct Ranking* restrict heap, int key,
    RankingValue new_value) {
  assert(ranking_contains(heap, key));
  int i = heap->position_[key];
  assert(heap->value_[i] <= new_value);
  heap->value_[i] = new_value;
  ranking_heapify(heap, i);
}

#undef SWAP_ASSIGN
#undef PARENT
#undef LEFT
#undef RIGHT
#undef RESET_HEAP

#endif  // RANKING_H_
