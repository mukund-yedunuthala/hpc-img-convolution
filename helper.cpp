/**
 * \file
 * \brief Definitions of helper functions
 * \author Venkata Mukund Kashyap Yedunuthala
 * \date 15 March 2023
 */ 


#include "helper.h"
void printinfo(
    unsigned int& numRows,
    unsigned int& numCols,
    unsigned short int& maxVal
) {
    std::cout << "Image convolution using MPI\n";
    std::cout << "High Performance Computing and Optimization";
    std::cout << " | WS2020-21\n";
    std::cout << "Technische Universität Bergakademie Freiberg\n";
    std::string separator(75,'=');
    std::cout << separator << "\n";
    std::cout << "Version of PGM Image: P2\n";
    std::cout << "Rows present: " <<numRows;
    std::cout <<  " Columns present: " << numCols << "\n";
    std::cout << "Maximum gray value: " << maxVal << "\n";
}

void set_kernels(
    int**& edgeDetection,
    int**& identity,
    int**& sharpen,
    double**& gaussBlur
) {
    // std::cout << "Address: " << &gaussBlur << std::endl;
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            edgeDetection[i][j] = -1;
            identity[i][j] = 0;
            if((i+j)%2!=0)
            {
                gaussBlur[i][j] = 0.0625 * 2;
                sharpen[i][j] = -1;
            }
            else
            {
                gaussBlur[i][j] = 0.0625 * 1;
                sharpen[i][j] = 0;
            }
            if (i==1&&j==1)
            {
                gaussBlur[i][j] = 0.0625 * 4;
                sharpen[i][j] = 5;
                edgeDetection[i][j] = 8;
                identity[i][j] = 1;
            }
        }
    }
}

void set_input(int*& array, unsigned long int& N, std::stringstream& strStream) {
    for (size_t i = 0; i < N; i++) {
        strStream >> array[i];
    }
    
}

void set_array(int*& array, unsigned long int& size, int*& inputArr) {
    for (size_t i = 0; i < size; i++) {
        array[i] = inputArr[i];
    }
}

void set_array(int*& array, unsigned long int& size, int& value) {
    for (size_t i = 0; i < size; i++) {
        array[i] = value;
    }
}

void print_time_info(double& time) {
    std::string separator(75,'-');
    std::cout << separator << "\n" << "Time taken: " \
        << time << "\n" << separator << "\n";
}