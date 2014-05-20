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
#include <mpi.h>
#include "./matgen.h"
#define MICROPROF_ENABLE
#define MICROPROF_IS_PROFILED (my_rank == kRootRank)
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

static MPI_Datatype partial_sum_t;
static MPI_Op max_partial_sum_op;

void max_partial_sum(struct PartialSum* in, struct PartialSum* inout, int* len,
    MPI_Datatype* type) {
  SUPPRESS_UNUSED(type);
  for (int i = 0; i < *len; ++i, ++in, ++inout) {
    if (in->sum > inout->sum) {
      memcpy(inout, in, sizeof(struct PartialSum));
    }
  }
}

struct SubSolution {
  long long T;
  /* Interleaving data makes assignments faster. */
  long long P;
  int Pj, Pl;
  long long S;
  int Sj, Sl;
  long long M;
  int Mj, Ml;
};

static MPI_Datatype sub_solution_t;
static MPI_Op join_sub_solution_op;

void join_sub_solution(struct SubSolution* in, struct SubSolution* inout, int*
    len, MPI_Datatype* type) {
  SUPPRESS_UNUSED(type);
  for (int i = 0; i < *len; ++i, ++in, ++inout) {
    assert(in->Sl + 1 == inout->Pj);
    /* Maximum subsequence. */
    if (inout->M < in->M) {
      inout->M = in->M;
      inout->Mj = in->Mj;
      inout->Ml = in->Ml;
    }
    if (inout->M < in->S + inout->P) {
      inout->M = in->S + inout->P;
      inout->Mj = in->Sj;
      inout->Ml = inout->Pl;
    }
    assert(inout->Mj <= inout->Ml);
    /* Maximum prefix. */
    if (in->P < in->T + inout->P) {
      inout->P = in->T + inout->P;
      inout->Pj = in->Pj;
      inout->Pl = inout->Pl;
    } else {
      assert(in->P >= in->T + inout->P);
      inout->P = in->P;
      inout->Pj = in->Pj;
      inout->Pl = in->Pl;
    }
    assert(inout->Pj == in->Pj);
    /* Maximum suffix. */
    assert(inout->Sl == inout->Sl);
    if (in->S + inout->T > inout->S) {
      inout->S = in->S + inout->T;
      inout->Sj = in->Sj;
    } else {
      assert(in->S + inout->T <= inout->S);
      assert(inout->S == inout->S);
      assert(inout->Sj == inout->Sj);
    }
    /* Total sum. */
    inout->T += in->T;
  }
}

static inline void init_mpi(int* argc, char** argv[]) {
  MPI_Init(argc, argv);
  int block_lengths[] = { 1, 1, 1, 1, 1 };
  MPI_Aint offsets[] = {
    offsetof(struct PartialSum, sum),
    offsetof(struct PartialSum, i),
    offsetof(struct PartialSum, j),
    offsetof(struct PartialSum, k),
    offsetof(struct PartialSum, l)
  };
  MPI_Datatype types[] = { MPI_LONG_LONG_INT, MPI_INT, MPI_INT, MPI_INT,
    MPI_INT };
  MPI_Type_create_struct(5, block_lengths, offsets, types, &partial_sum_t);
  MPI_Type_commit(&partial_sum_t);
  MPI_Op_create((void(*) (void*, void*, int*, int*)) max_partial_sum, true,
      &max_partial_sum_op);

  int block_lengths1[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  MPI_Aint offsets1[] = {
    offsetof(struct SubSolution, T),
    offsetof(struct SubSolution, P),
    offsetof(struct SubSolution, Pj),
    offsetof(struct SubSolution, Pl),
    offsetof(struct SubSolution, S),
    offsetof(struct SubSolution, Sj),
    offsetof(struct SubSolution, Sl),
    offsetof(struct SubSolution, M),
    offsetof(struct SubSolution, Mj),
    offsetof(struct SubSolution, Ml),
  };
  MPI_Datatype types1[] = { MPI_LONG_LONG_INT, MPI_LONG_LONG_INT, MPI_INT,
    MPI_INT, MPI_LONG_LONG_INT, MPI_INT, MPI_INT, MPI_LONG_LONG_INT, MPI_INT,
    MPI_INT };
  MPI_Type_create_struct(10, block_lengths1, offsets1, types1, &sub_solution_t);
  MPI_Type_commit(&sub_solution_t);
  MPI_Op_create((void(*) (void*, void*, int*, int*)) join_sub_solution, false,
      &join_sub_solution_op);
}

static inline void finalize_mpi() {
  MPI_Op_free(&join_sub_solution_op);
  MPI_Type_free(&sub_solution_t);
  MPI_Op_free(&max_partial_sum_op);
  MPI_Type_free(&partial_sum_t);
  MPI_Finalize();
}

static void print_usage(char const* prog) {
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "    %s <num_rows> <num_colums> <seed>\n\n", prog);
}

