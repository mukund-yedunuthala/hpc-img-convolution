/**
 * \file
 * \brief Header file for helper functions
 * \author Venkata Mukund Kashyap Yedunuthala
 * \date 15 March 2023
 */ 


#ifndef HELPER_H
#define HELPER_H
#include <iostream>
#include <sstream>
/**
 * \brief Prints out introductory info to terminal
 * \param numRows Number of rows present in input image file.
 * \param numCols Number of columns present in input image file.
 * \param maxVal Maximum value as seen in input image file.
*/
void printinfo(
    unsigned int& numRows,
    unsigned int& numCols,
    unsigned short int& maxVal
);

/**
 * \brief Initiates convolution kernels.
 * \param edgeDetection Edge detection kernel.
 * \param identity Identity kernel.
 * \param sharpen Sharpening kernel.
 * \param gaussBlur Gaussian blur kernel.
*/
void set_kernels(
    int**& edgeDetection,
    int**& identity,
    int**& sharpen,
    double**& gaussBlur
);

/**
 * \brief Initiates a given array with values present in stringstream. 
 * \param array Array to which values are written to.
 * \param N Size of the array.
 * \param strStream Input stringstream.
*/
void set_input(int*& array, unsigned long int& N, std::stringstream& strStream);

/**
 * \brief Initiates a given array with values present in another array. 
 * \param array Array to which values are written to.
 * \param size Size of the array.
 * \param inputArr Input array.
*/
void set_array(int*& array, unsigned long int& size, int*& inputArr);

/**
 * \brief Initiates a given array with a constant value. 
 * \param array Array to which values are written to.
 * \param size Size of the array.
 * \param value Constant value.
*/
void set_array(int*& array, unsigned long int& size, int& value);

/**
 * \brief Prints a given 2d (nxn) array to terminal. 
 * \param array Input array.
 * \param size Size of the array.
*/
template <typename printT> 
    void print_2d_array(printT **array, const unsigned short int& size)
    {
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++) {
                std::cout << array[i][j] << ' ';
            }
            std::cout << "\n";
        }
    };

/**
 * \brief Prints a given array to terminal. 
 * \param array Input array.
 * \param rows Rows in array.
 * \param cols Columns in array.  
*/
template <typename T>
void print_result(T *array, size_t rows, size_t cols) {
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            std::cout << array[(i*cols)+j] << " ";
        }
        std::cout << "\n";
    }
};

/**
 * \brief Prints total time taken to terminal. 
 * \param time Calculated wall time taken. 
*/
void print_time_info(double& time);


void printMaxValue(const int* const& buffer, int bufferSize); 
#endif