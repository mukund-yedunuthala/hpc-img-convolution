#ifndef MPI_CONVOLUTIONS_H
#define MPI_CONVOLUTIONS_H
#include <iostream>
#include <mpi.h>
#include "kernels.h"
#include "helper.h"
void setup_convolution_new(
    Kernels& kernels,
    unsigned int& rows,
    unsigned int& cols,
    int*& valueArray,
    int*& resultArray
);
#endif