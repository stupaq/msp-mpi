#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "matgen.h"

static void printUsage(char const * prog)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "    %s <num_rows> <num_colums> <seed>\n\n", prog);
}

static void printMatrix(long long int const * m, int r, int c)
{
    int i, j;
    for (i = 0; i <= r; ++i)
    {
        for (j = 0; j <= c; ++j)
        {
            printf(" %lld", *m++);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char * argv[])
{
    int numRows, numColumns, seed;

    MPI_Init(&argc, &argv);

    if (argc != 4)
    {
        fprintf(stderr, "ERROR: Invalid arguments!\n");
        printUsage(argv[0]);
        MPI_Finalize();
        exit(1);
    }
    numRows = atoi(argv[1]);
    numColumns = atoi(argv[2]);
    seed = atoi(argv[3]);
    if (numRows <= 0 || numColumns <= 0 || seed <= 0)
    {
        fprintf(stderr, "ERROR: Invalid arguments: %s %s %s!\n", argv[1],
                argv[2], argv[3]);
        printUsage(argv[0]);
        MPI_Finalize();
        exit(1);
    }

    /* FIXME */

    fprintf(stderr, "PWIR2014_Jan_Kowalski_123456 Input: (%d,%d,%d) Solution: |(%d,%d),(%d,%d)|=%lld Time: %.10f\n",
            numRows, numColumns, seed,
            0, 0, 0, 0, 0L, 0.0f);

    MPI_Finalize();
    return 0;
}
