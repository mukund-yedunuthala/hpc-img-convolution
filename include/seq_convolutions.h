/**
 * \file
 * \brief Header file for sequential convolutions
 * \author Venkata Mukund Kashyap Yedunuthala
 * \date 23 January 2024
 */
#ifndef SEQ_CONVOLUTIONS_H
#define SEQ_CONVOLUTIONS_H 
#include "kernels.h"

/**
 * \brief Performs convolution of a given array with a given kernel 
 * sequentially. 
 * 
 * \param kernels Kernels struct object containing all the information.
 * \param rows Number of rows present in input array.
 * \param cols Number of columns present in input array.
 * \param valueArray Input array.
 * \param resultArray Output array.
*/

void seq_convolution(
    Kernels& kernels,
    unsigned int& rows,
    unsigned int& cols,
    int*& valueArray,
    int*& resultArray
);

#endif