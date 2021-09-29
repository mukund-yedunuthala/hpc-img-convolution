/**
 * \file
 * \brief File for image convolution using MPI
 * \author Venkata Mukund Kashyap Yedunuthala
 * \date 14 September 2021
 */ 

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <mpi.h>

/**
 * \brief Reads data from specified ```.pgm``` file
 * \param inputFile The ifstream variable of the file
 * \param numberOfRows Output - The number of rows in image from the file.
 * \param numberOfCols Output - The number of columns in image from the file. 
 * \param maxGrayVal Output - The maximum possible value for a 
 * pixel in the image. 
 * \param strStream The string stream variable that aids in reading data in.
 */

void get_file_data(std::ifstream& inputFile, int& numberOfRows, 
        int& numberOfCols, int& maxGrayVal, std::stringstream& strStream)
{
    std::string line;
    getline(inputFile,line);
    strStream << inputFile.rdbuf();
    strStream >> numberOfCols >> numberOfRows >> maxGrayVal;
}

/**
 * \brief Reads the values at each pixel and assigns them to a one-dimensional array.
 * \param array One-dimensional array that will contain all pixel values.
 * \param rows Input - The number of rows in image, 
 * read from the file using ```get_file_data()```.
 * \param columns Input - The number of columns in image, read 
 * from the file using ```get_file_data()```. 
 * \param strStream The string stream variable that aids in reading data in, 
 * initialised using ```get_file_data()```.
 */

void set_value_array(int *array, int& rows, 
    int& columns, std::stringstream& strStream)
{
    int k = 0;
    for (k; k<rows*columns; k++)
    {
        strStream >> array[k];
    }
}

/**
 * \brief Initializes the values of an array to zero.
 * \param array One-dimensional array that will contain all pixel values.
 * \param rows Input - The number of rows in image, read from 
 * the file using ```get_file_data()```.
 * \param cols Input - The number of columns in image, read from 
 * the file using ```get_file_data()```. 
 */

void set_result_array(int *array, int& rows, int& cols)
{
    
    int k = 0;
    for (k; k<rows*cols; k++)
    {
        array[k] = 0;
    }
}

/**
 * \brief Writes the values present in an array to a file of ```.pgm``` format.
 * \param array Input - array from which the file is to be written.
 * \param rows Input - The number of rows in image, read from
 *  the input file using ```get_file_data()```.
 * \param cols Input - The number of columns in image, read from 
 * the input file using ```get_file_data()```. 
 * \param greyvalue Input - The maximum possible value for a pixel in the image,
 *  read from the input file using ```get_file_data()```. 
 * \param fileName Input - Name of the image file to be written.
 */

void write_to_image(int *array, int& rows, 
    int& cols, int& greyvalue, std::string& fileName)
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

/**
 * \brief Assigns values to image convolution kernels.
 * \param edgeDetection Array to store values of edge detection kernel.
 * \param sharpen Array to store values of image sharpening kernel.
 * \param gaussianBlur Array to store values of blurring kernel.
 * \param identity Array to store values of identity kernel.
 * \param s Size of the kernels, here 3.
 */
void set_kernels(int **edgeDetection, int **sharpen, 
    double **gaussianBlur, int **identity, int& s)
{
    for ( int i=0; i<3; i++)
    {
        for( int j=0; j<3; j++)
        {
            edgeDetection[i][j] = -1;
            identity[i][j] = 0;
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
                identity[i][j] = 1;
            }
        }
    }
}

/**
 * \brief Template for convolution kernels that allows usage of 
 * type-appropriate variables.
 */
