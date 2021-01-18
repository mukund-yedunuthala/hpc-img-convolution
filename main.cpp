#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "operations.h"

int main()
{
    int numberOfRows, numberOfCols, row, column, maxGrayVal;
    std::string filename;
    std::stringstream strStream;
    filename = "test.pgm";
    std::ifstream inputFile(filename);

    get_file_stats(inputFile, numberOfRows, numberOfCols, maxGrayVal, strStream);
    int valueArray[numberOfRows][numberOfCols];
    for( row = 0; row < numberOfRows; ++row )
    {
        for ( column = 0; column < numberOfCols; ++column )
        {
            strStream >> valueArray[row][column];
        }
    }
    inputFile.close();
    return 0;
}