int main(int argc, char * argv[]) {
  int tmp, err = 0;
  matgen_t* generator = NULL;
  long long* matrix_ptr = NULL;

  /* SETUP */
  init_mpi(&argc, &argv);

  const int kRootRank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &tmp);
  const int num_processes = tmp;
  MPI_Comm_rank(MPI_COMM_WORLD, &tmp);
  const int my_rank = tmp;

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

  const double start_time = MPI_Wtime();
  /* ACTUAL COMPUTATION */

  /* At this point we also decide which algorithm we should use, generalised
   * Kadane's or the one for degenerate case. */
  MICROPROF_START(column_sums);
  const bool degenerate = num_rows * (num_rows - 1) / 2 < num_processes &&
    num_columns > 3 * num_processes;
  int my_first_column, my_last_column;
  if (degenerate) {
    my_first_column = 1 + (my_rank * num_columns) / num_processes;
    my_last_column = ((my_rank + 1 ) * num_columns) / num_processes;
    if (my_last_column > num_columns) {
      my_last_column = num_columns;
    }
  } else {
    my_first_column = 1;
    my_last_column = num_columns;
  }
  assert(1 <= my_first_column);
  assert(my_first_column <= my_last_column);
  assert(my_last_column <= num_columns);

  /* Prefix sums of our subset of columns. */
  for (int j = my_first_column; j <= my_last_column; ++j) {
    MATRIX_ARR(0, j) = 0;
  }
  for (int i = 1; i <= num_rows; ++i) {
    for (int j = my_first_column; j <= my_last_column; ++j) {
      MATRIX_ARR(i, j) += MATRIX_ARR(i - 1, j);
    }
  }
  MICROPROF_END(column_sums);

  struct PartialSum best = {
    MATRIX_ARR(1, my_first_column) - MATRIX_ARR(0, my_first_column),
    1, my_first_column, 1, my_first_column
  };