template <typename kernel>
struct convolutions 
{
    /**
     * \brief Function for convolution
     * \param valueArray Source array upon which convolution is performed.
     * \param resultArray Resultant array after convolution
     * \param operation Convolution operation. Available: edgeDetection,
     *  gaussianBlur, sharpen, identity.
     * \param rows Number of rows in the chunk.
     * \param cols Number of columns in the chunk.
     */
    static void apply_convolution( kernel *valueArray, kernel *resultArray,
            kernel **operation, int& rows, int& cols)
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
                        acc += valueArray[((x+2-i)%rows)*cols+((y+2-j)%cols)] \
                        * operation[i][j];
                    }
                }
                // eliminating undesired values by hard coding limits
                if (acc>=0 && acc<=255)
                    resultArray[(x*cols)+y] = acc;

                else if (acc<0)
                    resultArray[(x*cols)+y] = 0;
            
                else
                    resultArray[(x*cols)+y] = 255;
            }
        }
    }

    /**
     * \brief Function to set up convolutions using MPI. Takes input array
     * and scatters the data to available processes in the specified 
     * communicator. Also communicates halo information to appropriate 
     * processes.
     * \param numRows Number of rows present in the input array.
     * \param numCols Number of columns present in the input array.
     * \param valueArray The input array.
     * \param convKernel Kernel for appropriate convolution, as defined 
     * in ```set_kernels()```.
     * \param resultArray The resultant array that will be written to image.
     * \param INSTANCES Number of times the convolution operation is to be 
     * performed. 
     */

    static void mpi_convolution(int numRows, int numCols, int *valueArray, 
                    kernel **convKernel, int *resultArray, int INSTANCES = 1)
    {
        int SIZE, rank, ROOT = 0;
        MPI_Comm_size(MPI_COMM_WORLD, &SIZE);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        int rowsToSend = (numRows/SIZE); // divides the available rows into chunks
        int buffCount = rowsToSend*numCols; // chunk size per process 
        int sender1, sender2, receiver1, receiver2; // ranks for halo exchange

        // initializing arrays for MPI communication

        int *recvBuff, *sendHalo1, *sendHalo2, *recvHalo1, *recvHalo2; 
        recvBuff = new int[buffCount];
        sendHalo1 = new int[numCols];
        sendHalo2 = new int[numCols];
        recvHalo1 = new int[numCols];
        recvHalo2 = new int[numCols];

        // initializing arrays upon which convolution is performed

        int convRows = rowsToSend+2;
        int convCount = convRows*numCols;
        kernel *convBuff;
        convBuff = new kernel[convCount];

        // scattering to recvBuff

        MPI_Scatter(valueArray, buffCount, MPI_INT,
            recvBuff, buffCount, MPI_INT, ROOT, MPI_COMM_WORLD);

        // Halo communication - initializing rows to be sent

        for(int i = 0; i < numCols; i++)
        {
            sendHalo1[i] = recvBuff[i];
            sendHalo2[i] = recvBuff[i+((rowsToSend-1)*numCols)];
        }

        // setting up senders and receivers

        if (SIZE == 1)
        {
            receiver1 = 0;
            receiver2 = 0;
            sender1 = 0;
            sender2 = 0;
        }
        else
        {
            if (rank == 0)
            {
                receiver1 = SIZE - 1;
                receiver2 = rank + 1;
                sender1 = SIZE - 1;
                sender2 = rank + 1;
            }
            else if (rank == SIZE - 1)
            {
                receiver1 = rank - 1;
                receiver2 = 0;
                sender1 = rank - 1;
                sender2 = 0;
            }
            else
            {
                receiver1 = rank - 1;
                receiver2 = rank + 1;
                sender1 = rank - 1;
                sender2 = rank + 1;
            }
        }

        // MPI instant send and receive operations 
        // for non-blocking communication

        MPI_Request req[2];
        MPI_Isend(sendHalo1, numCols, MPI_INT, 
            receiver1, 1, MPI_COMM_WORLD, &req[0]);
        MPI_Isend(sendHalo2, numCols, MPI_INT, 
            receiver2, 2, MPI_COMM_WORLD, &req[1]);
        MPI_Irecv(recvHalo1, numCols, MPI_INT, 
            sender1, 2, MPI_COMM_WORLD, &req[1]);
        MPI_Irecv(recvHalo2, numCols, MPI_INT, 
            sender2, 1, MPI_COMM_WORLD, &req[0]);
        MPI_Waitall(2, req, MPI_STATUSES_IGNORE);

        // setting up convolution array with received halo arrays

        for (int i = 0; i < convCount; i++)
        {
            if (i < numCols)
            {
                convBuff[i] = recvHalo1[i];
                convBuff[buffCount+numCols+i] = recvHalo2[i];
            }
            else if (i < buffCount + numCols)
            {
                convBuff[i] = recvBuff[i-numCols];
            }
        }

        while (INSTANCES>0)
        {
            apply_convolution(convBuff, convBuff, 
            convKernel, convRows, numCols);
            INSTANCES--;
        }
        
        for (int i = 0; i < buffCount; i++)
        {   
            recvBuff[i] = convBuff[i];
        }

        // gathering into resultArray after convolution

        MPI_Gather(recvBuff, buffCount, MPI_INT,
            resultArray, buffCount, MPI_INT, ROOT, MPI_COMM_WORLD);
    
        // freeing memory allocated

        delete[] recvBuff, convBuff;
        delete[] sendHalo1, sendHalo2, recvHalo1, recvHalo2;

    }
};


