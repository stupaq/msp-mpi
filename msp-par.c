#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>

#include "./matgen.h"
#include "./microprof.h"

static void printUsage(char const * prog) {
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "    %s <num_rows> <num_colums> <seed>\n\n", prog);
}

int main(int argc, char * argv[]) {
  int err = 0;
  matgen_t* matgenPtr = NULL;
  long long* matrixPtr = NULL;

  /* MPI SETUP */
  MPI_Init(&argc, &argv);

  int numProcesses, myRank;
  MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  /* PARSING ARGUMENTS */
  if (argc != 4) {
    fprintf(stderr, "ERROR: Invalid arguments!\n");
    err = 1;
    goto exit;
  }
  const int numRows = atoi(argv[1]),
        numColumns = atoi(argv[2]),
        seed = atoi(argv[3]);
  if (numRows <= 0 || numColumns <= 0 || seed <= 0) {
    fprintf(stderr, "ERROR: Invalid arguments: %s %s %s!\n", argv[1],
        argv[2], argv[3]);
    err = 1;
    goto exit;
  }

  /* GENERATING INPUT */
  matgenPtr = matgenNew(numRows, numColumns, seed);
  if (matgenPtr == NULL) {
    fprintf(stderr, "ERROR: Unable to create the matrix generator!\n");
    err = 1;
    goto exit;
  }
  matrixPtr = (long long*) malloc(sizeof(long long) * (numRows + 1) *
      (numColumns + 1));
  if (matrixPtr == NULL) {
    fprintf(stderr, "ERROR: Unable to create the matrix!\n");
    err = 1;
    goto exit;
  }
#define matrixArr(i, j) matrixPtr[(i) * (numColumns + 1) + (j)]
  for (int i = 1; i <= numRows; ++i) {
    for (int j = 1; j <= numColumns; ++j) {
      matrixArr(i, j) = matgenGenerate(matgenPtr);
    }
  }
  matgenDestroy(matgenPtr);
  matgenPtr = NULL;

  const double startTime = MPI_Wtime();
  /* ACTUAL COMPUTATION */
  // TODO(stupaq) transpose when generating matrix
  MICROPROF_WARN(numRows < numColumns, "Sub-optimal matrix layout: M < N");

  MICROPROF_START(row_prefix_sums);
  for (int j = 0; j <= numColumns; ++j) {
    matrixArr(0, j) = 0;
  }
  for (int i = 1; i <= numRows; ++i) {
    matrixArr(i, 0) = 0;
  }
  for (int j = 1; j <= numColumns; ++j) {
    for (int i = 1; i <= numRows; ++i) {
      matrixArr(i, j) += matrixArr(i - 1, j);
    }
  }
  MICROPROF_END(row_prefix_sums);

  int bestI, bestJ, bestK, bestL;
  bestI = bestJ = bestK = bestL = 1;
  long long maxSum = matrixArr(1, 1);
#define updateMaxSum(sum, i, j, k, l) \
  if (maxSum < current) {             \
    maxSum = current;                 \
    bestI = i; bestJ = j;             \
    bestK = k; bestL = l;             \
  }

  for (int j = 1; j <= numColumns; ++j) {
    for (int l = j; l <= numColumns; ++l) {
      assert(l >= j);
#define rowSum(i) (matrixArr(i, l) - matrixArr(i, j - 1))
      long long current = -1, nextDiff = rowSum(1);
      for (int i = 1, k = 1; k <= numRows; ++k) {
        assert(i <= k);
        if (current < 0) {
          current = 0;
          i = k;
        }
        current += nextDiff;
        if (k == numRows || (nextDiff = rowSum(k + 1)) < 0) {
          updateMaxSum(current, i, j, k, l);
        }
      }
#undef rowSum
    }
  }
  /* FIXME */

  /* DONE */
  const double endTime = MPI_Wtime();

  if (myRank == 0) {
    fprintf(stderr,
        "PWIR2014_Mateusz_Machalica_305678 "
        "Input: (%d,%d,%d) Solution: |(%d,%d),(%d,%d)|=%lld Time: %.10f\n",
        numRows, numColumns, seed,
        bestJ, bestI, bestK, bestL, maxSum, endTime - startTime);
  }

exit:
  /* CLEANUP */
#undef updateMaxSum
  if (matgenPtr) {
    matgenDestroy(matgenPtr);
  }
#undef matrixArr
  free(matrixPtr);
  if (err) {
    printUsage(argv[0]);
  }
  MPI_Finalize();

  return err;
}
