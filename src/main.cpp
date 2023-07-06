/**
 * \file
 * \brief Main project file
 * \author Venkata Mukund Kashyap Yedunuthala
 * \date 15 March 2023
 */ 

#include "io.h"
#include "helper.h"
#include "HPC2020Config.h"
#ifdef WITH_MPI
    #include "mpi_convolutions.h"
#else
    #include <time.h>
    #include "convolutions.h"
#endif
/**
 * \brief Main function.
 * 
 * Reads a given input PGM file and performs convolution using given 
 * kernels and writes output to indicated file. Outputs 3 files, one 
 * for each convolution kernel. Blur is performed 5 times, whereas the
 * other two are limited to one instance. Supports parallel processing
 * using MPI. Input array is divided into chunks if more than 1 process
 * exist in the swarm. File and terminal outputs occur on root process.
*/
int main(int argc, char** argv) {
    
    int rank{}, WORLD_SIZE{}, ROOT{0};
    unsigned int numRows{}, numCols{};
    unsigned short int maxVal{};
    unsigned short int matSize{3};
    unsigned short int blurInstances{5}, edgeInstances{1}, sharpInstances{1};
    std::string ipFileName{""}, opFileName{""};
    ipFileName = "../inputs/512.pgm";

    std::stringstream strStream;
    std::ifstream ipFile{ipFileName};

    get_file_data(ipFile, numRows, numCols, maxVal, strStream);
    
    unsigned long int arrSize{numCols * numRows};
    int defaultValue{};

    int *inputArray {new int[arrSize]};
    int *valueArray {new int[arrSize]};
    int *resultArray {new int[arrSize]};

    set_input(inputArray, arrSize, strStream);
    

    ipFile.close();
    
    // setting kernels 

    int **edgeDetection{new int*[matSize]};
    int **sharpen{new int*[matSize]};
    int **identity{new int*[matSize]};
    double **gaussBlur{new double*[matSize]};
    for (size_t i = 0; i < matSize; i++){
        edgeDetection[i] = new int[matSize];
        sharpen[i] = new int[matSize];
        identity[i] = new int[matSize];
        gaussBlur[i] = new double[matSize];
    }
    // std::cout << "Edge address (out): " << &edgeDetection << std::endl;
    set_kernels(edgeDetection, identity, sharpen, gaussBlur);
#ifdef WITH_MPI
    double wallTime{};
    MPI_Init(&argc, &argv);
    wallTime = MPI_Wtime();
    MPI_Comm_size(MPI_COMM_WORLD, &WORLD_SIZE);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#else
    clock_t wallTime = clock();
#endif

    set_array(valueArray, arrSize, inputArray);
    set_array(resultArray, arrSize, defaultValue);
    opFileName = "../output/512edge.pgm";
#ifdef WITH_MPI
    setup_convolution<int>(
            numRows, numCols, valueArray, edgeDetection,
            resultArray, matSize, edgeInstances 
        );
    if (rank == ROOT) { write_to_file(resultArray, numRows, numCols, maxVal, opFileName); }
#else
    while (edgeInstances > 0) {
        convolution<int>(
            valueArray, resultArray, edgeDetection, 
            numRows, numCols, matSize
        );
        edgeInstances--;
    }
    write_to_file(resultArray, numRows, numCols, maxVal, opFileName);
#endif
    
    set_array(valueArray, arrSize, inputArray);
    set_array(resultArray, arrSize, defaultValue);
    opFileName = "../output/512sharpen.pgm";
#ifdef WITH_MPI
    setup_convolution<int>(
            numRows, numCols, valueArray, sharpen,
            resultArray, matSize, sharpInstances 
        );
    if (rank == ROOT) { write_to_file(resultArray, numRows, numCols, maxVal, opFileName); }
#else
    while (sharpInstances > 0) {
        convolution<int>(
            valueArray, resultArray, sharpen, 
            numRows, numCols, matSize
        );
        sharpInstances--;
    }
    write_to_file(resultArray, numRows, numCols, maxVal, opFileName);
#endif

    set_array(valueArray, arrSize, inputArray);
    set_array(resultArray, arrSize, defaultValue);
    opFileName = "../output/512blur.pgm";
#ifdef WITH_MPI
    setup_convolution<double>(
            numRows, numCols, valueArray, gaussBlur,
            resultArray, matSize, blurInstances 
        );
    if (rank == ROOT) { write_to_file(resultArray, numRows, numCols, maxVal, opFileName); }
#else
    while (blurInstances > 0) {  
        convolution<double>(
            valueArray, resultArray, gaussBlur, 
            numRows, numCols, matSize
        );
        blurInstances--;
    }
    write_to_file(resultArray, numRows, numCols, maxVal, opFileName);
#endif
    
#ifdef WITH_MPI
    if (rank == ROOT) {
        printinfo(numRows, numCols, maxVal);
        wallTime = MPI_Wtime() - wallTime;
        print_time_info(wallTime);
    }
    MPI_Finalize();
#else
    wallTime = clock() - wallTime;
    printinfo(numRows, numCols, maxVal);
    double elapsed = (double) wallTime/CLOCKS_PER_SEC;
    print_time_info(elapsed);
#endif
    for (size_t i = 0; i < matSize; i++){
        delete[] edgeDetection[i];
        delete[] identity[i];
        delete[] sharpen[i];
        delete[] gaussBlur[i];
        edgeDetection[i] = nullptr;
        identity[i] = nullptr;
        sharpen[i] = nullptr;
        gaussBlur[i] = nullptr;
    }
    delete[] edgeDetection, identity, sharpen, gaussBlur;
    edgeDetection = nullptr;
    identity = nullptr;
    sharpen = nullptr;
    gaussBlur = nullptr;
    
    delete[] valueArray;
    valueArray = nullptr;
    delete[] resultArray;
    resultArray = nullptr;
    delete[] inputArray;
    inputArray = nullptr;

    return 0;
}