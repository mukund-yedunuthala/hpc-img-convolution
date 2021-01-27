#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

void display_array(int **array, int& rows, int& cols)
{
    for ( int i=rows-1; i>=0; i-- )
    {
        for ( int j=0; j<cols; j++ )
        {
            std::cout << array[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

int multiply(int **a, int **b) 
{   
    int result=0;
    for (int i=0; i<3; i++)
        {
            for(int j=0; j<3; j++)
            {
                result=a[i][j]*1;
                // result = 255;
            }
        }
    return result;
}

int** slice(int size, int row, int col, int** mat, int rows, int cols)
{   
    int** res;
    res = new int*[size];
    for ( int h=0; h<size; h++ ) res[h] = new int[size];
    for ( int h=0; h<size; h++ )
    {
        for ( int w=0; w<size; w++ )
        {   
            res[h][w] = mat[(row+h+rows-1)%rows][(col+w+cols-1)%cols];
        }
    }
    return res;
}

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
    int h,w;
    for ( h=7; h>=0; h-- )
        for( w=0;  w<columns; w++ )
            strStream >> array[h][w];
    std::cout << "Conversion of values to array complete. \n";
}

void write_to_image(int **array, int& rows, int& cols, int& greyvalue)
{
    std::cout << "Writing resultant array to image in PGM format... \n";
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

void set_kernels(int **edgeDetection, int **sharpen, int **gaussianBlur, int& s)
{
    for ( int i=0; i<3; i++)
    {
        for( int j=0; j<3; j++)
        {
            edgeDetection[i][j] = -1;
            if((i+j)%2!=0)
            {
                gaussianBlur[i][j] = 2;
                sharpen[i][j] = -1;
            }
            else
            {
                gaussianBlur[i][j] = 1;
                sharpen[i][j] = 0;
            }
            if (i==1&&j==1)
            {
                gaussianBlur[i][j] = 4;
                sharpen[i][j] = 5;
                edgeDetection[i][j] = 8;
            }
        }
    }
    std::cout << "Kernel matrices set. \n";
}

void apply_convolution( int **valueArray, 
                        int **resultArray, 
                        int **operation,
                        int& rows,
                        int& cols)
{
    int s=3;
    std::cout << "Applying convolution kernel... \n";
    for (int row=0; row<rows; row++)
        for(int col=0; col<cols; col++)
        {   
            resultArray[row][col]=0;
            int **slicedArray = slice(s,row,col,valueArray,rows,cols);
            resultArray[row][col] = multiply(slicedArray,operation);  
        }
    std::cout << "Application complete. \n";
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
    int **gaussianBlur;
    edgeDetection = new int *[size];
    sharpen = new int *[size];
    gaussianBlur = new int *[size];
    for(int j=0; j<size; j++)
    {
        edgeDetection[j] = new int[size];
        sharpen[j] = new int[size];
        gaussianBlur[j] = new int[size];
    }
    printf("Updating kernel arrays... \n");
    set_kernels(edgeDetection, sharpen, gaussianBlur, size);

    int **resultArray;
    resultArray = new int *[numberOfRows];
    for (int i=0; i<numberOfCols; i++) resultArray[i] = new int[numberOfCols];
    // apply_convolution(valueArray, resultArray, edgeDetection, numberOfRows, numberOfCols);
    // write_to_image(resultArray, numberOfRows, numberOfCols, maxGrayVal);
    return 0;
}