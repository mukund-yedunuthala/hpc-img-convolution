/**
 * \file
 * \brief Convolutions using MPI, function definition.
 * \author Venkata Mukund Kashyap Yedunuthala
 * \date 23 January 2024
 */ 


#include "../include/mpi_convolutions.h"

void setup_convolution_new(
    Kernels& kernels,
    unsigned int& rows,
    unsigned int& cols,
    int*& valueArray,
    int*& resultArray
) {
    int WORLD_SIZE{}, rank{}, ROOT{};
    MPI_Comm_size(MPI_COMM_WORLD, &WORLD_SIZE);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    unsigned int rowsToSend {rows/WORLD_SIZE};
    unsigned long int bufferSize {rowsToSend * cols};
    unsigned short int sender_1{}, sender_2{};
    unsigned short int receiver_1{}, receiver_2{};
    int* receiverBuffer {new int[bufferSize]};
    int* senderHalo_1 {new int[cols]};
    int* senderHalo_2 {new int[cols]};
    int* receiverHalo_1 {new int[cols]};
    int* receiverHalo_2 {new int[cols]};
    unsigned int convRows{rowsToSend + 2};
    unsigned long int convSize{convRows * cols};
    int* convolutionBuffer{new int[convSize]{}};
    int* convolutionResult{new int[convSize]};
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
    else {
        receiver_1 = ROOT;
        receiver_2 = ROOT;
        sender_1 = ROOT;
        sender_2 = ROOT;
    }
    MPI_Request requests[2]{};
    while (kernels.instancesArray[0] > 0 || kernels.instancesArray[1] > 0 || kernels.instancesArray[2] > 0) {
    MPI_Scatter(
                valueArray, bufferSize, MPI_INT, 
                receiverBuffer, bufferSize, MPI_INT, 
                ROOT, MPI_COMM_WORLD
    );
    for (unsigned int i = 0; i < cols; i++) {
        senderHalo_1[i] = receiverBuffer[i];
        senderHalo_2[i] = receiverBuffer[i + ((rowsToSend-1) * cols)];
    }
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
    for (unsigned int i = 0; i < convSize; i++) {
        if (i < cols) {
           convolutionBuffer[i] = receiverHalo_1[i];
           convolutionBuffer[bufferSize + cols + i] = receiverHalo_2[i];
        }
        else if (i < (bufferSize + cols)) {
            convolutionBuffer[i] = receiverBuffer[i - cols];
        }
    }
    for (unsigned int x = 0; x < convRows; x++) {
       for (unsigned int y = 0; y < cols; y++) {
            int sum {};
            for (size_t i = 0; i < 3; i++) {
                for (size_t j = 0; j < 3; j++) {
                    if (kernels.instancesArray[0] > 0) {
                        unsigned long int index = (((x+1-i)%convRows)*cols)+((y%cols)+1-j);
                        int value = convolutionBuffer[index];
                        sum += value * kernels.edgeDetection[i][j];
                    }
                    else if (kernels.instancesArray[1] > 0) {
                        unsigned long int index = (((x+1-i)%convRows)*cols)+((y%cols)+1-j);
                        int value = convolutionBuffer[index];
                        sum += value * kernels.gaussBlur[i][j];
                    }
                    else if (kernels.instancesArray[2] > 0) {
                        unsigned long int index = (((x+1-i)%convRows)*cols)+((y%cols)+1-j);
                        int value = convolutionBuffer[index];
                        sum += value * kernels.sharpen[i][j];
                    } 
                }
            }
            if (sum>=0 && sum<=255)
                convolutionResult[(x*cols)+y] = sum;
            else if (sum<0)
                convolutionResult[(x*cols)+y] = 0;
            else
                convolutionResult[(x*cols)+y] = 255;
        }
    }
    for (unsigned int i = 0; i < bufferSize; i++) {
        receiverBuffer[i] = convolutionResult[i+cols];
    }
    MPI_Gather(
        receiverBuffer, bufferSize, MPI_INT,
        resultArray, bufferSize, MPI_INT, ROOT, MPI_COMM_WORLD
    );
    for (unsigned int i = 0; i < (rows * cols); i++) {
        valueArray[i] = resultArray[i];
    }
    if (kernels.instancesArray[0] > 0) {kernels.instancesArray[0]--;}
    if (kernels.instancesArray[1] > 0) {kernels.instancesArray[1]--;}
    if (kernels.instancesArray[2] > 0) {kernels.instancesArray[2]--;}
    }
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