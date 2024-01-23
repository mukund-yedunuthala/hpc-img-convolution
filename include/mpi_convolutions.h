/**
 * \file
 * \brief Header file for MPI based convolutions
 * \author Venkata Mukund Kashyap Yedunuthala
 * \date 23 January 2024
 */

#ifndef MPI_CONVOLUTIONS_H
#define MPI_CONVOLUTIONS_H
#include <iostream>
#include <mpi.h>
#include "kernels.h"
#include "helper.h"
/**
 * \brief Performs convolution of a given array with a given kernel 
 * in parallel using MPI. 
 * 
 * Performs MPI scatter and gather operations. Additional information 
 * regarding halo, i.e. rows present above and below a portion of input 
 * array are also communicated. 
 * \param kernels Kernels struct object containing all the information.
 * \param rows Number of rows present in input array.
 * \param cols Number of columns present in input array.
 * \param valueArray Input array.
 * \param resultArray Output array.
*/

void setup_convolution_new(
    Kernels& kernels,
    unsigned int& rows,
    unsigned int& cols,
    int*& valueArray,
    int*& resultArray
);
#endif