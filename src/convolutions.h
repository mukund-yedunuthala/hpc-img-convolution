/**
 * \file
 * \brief Header file for convolutions
 * \author Venkata Mukund Kashyap Yedunuthala
 * \date 15 March 2023
 */ 
#include <math.h>
/**
 * \brief Performs convolution parameter with a given kernel on a given array.
 * \param valueArray Input array.
 * \param resultArray Output array.
 * \param operation Convolution kernel.
 * \param rows Number of rows present in input array.
 * \param cols Number of columns present in input array.
 * \param size Size of the convolution kernel array.
 * \param instances Number of convolution instances.
*/
template <typename kernel>
void convolution(
    int*& valueArray, 
    int*& resultArray, 
    kernel**& operation, 
    unsigned int& rows, 
    unsigned int& cols, 
    unsigned short int& size,
    unsigned short int& instances = 1
) {
    while (instances > 0) {
        for (size_t x = 0; x < rows; x++) {
            for (size_t y = 0; y < cols; y++) {
                kernel sum {};
                for (size_t i = 0; i < size; i++) {
                    for (size_t j = 0; j < size; j++) {
                        sum += valueArray[((x+1-i))*cols+((y+1-j))] \
                            * operation[i][j];
                    }
                }
                if (sum>=0 && sum<=255)
                    resultArray[(x*cols)+y] = sum;

                else if (sum<0)
                    resultArray[(x*cols)+y] = 0;

                else
                    resultArray[(x*cols)+y] = 255;
            }
        }   
        instances--;
        for (size_t i = 0; i < (rows * cols); i++) {
            valueArray[i] = resultArray[i];
        }
    }
};