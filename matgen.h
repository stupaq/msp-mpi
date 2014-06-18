/** Copyright (C) Mateusz Machalica, 2014. */

#ifndef MATGEN_H_
#define MATGEN_H_

struct matgen_s;
typedef struct matgen_s matgen_t;

/** Creates a new matrix generator from given seed. */
matgen_t* matgenNew(int numRows, int numColumns, int seed);

/** Returns the next matrix element row-wise. */
int matgenGenerate(matgen_t* matgenPtr);

/** Destroys a given matrix generator. */
void matgenDestroy(matgen_t* matgenPtr);

#endif  // MATGEN_H_
