/**
 * \file
 * \brief Definitions for Kernels struct members.
 * \author Venkata Mukund Kashyap Yedunuthala
 * \date 23 January 2024
 */ 
#include "../include/kernels.h"

Kernels::Kernels() : identity(nullptr), edgeDetection(nullptr),
    sharpen(nullptr), gaussBlur(nullptr), instancesArray(nullptr), matSize(0) {}

void Kernels::setIdentity(int **identityNew) {
    identity = identityNew;
}

void Kernels::setEdgeDetection(int **edgeDetectionNew) {
    edgeDetection = edgeDetectionNew;
}

void Kernels::setSharpen(int **sharpenNew) {
    sharpen = sharpenNew;
}

void Kernels::setGBlur(double **gaussBlurNew) {
    gaussBlur = gaussBlurNew;
}

void Kernels::setMatSize(unsigned short matSizeNew) {
    matSize = static_cast<unsigned short>(matSizeNew);
}

void Kernels::setInstances(unsigned short edge, unsigned short blur, unsigned short sharp) {
    instancesArray = new unsigned short[3];
    instancesArray[0] = static_cast<unsigned short>(edge);
    instancesArray[1] = static_cast<unsigned short>(blur);
    instancesArray[2] = static_cast<unsigned short>(sharp);
}

