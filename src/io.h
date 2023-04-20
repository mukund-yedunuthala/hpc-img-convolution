/**
 * \file
 * \brief Header file for input/output operations
 * \author Venkata Mukund Kashyap Yedunuthala
 * \date 15 March 2023
 */ 

#ifndef IO_H
#define IO_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

/**
 * \brief Reads data from a PGM format image file
 * \param inputFile Input file stream.
 * \param numberOfRows Stores number of rows from the file.
 * \param numberOfCols Stores number of columns from the file.
 * \param maxGrayval Stores maximum value from the file.
 * \param strStream Input string stream.
*/
void get_file_data(
        std::ifstream& inputFile, 
        unsigned int& numberOfRows, 
        unsigned int& numberOfCols, 
        unsigned short int& maxGrayVal, 
        std::stringstream& strStream);
/**
 * \brief Writes an array to a PGM (P2) file.
 * \param array Array to be written to file.
 * \param rows Number of rows to be written.
 * \param cols Number of cols to be written. 
 * \param maxValue Maximum value in file.
 * \param fileName File name string.
*/
void write_to_file(
    int*& array, 
    unsigned int& rows, 
    unsigned int& cols, 
    unsigned short int& maxValue, 
    std::string& fileName
);

#endif