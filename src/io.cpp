/**
 * \file
 * \brief Defintions file for input/output operations
 * \author Venkata Mukund Kashyap Yedunuthala
 * \date 23 January 2024
 */ 

#include "../include/io.h"

void get_file_data(
    std::ifstream& inputFile, 
    unsigned int& numberOfRows, 
    unsigned int& numberOfCols, 
    unsigned short int& maxGrayVal, 
    std::stringstream& strStream
    ) {
            std::string line;
            getline(inputFile,line);
            strStream << inputFile.rdbuf();
            strStream >> numberOfCols >> numberOfRows >> maxGrayVal;
    }
    
void write_to_file(
    int*& array, 
    unsigned int& rows, 
    unsigned int& cols, 
    unsigned short int& maxValue, 
    std::string& fileName
) {
    std::ofstream optFile(fileName);
    optFile << "P2\n" << rows << " " << cols << "\n";
    optFile << maxValue << "\n";
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            optFile << array[(i*cols)+j] << " ";
        }
        optFile << "\n";
    }
    optFile.close();
}