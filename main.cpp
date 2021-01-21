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
    std::cout << "Reading image successful. \n"; 
}

void set_value_array(int **array, int& rows, int& columns, std::stringstream& strStream)
{
    int row, col;
    for ( row = 0; row < rows; ++row )
        for( col = 0; col < columns; ++col)
            strStream >> array[row][col];
    std::cout << "Conversion of values to array complete. \n";
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

void write_to_image(int **array, int& rows, int& cols, int& greyvalue)
{
    std::ofstream outputFile("result.pgm");
    outputFile << "P2\n" << rows << " " << cols << "\n";
    outputFile << greyvalue << "\n";
    for ( int i=0; i<rows; i++ )
    {
        for ( int j=0; j<cols; j++ )
        {
            outputFile << array[i][j] << " ";
        }
        outputFile << "\n";
    }
    std::cout << "Creation of resultant image complete. \n";
    outputFile.close();
}

void set_kernels(int **edgeDetection, int **sharpen, double **gaussianBlur, int& s)
{
    for ( int i=0; i<3; i++)
    {
        for( int j=0; j<3; j++)
        {
            edgeDetection[i][j] = -1;
            if((i+j)%2!=0)
            {
                gaussianBlur[i][j] = 0.0625*2;
                sharpen[i][j] = -1;
            }
            else
            {
                gaussianBlur[i][j] = 0.0625*1;
                sharpen[i][j] = 0;
            }
            if (i==1&&j==1)
            {
                gaussianBlur[i][j] = 0.0625*4;
                sharpen[i][j] = 5;
                edgeDetection[i][j] = 8;
            }
        }
    }
    std::cout << "Kernel matrices set. \n";
}
int main()
{
    int numberOfRows, numberOfCols, maxGrayVal;
    std::string fileName;
    std::stringstream strStream;
    fileName = "test.pgm";
    std::ifstream inputFile(fileName);
    get_file_stats(inputFile, numberOfRows, numberOfCols, maxGrayVal, strStream);
    inputFile.close();

    int **valueArray;
    valueArray = new int *[numberOfRows];
    for (int i=0; i<numberOfCols; i++) valueArray[i] = new int[numberOfCols];
    set_value_array(valueArray, numberOfRows, numberOfRows, strStream);
    
    int size = 3; 
    int **edgeDetection, **sharpen;
    double **gaussianBlur;
    edgeDetection = new int *[size];
    sharpen = new int *[size];
    gaussianBlur = new double *[size];
    for(int j=0; j<size; j++)
    {
        edgeDetection[j] = new int[size];
        sharpen[j] = new int[size];
        gaussianBlur[j] = new double[size];
    }
    printf("Updating kernel arrays... \n");
    set_kernels(edgeDetection, sharpen, gaussianBlur, size);
    // write_to_image(valueArray, numberOfRows, numberOfCols, maxGrayVal);
    return 0;
}