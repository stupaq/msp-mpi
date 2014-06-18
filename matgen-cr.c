/** Copyright (C) Mateusz Machalica, 2014. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matgen.h"

struct matgen_s {
  unsigned int seed;
};

matgen_t* matgenNew(int numRows, int numColumns, int seed) {
  matgen_t* matgenPtr = NULL;
  if (numRows <= 0 || numColumns <= 0 || seed <= 0) {
    return NULL;
  }
  matgenPtr = (matgen_t*) malloc(sizeof(struct matgen_s));
  if (matgenPtr != NULL) {
    matgenPtr->seed = seed;
  }
  return matgenPtr;
}

int matgenGenerate(matgen_t* matgenPtr) {
  return rand_r(&matgenPtr->seed) - RAND_MAX / 2;
}

void matgenDestroy(matgen_t * matgenPtr) {
  free(matgenPtr);
}

