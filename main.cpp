#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
void get_file_stats(std::ifstream& inputFile, int& numberOfRows, int& numberOfCols, int& maxGrayVal, std::stringstream& strStream)
{
    std::string line;
    getline(inputFile,line);
    std::cout << "Version of PGM Image: " << line << "\n";
    strStream << inputFile.rdbuf();
    strStream >> numberOfCols >> numberOfRows >> maxGrayVal;
    std::cout << "Rows present: " << numberOfRows;
    std::cout <<  " Columns present: " << numberOfCols << "\n";
    std::cout << "ASCII decimal for maximum gray value: " << maxGrayVal << "\n";
}
void set_value_array(int **array, int& rows, int& columns, std::stringstream& strStream)
{
    int row, col;
    for ( row = 0; row < rows; ++row )
        for( col = 0; col < columns; ++col)
            strStream >> array[row][col];
}
void display_array(int **array, int& rows, int& cols)
{
    for ( int i=0; i<rows; i++ )
    {
        for ( int j=0; j<cols; j++ )
        {
            std::cout << array[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
int main()
{
    int numberOfRows, numberOfCols, maxGrayVal;
    std::string filename;
    std::stringstream strStream;
    filename = "test.pgm";
    std::ifstream inputFile(filename);

    get_file_stats(inputFile, numberOfRows, numberOfCols, maxGrayVal, strStream);
    int **valueArray;
    valueArray = new int *[numberOfRows];
    for (int i=0; i<numberOfCols; i++) valueArray[i] = new int[numberOfRows];
    set_value_array(valueArray, numberOfRows, numberOfRows, strStream);
    display_array(valueArray, numberOfRows, numberOfCols);
    inputFile.close();
    return 0;
}