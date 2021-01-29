#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

void display_array(unsigned int **array, int& rows, int& cols)
{
    for ( int i=0; i<rows; i++ )
    {
        for ( int j=0; j<cols; j++ )
        {
            std::cout << array[i][j] << " ";
        }
        std::cout << "\n";
    }
}

void get_file_data(std::ifstream& inputFile, int& numberOfRows, int& numberOfCols, int& maxGrayVal, std::stringstream& strStream)
{
    std::string line;
    getline(inputFile,line);
    std::cout << "Version of PGM Image: " << line << "\n";
    strStream << inputFile.rdbuf();
    strStream >> numberOfCols >> numberOfRows >> maxGrayVal;
    numberOfCols+=2; numberOfRows+=2;
    std::cout << "Rows present: " << numberOfRows-2;
    std::cout <<  " Columns present: " << numberOfCols-2 << "\n";
    std::cout << "ASCII decimal for maximum gray value: " << maxGrayVal << "\n";
    std::cout << "Reading image successful. \n"; 
}

void set_value_array(unsigned int **array, int& rows, int& columns, std::stringstream& strStream)
{
    int h,w;
    for ( h=0; h<rows; h++ )
        for( w=0;  w<columns; w++ )
        {
            if(h<(rows-2)&&w<(columns-2))
                strStream >> array[h][w];
            else
                array[h][w] = 127;         
        }
    std::cout << "Conversion of values to array complete. \n";
}

void write_to_image(unsigned int **array, int& rows, int& cols, int& greyvalue)
{
    std::cout << "Writing resultant array to image in PGM format... \n";
    std::ofstream outputFile("result-final.pgm");
    outputFile << "P2\n" << rows-2 << " " << cols-2 << "\n";
    outputFile << greyvalue << "\n";
    for ( int i=0; i<rows-2; i++ )
    {
        for ( int j=0; j<cols-2; j++ )
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

void apply_convolution( unsigned int **valueArray, 
                        unsigned int **resultArray, 
                        int **operation,
                        int& rows,
                        int& cols)
{
    int s = 3;
    for(int x=0; x<rows; x++)
    {
        for(int y=0; y<cols; y++)
        {
            unsigned int acc = 0;
            for(int i=0; i<s; i++)
            {
                for(int j=0; j<s; j++)
                {
                    acc+= valueArray[(x-i+2)%rows][(y-j+2)%cols]*operation[i][j];
                }
            }
            valueArray[x][y] = acc;
        }
    }
    std::cout<<"Convolution application complete. \n";
}

void apply_gaussian_blur(   unsigned int **valueArray, 
                            unsigned int **resultArray, 
                            int **operation,
                            int& rows,
                            int& cols,
                            int instances)
{   
    int s = 3;
    while (instances>0)
    {
    for(int x=0; x<rows; x++)
    {
        for(int y=0; y<cols; y++)
        {
            unsigned int acc = 0;
            for(int i=0; i<s; i++)
            {
                for(int j=0; j<s; j++)
                {
                    acc+= 0.0625*valueArray[(x-i+2)%rows][(y-j+2)%cols]*operation[i][j];
                }
            }
            valueArray[x][y] = acc;
        }
    }
    instances--;
    }
}

int main()
{
    int numberOfRows, numberOfCols, maxGrayVal;
    std::string fileName;
    std::stringstream strStream;
    fileName = "512.pgm";
    std::ifstream inputFile(fileName);
    get_file_data(inputFile, numberOfRows, numberOfCols, maxGrayVal, strStream);
    inputFile.close();

    unsigned int **valueArray;
    valueArray = new unsigned int *[numberOfRows];
    for (int i=0; i<numberOfCols; i++) valueArray[i] = new unsigned int[numberOfCols];
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

    unsigned int **resultArray;
    resultArray = new unsigned int *[numberOfRows];
    for (int i=0; i<numberOfCols; i++) resultArray[i] = new unsigned int[numberOfCols];
    // apply_gaussian_blur(valueArray,resultArray,gaussianBlur,numberOfRows,numberOfCols,5);
    // apply_convolution(valueArray, resultArray, sharpen, numberOfRows, numberOfCols);
    apply_convolution(valueArray, resultArray, edgeDetection, numberOfRows, numberOfCols);
    write_to_image(valueArray, numberOfRows, numberOfCols, maxGrayVal);

    for(int row = 0; row < numberOfRows; row++)
    {
        delete[] valueArray[row];
		delete[] resultArray[row];
	}
	delete[] valueArray;
	delete[] resultArray;
    for(int k=0; k<size; k++)
    {
        delete[] edgeDetection[k];
        delete[] gaussianBlur[k];
        delete[] sharpen[k];
    }
    delete[] edgeDetection;
    delete[] gaussianBlur;
    delete[] sharpen;
    return 0;
}