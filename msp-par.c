#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "matgen.h"

static void printUsage(char const * prog) {
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "    %s <num_rows> <num_colums> <seed>\n\n", prog);
}

static void printMatrix(long long int const * m, int r, int c) {
  int i, j;
  for (i = 0; i <= r; ++i) {
    for (j = 0; j <= c; ++j) {
      printf(" %lld", *m++);
    }
    printf("\n");
  }
  printf("\n");
}

int main(int argc, char * argv[]) {
  MPI_Init(&argc, &argv);

  int numProcesses, myRank;
  MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  if (argc != 4) {
    fprintf(stderr, "ERROR: Invalid arguments!\n");
    goto failed;
  }
  const int numRows = atoi(argv[1]),
        numColumns = atoi(argv[2]),
        seed = atoi(argv[3]);
  if (numRows <= 0 || numColumns <= 0 || seed <= 0) {
    fprintf(stderr, "ERROR: Invalid arguments: %s %s %s!\n", argv[1],
        argv[2], argv[3]);
    goto failed;
  }

  matgen_t * matgenPtr = matgenNew(numRows, numColumns, seed);
  if (matgenPtr == NULL) {
    fprintf(stderr, "ERROR: Unable to create the matrix generator!\n");
    goto failed;
  }
  long long int * matrixPtr = (long long int *) malloc(
      sizeof(long long int) * (numRows + 1) * (numColumns + 1));
  if (matrixPtr == NULL) {
    fprintf(stderr, "ERROR: Unable to create the matrix!\n");
    goto failed;
  }
  int i, j, k, l;
  for (j = 1; j <= numRows; ++j) {
    for (i = 1; i <= numColumns; ++i) {
      matrixPtr[j * (numColumns + 1) + i] = matgenGenerate(matgenPtr);
    }
  }
  matgenDestroy(matgenPtr);

  const double startTime = MPI_Wtime();

  /* FIXME */

  const double endTime = MPI_Wtime();

  if (myRank == 0) {
    fprintf(stderr, "PWIR2014_Mateusz_Machalica_305678 Input: (%d,%d,%d) Solution: |(%d,%d),(%d,%d)|=%lld Time: %.10f\n",
        numRows, numColumns, seed,
        0, 0, 0, 0, 0LL, endTime - startTime);
  }

  MPI_Finalize();
  return 0;

failed:
  printUsage(argv[0]);
  MPI_Finalize();
  return 1;
}
