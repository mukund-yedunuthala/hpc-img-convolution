#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "operations.h"

void get_file_stats(std::ifstream& inputFile, int& numberOfRows, int& numberOfCols, int& maxGrayVal, std::stringstream& strStream)
{
    std::string line;
    getline(inputFile,line);
    std::cout << "Version of PGM Image: " << line << "\n";
    strStream << inputFile.rdbuf();
    strStream >> numberOfCols >> numberOfRows >> maxGrayVal;
    std::cout << "Rows present: " << numberOfRows << " Columns present: " << numberOfCols << "\n";
    std::cout << "ASCII decimal for maximum gray value: " << maxGrayVal << "\n";
}