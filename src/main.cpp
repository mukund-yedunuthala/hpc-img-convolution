#include "../include/io.h"
#include "../include/helper.h"
#include "../include/kernels.h"

#include "HPC2020Config.h"
#ifdef WITH_MPI
    #include "../include/mpi_convolutions.h"
#else
    #include <time.h>
    #include "../include/seq_convolutions.h"
#endif

int main(int argc, char** argv) {

    // Initial values
    int rank{}, WORLD_SIZE{}, ROOT{0};
    unsigned int numRows{}, numCols{};
    unsigned short int maxVal{};
    unsigned short int matSize{3};
    unsigned short int blurInstances{5}, edgeInstances{1}, sharpInstances{1};
    std::string ipFileName{""}, opFileName{""};
    std::string ipFilePrefix{"512"};
    ipFileName = "../inputs/" + ipFilePrefix + ".pgm";

    // Read from file
    std::stringstream strStream;
    std::ifstream ipFile{ipFileName};
    get_file_data(ipFile, numRows, numCols, maxVal, strStream);

    // Set values related to file arrays
    unsigned long int arrSize{numCols * numRows};
    int defaultValue{};
    
    // Define file arrays
    int *inputArray {new int[arrSize]};
    int *valueArray {new int[arrSize]};
    int *resultArray {new int[arrSize]};
    set_input(inputArray, arrSize, strStream);
    ipFile.close();
    
    // Setting convolution kernels
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
    set_kernels(edgeDetection, identity, sharpen, gaussBlur);
    Kernels kernels;
    kernels.setMatSize(matSize);
    kernels.setInstances(edgeInstances, blurInstances, sharpInstances);
    kernels.setIdentity(identity);
    kernels.setEdgeDetection(edgeDetection);
    kernels.setSharpen(sharpen);
    kernels.setGBlur(gaussBlur);
    // Convolution
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
    opFileName = "../output/" + ipFilePrefix + ".pgm";
#ifdef WITH_MPI
    setup_convolution_new(kernels, numRows, numCols, valueArray, resultArray);    
    if (rank == ROOT) { write_to_file(resultArray, numRows, numCols, maxVal, opFileName); }
#else
    seq_convolution(kernels, numRows, numCols, valueArray, resultArray);
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

    // Destroy convolution kernels
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
    delete[] kernels.instancesArray;
    delete[] edgeDetection, identity, sharpen, gaussBlur;
    edgeDetection = nullptr;
    identity = nullptr;
    sharpen = nullptr;
    gaussBlur = nullptr;
    kernels.instancesArray = nullptr;

    // Destroy file arrays
    delete[] valueArray;
    valueArray = nullptr;
    delete[] resultArray;
    resultArray = nullptr;
    delete[] inputArray;
    inputArray = nullptr;
}