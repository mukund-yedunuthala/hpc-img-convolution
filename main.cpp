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
    int h,w;
    for ( h=0; h<rows; h++ )
        for( w=0;  w<columns; w++ )
        {
            if(h<(rows)&&w<(columns))
                strStream >> array[(h*columns)+w];
            else
                array[(h*columns)+w] = 0;         
        }
}

void set_result_array(int *array, int& rows, int& cols)
{
    int h,w;
    for ( h=0; h<rows; h++ )
    {
        for( w=0;  w<cols; w++ )
        {
            array[(h*cols)+w] = 0;         
        }
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
                    acc += valueArray[(((x-i+2)%rows)*cols)+((y-j+2)%cols)]*operation[i][j];
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
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &SIZE);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // std::cout << "Rank = " << rank << ", Size = " << SIZE << "\n";
    MPI_Barrier(MPI_COMM_WORLD);

    int numberOfRows = 0, numberOfCols = 0, maxGrayVal = 0, blurInstances = 5, matSize = 3;
    std::string fileName, opFile;
    std::stringstream strStream;
    fileName = "test.pgm";
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
        std::string separator(60,'=');
        std::cout << separator << "\n\n";
        std::cout << "Version of PGM Image: P2\n";
        std::cout << "Rows present: " << numberOfRows;
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
    int *recvBuff, *sendCounts, *disps, *megaResArray;
    int remRows = numberOfRows%SIZE, sum = 0;
    int buffCnt = (rowsToSend+remRows)*numberOfCols;
    recvBuff = new int[buffCnt];
    sendCounts = new int[SIZE];
    disps = new int[SIZE];
    for(int r = 0; r < SIZE; r++)
    {
        sendCounts[r] = buffCnt;
        if(r==SIZE-1)
        {
            sendCounts[r] += remRows * numberOfCols;
        }
        disps[r] = sum;
        sum += sendCounts[r];
    }
    MPI_Scatterv(valueArray, sendCounts, disps, MPI_INT, recvBuff, buffCnt, MPI_INT, ROOT, MPI_COMM_WORLD);

    // MPI_Scatter(    valueArray, buffCnt, MPI_INT,
    //                 recvBuff, buffCnt, MPI_INT,
    //                 ROOT, MPI_COMM_WORLD    );
    // apply_convolution<int>(recvBuff, recvBuff, edgeDetection, rowsToSend, numberOfCols);
    // MPI_Gather(     recvBuff, buffCnt, MPI_INT,
    //                 resultArray, buffCnt, MPI_INT,
    //                 ROOT, MPI_COMM_WORLD    );
    // if(rank == ROOT)
    // {
    //     opFile = "mpitest-" + fileName;
    //     write_to_image(resultArray, numberOfRows, numberOfCols, maxGrayVal, opFile);
    // }

	delete[] valueArray, resultArray, recvBuff, sendCounts, disps, megaResArray;
    for(int k=0; k < matSize; k++)
    {
        delete[] edgeDetection[k], gaussianBlur[k], sharpen[k];
    }
    delete[] edgeDetection, sharpen, gaussianBlur;
    MPI_Finalize();
    return 0;
}