int main(int argc, char** argv)
{   
/**
 * \brief The main function. Reads ```.pgm``` format image in, 
 * sets up convolutions and performs them using MPI. 
 * Finally writes the image to memory in ```.pgm``` format.
 * Prints the time taken to shell. 
 * \param argc Number of arguments.
 * \param argv Arguments.
 * \remark File performs the following four tasks:
 * \remark Task 01: Convolution with edge detection kernel - one instance
 * \remark Task 02: Convolution with blur kernel - one instance
 * \remark Task 03: Convolution with sharpen kernel - one instance
 * \remark Task 04: Convolution with blur - five instances, 
 * then edge detection, and sharpen - one instance each. 
 * \return EXIT_SUCCESS or EXIT_FAILURE depending on the situation.
 */
    int rank, SIZE, ROOT = 0;
    double time;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &SIZE);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    time = MPI_Wtime(); // recording time at the beginning 
    int numberOfRows = 0, numberOfCols = 0, maxGrayVal = 0, \
    blurInstances = 5, matSize = 3;

    // reading from file 

    std::string fileName, opFile;
    std::stringstream strStream;
    fileName = "512.pgm";
    std::ifstream inputFile(fileName);
    get_file_data(inputFile, numberOfRows, numberOfCols, 
        maxGrayVal, strStream);
    
    int arrSize = numberOfCols*numberOfRows;
    int *valueArray, *resultArray;
    valueArray = new int [arrSize];
    resultArray = new int [arrSize];    

    // reading data into arrays only on ROOT process
    if(rank == ROOT) set_value_array(valueArray, numberOfRows, numberOfRows, strStream);
    set_result_array(resultArray, numberOfRows, numberOfCols);
    inputFile.close();
    
    if(rank == ROOT)
    {
        std::cout << "Image convolution using MPI\n";
        std::cout << "High Performance Computing and Optimization";
        std::cout << "| WS2020-21\n";
        std::cout << "Technische Universitaet Bergakademie Freiberg\n";
        std::string separator(75,'=');
        std::cout << separator << "\n";
        std::cout << "Version of PGM Image: P2\n";
        std::cout << "Rows present: " <<numberOfRows;
        std::cout <<  " Columns present: " << numberOfCols << "\n";
        std::cout << "Maximum gray value: " << maxGrayVal << "\n";
    }

    // allocating space for convolution kernels and initializing
    
    int **edgeDetection, **sharpen, **identity;
    double **gaussianBlur;
    edgeDetection = new int *[matSize];
    sharpen = new int *[matSize];
    gaussianBlur = new double *[matSize];
    identity = new int *[matSize];
    for(int j=0; j<matSize; j++)
    {
        edgeDetection[j] = new int[matSize];
        sharpen[j] = new int[matSize];
        gaussianBlur[j] = new double[matSize];
        identity[j] = new int[matSize];
    }
    set_kernels(edgeDetection, sharpen, gaussianBlur, identity, matSize);
    
    convolutions<int>::mpi_convolution(numberOfRows, numberOfCols,
        valueArray, edgeDetection, resultArray, 1);
    if(rank == ROOT)
    {
        opFile = "output/mpi-edge-halo-" + fileName;
        write_to_image(resultArray, numberOfRows, 
            numberOfCols, maxGrayVal, opFile);
    }

    set_result_array(resultArray, numberOfRows, numberOfCols);
    convolutions<double>::mpi_convolution(numberOfRows, numberOfCols,
        valueArray, gaussianBlur, resultArray, 1);
    if(rank == ROOT)
    {
        opFile = "output/mpi-blur-halo-" + fileName;
        write_to_image(resultArray, numberOfRows, 
            numberOfCols, maxGrayVal, opFile);
    }

    set_result_array(resultArray, numberOfRows, numberOfCols);
    convolutions<int>::mpi_convolution(numberOfRows, numberOfCols,
        valueArray, sharpen, resultArray, 1);
    if(rank == ROOT)
    {
        opFile = "output/mpi-sharpen-halo-" + fileName;
        write_to_image(resultArray, numberOfRows, 
            numberOfCols, maxGrayVal, opFile);
    }
    
    set_result_array(resultArray, numberOfRows, numberOfCols);
    convolutions<double>::mpi_convolution(numberOfRows, numberOfCols,
        valueArray, gaussianBlur, resultArray, 5);
    convolutions<int>::mpi_convolution(numberOfRows, numberOfCols,
        resultArray, edgeDetection, resultArray, 1);
    convolutions<int>::mpi_convolution(numberOfRows, numberOfCols,
        resultArray, sharpen, resultArray, 1);

    // ROOT rank writes the resultant array to file in .pgm format
    if(rank == ROOT)
    {
        opFile = "output/mpi-all-halo-" + fileName;
        write_to_image(resultArray, numberOfRows, 
            numberOfCols, maxGrayVal, opFile);
    }

    // freeing up allocated memory
	delete[] valueArray, resultArray;
    for(int k=0; k < matSize; k++)
    {
        delete[] edgeDetection[k], gaussianBlur[k], sharpen[k], identity[k];
    }
    delete[] edgeDetection, sharpen, gaussianBlur, identity;
    time = MPI_Wtime() - time; // recording total time taken
    if(rank==ROOT){
        std::string separator(75,'-');
        std::cout << separator << "\n" << "Time taken: " \
        << time << "\n" << separator << "\n";
    }

    MPI_Finalize();
    return 0;
}