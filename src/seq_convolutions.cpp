#include "../include/seq_convolutions.h"

void seq_convolution(
    Kernels& kernels,
    unsigned int& rows,
    unsigned int& cols,
    int*& valueArray,
    int*& resultArray
) {
    while (kernels.instancesArray[0] > 0 || kernels.instancesArray[1] > 0 || kernels.instancesArray[2] > 0) {
        for (unsigned int x = 0; x < rows; x++) {
            for (unsigned int y = 0; y < cols; y++) {
                int sum {};
                for (size_t i = 0; i < 3; i++) {
                    for (size_t j = 0; j < 3; j++) {
                        if (kernels.instancesArray[0] > 0) {
                            unsigned long int index = (((x+1-i)%rows)*cols)+((y%cols)+1-j);
                            int value = valueArray[index];
                            sum += value * kernels.edgeDetection[i][j];
                        }
                        else if (kernels.instancesArray[1] > 0) {
                            unsigned long int index = (((x+1-i)%rows)*cols)+((y%cols)+1-j);
                            int value = valueArray[index];
                            sum += value * kernels.gaussBlur[i][j];
                        }
                        else if (kernels.instancesArray[2] > 0) {
                            unsigned long int index = (((x+1-i)%rows)*cols)+((y%cols)+1-j);
                            int value = valueArray[index];
                            sum += value * kernels.sharpen[i][j];
                        } 
                    }
                }
            if (sum>=0 && sum<=255)
                resultArray[(x*cols)+y] = sum;
            else if (sum<0)
                resultArray[(x*cols)+y] = 0;
            else
                resultArray[(x*cols)+y] = 255;
            }
        }
        for (unsigned int i = 0; i < (rows * cols); i++) {
            valueArray[i] = resultArray[i];
        }
        if (kernels.instancesArray[0] > 0) {kernels.instancesArray[0]--;}
        if (kernels.instancesArray[1] > 0) {kernels.instancesArray[1]--;}
        if (kernels.instancesArray[2] > 0) {kernels.instancesArray[2]--;}    
    }
}