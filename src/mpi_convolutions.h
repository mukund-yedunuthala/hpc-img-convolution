#include <mpi.h>
#include "convolutions.h"
/**
 * \brief Sets up convolution of a given array with a given kernel for MPI. 
 * 
 * Performs MPI scatter and gather operations if more than 1 process is 
 * present in the swarm. Scatter and gather operations are null-safe and 
 * memory-safe. Additional information regarding halo, i.e. rows present 
 * above and below a portion of input array are also communicated. 
 * \param valueArray Input array.
 * \param resultArray Output array.
 * \param convolutionKernel Convolution kernel.
 * \param rows Number of rows present in input array.
 * \param cols Number of columns present in input array.
 * \param size Size of the convolution kernel array.
 * \param instances Number of convolution instances.
*/
template <typename kernel>
void setup_convolution(
    unsigned int& rows,
    unsigned int& cols,
    int*& valueArray,
    kernel**& convolutionKernel,
    int*& resultArray,
    unsigned short int& size,
    unsigned short int& instances = 1
) {
    int WORLD_SIZE{}, rank{}, ROOT{};
    MPI_Comm_size(MPI_COMM_WORLD, &WORLD_SIZE);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(WORLD_SIZE == 1) {
        convolution<kernel>(
            valueArray, resultArray, convolutionKernel, 
            rows, cols, size, instances
        );
    }

    else {
        unsigned int rowsToSend {rows/WORLD_SIZE};
        int bufferSize {rowsToSend * cols};
        unsigned short int sender_1{}, sender_2{};
        unsigned short int receiver_1{}, receiver_2{};

        int* receiverBuffer {new int[bufferSize]};
        int* senderHalo_1 {new int[cols]};
        int* senderHalo_2 {new int[cols]};
        int* receiverHalo_1 {new int[cols]};
        int* receiverHalo_2 {new int[cols]};

        unsigned int convRows{rowsToSend + 2};
        int convSize{convRows * cols};
        int* convolutionBuffer{new int[convSize]{}};
        int* convolutionResult{new int[convSize]};

        MPI_Scatter(
            valueArray, bufferSize, MPI_INT, 
            receiverBuffer, bufferSize, MPI_INT, 
            ROOT, MPI_COMM_WORLD
        );

        for (size_t i = 0; i < cols; i++) {
            senderHalo_1[i] = receiverBuffer[i];
            senderHalo_2[i] = receiverBuffer[i + ((rowsToSend-1) * cols)];
        }

        if (WORLD_SIZE != 1) {
            if (rank == 0) {
                receiver_1 = WORLD_SIZE - 1;
                receiver_2 = rank + 1;
                sender_1 = WORLD_SIZE - 1;
                sender_2 = rank + 1;
            }
            else if (rank == (WORLD_SIZE - 1)) {
                receiver_1 = rank - 1;
                receiver_2 = 0;
                sender_1 = rank - 1;
                sender_2 = 0;
            }
            else {
                receiver_1 = rank - 1;
                receiver_2 = rank + 1;
                sender_1 = rank - 1;
                sender_2 = rank + 1;
            }
        }

        MPI_Request requests[2]{};
        MPI_Isend(
            senderHalo_1, cols, MPI_INT,
            receiver_1, 1, MPI_COMM_WORLD, &requests[0]
        );
        MPI_Isend(
            senderHalo_2, cols, MPI_INT,
            receiver_2, 2, MPI_COMM_WORLD, &requests[1]
        );
        MPI_Irecv(
            receiverHalo_1, cols, MPI_INT,
            sender_1, 2, MPI_COMM_WORLD, &requests[1]
        );
        MPI_Irecv(
            receiverHalo_2, cols, MPI_INT,
            sender_2, 1, MPI_COMM_WORLD, &requests[0]
        );
        MPI_Waitall(2, requests, MPI_STATUS_IGNORE);
        for (size_t i = 0; i < convSize; i++)
        {
            if (i < cols) {
               convolutionBuffer[i] = receiverHalo_1[i];
               convolutionBuffer[bufferSize + cols + i] = receiverHalo_2[i];
            }
            else if (i < (bufferSize + cols)) {
                convolutionBuffer[i] = receiverBuffer[i - cols];
            }
        }

        convolution<kernel>(
            convolutionBuffer, convolutionResult, 
            convolutionKernel, convRows, cols, size, instances
        );

        for (size_t i = 0; i < bufferSize; i++) {
            receiverBuffer[i] = convolutionResult[i+cols];
        }

        MPI_Gather(
            receiverBuffer, bufferSize, MPI_INT,
            resultArray, bufferSize, MPI_INT, ROOT, MPI_COMM_WORLD
        );

        delete[] receiverBuffer;
        receiverBuffer = nullptr;
        delete[] convolutionBuffer;
        convolutionBuffer = nullptr;
        delete[] convolutionResult;
        convolutionResult = nullptr;
        delete[] senderHalo_1, senderHalo_2, receiverHalo_1, receiverHalo_2;
        senderHalo_1 = nullptr;
        senderHalo_2 = nullptr;
        receiverHalo_1 = nullptr;
        receiverHalo_2 = nullptr;
    }
}