#define UPDATE_BEST(_current_, _i_, _j_, _k_, _l_) \
  if (best.sum < _current_) {                         \
    best.sum = _current_;                             \
    best.i = _i_; best.j = _j_;                       \
    best.k = _k_; best.l = _l_;                       \
  }

  /* Our hybrid algorithm. */
  if (degenerate) {
    MICROPROF_START(kadanes_2d_degenerate);
    for (int i = 1; i <= num_rows; ++i) {
      for (int k = i; k <= num_rows; ++k) {
        assert(i > 0 && k >= i);
#define COLUMN_SUM(_j_) (MATRIX_ARR(k, _j_) - MATRIX_ARR(i - 1, _j_))
        struct SubSolution local = {
          /* T */
          0LL,
          /* P, Pj, Pl */
          COLUMN_SUM(my_first_column), my_first_column, my_first_column,
          /* S, Sj, Sl */
          COLUMN_SUM(my_last_column), my_last_column, my_last_column,
          /* M, Mj, Ml */
          COLUMN_SUM(my_first_column), my_first_column, my_first_column
        };
        /* Forward run of 1D Kadane's algorithm. */
        long long current = -1;
        for (int j = my_first_column, l = j; l <= my_last_column; ++l) {
          assert(j > 0 && l >= j);
          if (current < 0) {
            current = 0;
            j = l;
          }
          local.T += COLUMN_SUM(l);
          current += COLUMN_SUM(l);
          if (current > local.M) {
            local.M = current;
            local.Mj = j;
            local.Ml = l;
          }
          if (local.T > local.P) {
            local.P = local.T;
            local.Pj = my_first_column;
            local.Pl = l;
          }
          assert(my_first_column <= local.Pj);
          assert(local.Pj <= local.Pl);
          assert(local.Pj == my_first_column);
          assert(my_first_column <= local.Mj);
          assert(local.Mj <= local.Ml);
          assert(local.Ml <= my_last_column);
        }
        /* Maximum suffix sum. */
        current = 0;
        for (int j = my_last_column; j >= my_first_column; --j) {
          current += COLUMN_SUM(j);
          if (current > local.S) {
            local.S = current;
            local.Sj = j;
            local.Sl = my_last_column;
          }
          assert(my_first_column <= local.Sj);
          assert(local.Sj <= local.Sl);
          assert(local.Sl == my_last_column);
        }
#undef COLUMN_SUM
        MICROPROF_START(reduction_degenerate);
        MPI_Reduce(my_rank == kRootRank ? MPI_IN_PLACE : &local, &local, 1,
            sub_solution_t, join_sub_solution_op, kRootRank, MPI_COMM_WORLD);
        MICROPROF_END(reduction_degenerate);
        if (my_rank == kRootRank) {
          assert(local.Mj <= local.Ml);
          UPDATE_BEST(local.M, i, local.Mj, k, local.Ml);
        }
      }
    }
    MICROPROF_END(kadanes_2d_degenerate);
  } else {
    MICROPROF_START(kadanes_2d);
    /* The reason for such weird dispatch is that for num_columns > 1,
     * num_columns^2 has strictly more divisors than num_columns. */
    for (int dispatch = 0, i = 1; i <= num_rows; ++i) {
      for (int k = i; k <= num_rows; ++k) {
        assert(i > 0 && k >= i);
        ++dispatch;
        if (dispatch == num_processes) {
          dispatch = 0;
        }
        if (dispatch != my_rank) {
          continue;
        }
#define COLUMN_SUM(_j_) (MATRIX_ARR(k, _j_) - MATRIX_ARR(i - 1, _j_))
        long long current = -1;
        for (int j = 1, l = 1; l <= num_columns; ++l) {
          assert(j > 0 && l >= j);
          if (current < 0) {
            current = 0;
            j = l;
          }
          current += COLUMN_SUM(l);
          UPDATE_BEST(current, i, j, k, l);
        }
#undef COLUMN_SUM
      }
    }
    MICROPROF_START(reduction);
    MPI_Reduce(my_rank == kRootRank ? MPI_IN_PLACE : &best, &best, 1,
        partial_sum_t, max_partial_sum_op, kRootRank, MPI_COMM_WORLD);
    MICROPROF_END(reduction);
    MICROPROF_END(kadanes_2d);
  }
#undef UPDATE_BEST

#ifndef NDEBUG
  {
    for (int j = 1; j <= num_columns; ++j) {
      MATRIX_ARR(0, j) = 0;
    }
    for (int i = 1; i <= num_rows; ++i) {
      for (int j = 1; j <= num_columns; ++j) {
        if (j < my_first_column || my_last_column < j) {
          MATRIX_ARR(i, j) += MATRIX_ARR(i - 1, j);
        }
      }
    }
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
  const double end_time = MPI_Wtime();

  if (my_rank == kRootRank) {
    fprintf(stderr,
        "PWIR2014_Mateusz_Machalica_305678 "
        "Input: (%d,%d,%d) Solution: |(%d,%d),(%d,%d)|=%lld Time: %.10f\n",
        num_rows, num_columns, seed,
        best.i, best.j, best.k, best.l,
        best.sum, end_time - start_time);
  }

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
  finalize_mpi();

  return err;
}
