#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <mpi.h>
void get_file_data(std::ifstream& inputFile, int& numberOfRows, int& numberOfCols, int& maxGrayVal, std::stringstream& strStream)
{
    std::string line;
    getline(inputFile,line);
    strStream << inputFile.rdbuf();
    strStream >> numberOfCols >> numberOfRows >> maxGrayVal;
}

void set_value_array(int *array, int& rows, int& columns, std::stringstream& strStream)
{
    int k = 0;
    for (k; k<rows*columns; k++)
    {
        strStream >> array[k];
    }
}

void set_result_array(int *array, int& rows, int& cols)
{
    
    int k = 0;
    for (k; k<rows*cols; k++)
    {
        array[k] = 0;
    }
}

void write_to_image(int *array, int& rows, int& cols, int& greyvalue, std::string& fileName)
{
    std::ofstream outputFile(fileName);
    outputFile << "P2\n" << rows << " " << cols << "\n";
    outputFile << greyvalue << "\n";
    for ( int i=0; i<rows; i++ )
    {
        for ( int j=0; j<cols; j++ )
        {
            outputFile << array[(i*cols)+j] << " ";
        }
        outputFile << "\n";
    }
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
}

template <typename kernel>
void apply_convolution( int *valueArray,
                        int *resultArray,
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
                    acc += valueArray[(((x+2-i)%rows)*cols)+((y+2-j)%cols)]*operation[i][j];
                }
            }
            if (acc>=0 && acc<=255)
                resultArray[(x*cols)+y] = acc;

            else if (acc<0)
                resultArray[(x*cols)+y] = 0;
            
            else
                resultArray[(x*cols)+y] = 255;
        }
    }
}

int main(int argc, char** argv)
{
    int rank, SIZE, ROOT = 0;
    double time;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &SIZE);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    time = MPI_Wtime();
    int numberOfRows = 0, numberOfCols = 0, maxGrayVal = 0, blurInstances = 5, matSize = 3;
    std::string fileName, opFile;
    std::stringstream strStream;
    fileName = "512.pgm";
    std::ifstream inputFile(fileName);
    get_file_data(inputFile, numberOfRows, numberOfCols, maxGrayVal, strStream);
    int arrSize = numberOfCols*numberOfRows;
    int *valueArray, *resultArray;
    valueArray = new int [arrSize];
    resultArray = new int [arrSize];    
    set_value_array(valueArray, numberOfRows, numberOfRows, strStream);
    set_result_array(resultArray, numberOfRows, numberOfCols);
    inputFile.close();
    if(rank == ROOT)
    {
        std::cout << "Image convolution using MPI\n";
        std::cout << "High Performance Computing and Optimization | WS2020-21\n";
        std::cout << "Technische Universitaet Bergakademie Freiberg\n";
        std::string separator(75,'=');
        std::cout << separator << "\n";
        std::cout << "Version of PGM Image: P2\n";
        std::cout << "Rows present: " <<numberOfRows;
        std::cout <<  " Columns present: " << numberOfCols << "\n";
        std::cout << "Maximum gray value: " << maxGrayVal << "\n";
    }

    int **edgeDetection, **sharpen;
    double **gaussianBlur;
    edgeDetection = new int *[matSize];
    sharpen = new int *[matSize];
    gaussianBlur = new double *[matSize];
    for(int j=0; j<matSize; j++)
    {
        edgeDetection[j] = new int[matSize];
        sharpen[j] = new int[matSize];
        gaussianBlur[j] = new double[matSize];
    }
    set_kernels(edgeDetection, sharpen, gaussianBlur, matSize);
    
    int rowsToSend = (numberOfRows/SIZE);
    int buffCount = rowsToSend*numberOfCols, *recvBuff;    
    recvBuff = new int[buffCount];
    MPI_Scatter(    valueArray, buffCount, MPI_INT,
                    recvBuff, buffCount, MPI_INT,
                    ROOT, MPI_COMM_WORLD    );
    apply_convolution<int>(recvBuff, recvBuff, edgeDetection, rowsToSend, numberOfCols);
    MPI_Gather(     recvBuff, buffCount, MPI_INT,
                    resultArray, buffCount, MPI_INT,
                    ROOT, MPI_COMM_WORLD    );
    if(rank == ROOT)
    {
        opFile = "mpi-edge-detection-" + fileName;
        write_to_image(resultArray, numberOfRows, numberOfCols, maxGrayVal, opFile);
    }
    MPI_Scatter(    valueArray, buffCount, MPI_INT,
                    recvBuff, buffCount, MPI_INT,
                    ROOT, MPI_COMM_WORLD    );
    set_result_array(resultArray, numberOfRows, numberOfCols);
    int index = blurInstances;
    while(index>0)
    {
        apply_convolution<double>(recvBuff, recvBuff, gaussianBlur, rowsToSend, numberOfCols);
        index--;
    }
    MPI_Gather(     recvBuff, buffCount, MPI_INT,
                    resultArray, buffCount, MPI_INT,
                    ROOT, MPI_COMM_WORLD    );
    if(rank == ROOT)
    {
        opFile = "mpi-blur-" + fileName;
        write_to_image(resultArray, numberOfRows, numberOfCols, maxGrayVal, opFile);
    }
    MPI_Scatter(    valueArray, buffCount, MPI_INT,
                    recvBuff, buffCount, MPI_INT,
                    ROOT, MPI_COMM_WORLD    );
    set_result_array(resultArray, numberOfRows, numberOfCols);
    index = blurInstances;
    while(index>0)
    {
        apply_convolution<double>(recvBuff, recvBuff, gaussianBlur, rowsToSend, numberOfCols);
        index--;
    }
    apply_convolution<int>(recvBuff, recvBuff, sharpen, rowsToSend, numberOfCols);
    apply_convolution<int>(recvBuff, recvBuff, edgeDetection, rowsToSend, numberOfCols);
    MPI_Gather(     recvBuff, buffCount, MPI_INT,
                    resultArray, buffCount, MPI_INT,
                    ROOT, MPI_COMM_WORLD    );
    if(rank == ROOT)
    {
        opFile = "mpi-all-" + fileName;
        write_to_image(resultArray, numberOfRows, numberOfCols, maxGrayVal, opFile);
    }

	delete[] valueArray, resultArray, recvBuff;
    for(int k=0; k < matSize; k++)
    {
        delete[] edgeDetection[k], gaussianBlur[k], sharpen[k];
    }
    delete[] edgeDetection, sharpen, gaussianBlur;
    time = MPI_Wtime() - time;
    if(rank==ROOT){
        std::string separator(75,'-');
        std::cout << separator << "\n" << "Time taken: " << time << "\n" << separator << "\n";
    }
    MPI_Finalize();
    return 0;
}
