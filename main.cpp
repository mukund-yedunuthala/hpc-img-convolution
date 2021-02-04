#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

void get_file_data(std::ifstream& inputFile, int& numberOfRows, int& numberOfCols, int& maxGrayVal, std::stringstream& strStream)
{
    std::string line;
    getline(inputFile,line);
    std::cout << "Version of PGM Image: " << line << "\n";
    strStream << inputFile.rdbuf();
    strStream >> numberOfCols >> numberOfRows >> maxGrayVal;
    std::cout << "Rows present: " << numberOfRows;
    std::cout <<  " Columns present: " << numberOfCols << "\n";
    std::cout << "Maximum gray value: " << maxGrayVal << "\n";
    std::cout << "Reading image successful. \n"; 
}

void set_value_array(int **array, int& rows, int& columns, std::stringstream& strStream, int& pad)
{
    int h,w;
    for ( h=0; h<rows; h++ )
        for( w=0;  w<columns; w++ )
        {
            if(h<(rows-pad)&&w<(columns-pad))
                strStream >> array[h][w];
            else
                array[h][w] = 0;         
        }
    std::cout << "Conversion of values to array complete. \n";
}

void write_to_image(int **array, int& rows, int& cols, int& greyvalue, int& pad, std::string& fileName)
{
    std::cout << "Writing resultant array to image in PGM format... \n";
    std::ofstream outputFile(fileName);
    outputFile << "P2\n" << rows-pad << " " << cols-pad << "\n";
    outputFile << greyvalue << "\n";
    for ( int i=0; i<rows-pad; i++ )
    {
        for ( int j=0; j<cols-pad; j++ )
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
                gaussianBlur[i][j] = 0.0625 * 2;
                sharpen[i][j] = -1;
            }
            else
            {
                gaussianBlur[i][j] = 0.0625 * 1;
                sharpen[i][j] = 0;
            }
            if (i==1&&j==1)
            {
                gaussianBlur[i][j] = 0.0625 * 4;
                sharpen[i][j] = 5;
                edgeDetection[i][j] = 8;
            }
        }
    }
    std::cout << "Kernel matrices set. \n";
}

template <typename kernel>
void apply_convolution( int **valueArray,
                        kernel **operation,
                        int& rows,
                        int& cols)
{
    int s = 3;
    for(int x=0; x<rows; x++)
    {
        for(int y=0; y<cols; y++)
        {
            int acc = 0;
            for(int i=0; i<s; i++)
            {
                for(int j=0; j<s; j++)
                {
                    acc+= valueArray[(x-i+2)%rows][(y-j+2)%cols]*operation[i][j];
                }
            }
            if (acc>=0 && acc<=255)
                valueArray[x][y] = acc;

            else if (acc<0)
                valueArray[x][y] = 0;
            
            else
                valueArray[x][y] = 255;
        }
    }
}

int main()
{
    int numberOfRows = 0, numberOfCols = 0, maxGrayVal = 0, pad = 5, blurInstances = 5;
    std::string fileName;
    std::stringstream strStream;
    fileName = "512.pgm";
    std::ifstream inputFile(fileName);
    get_file_data(inputFile, numberOfRows, numberOfCols, maxGrayVal, strStream);
    numberOfCols += pad; 
    numberOfRows += pad;

    int **valueArray;
    valueArray = new int *[numberOfRows];    
    for (int i=0; i<numberOfCols; i++)
        valueArray[i] = new int[numberOfCols];
    
    set_value_array(valueArray, numberOfRows, numberOfRows, strStream, pad);

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

    // apply_convolution<int>(valueArray, edgeDetection, numberOfRows, numberOfCols);
    // fileName = "result01-edgedetection.pgm";
    // write_to_image(valueArray, numberOfRows, numberOfCols, maxGrayVal, pad, fileName);

    // while (blurInstances > 0)
    // {
    //     apply_convolution<double>(valueArray, gaussianBlur, numberOfRows, numberOfCols);
    //     blurInstances--;
    // }
    // fileName = "result02-blur.pgm";
    // write_to_image(valueArray, numberOfRows, numberOfCols, maxGrayVal, pad, fileName);

    for (int i = blurInstances; i > 0; i--)
    {
        apply_convolution<double>(valueArray, gaussianBlur, numberOfRows, numberOfCols);
    }
    apply_convolution<int>(valueArray, sharpen, numberOfRows, numberOfCols);
    apply_convolution<int>(valueArray, edgeDetection, numberOfRows, numberOfCols);
    fileName = "result03-all.pgm";
    write_to_image(valueArray, numberOfRows, numberOfCols, maxGrayVal, pad, fileName);

    std::cout<<"Convolution application complete. \n";


    for(int row = 0; row < numberOfRows; row++)
    {
        delete[] valueArray[row];
    }
	delete[] valueArray;
    for(int k=0; k<size; k++)
    {
        delete[] edgeDetection[k], gaussianBlur[k], sharpen[k];
    }
    delete[] edgeDetection, sharpen, gaussianBlur;
    inputFile.close();
    return 0;
}