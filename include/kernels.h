#ifndef KERNELS_H
#define KERNELS_H
struct Kernels {
    int **identity;
    int **edgeDetection;
    int **sharpen;
    double **gaussBlur;
    unsigned short* instancesArray;
    unsigned short matSize; // Assuming matSize is common for all arrays

    // Default c'tor
    Kernels();

    // Setter Methods
    void setIdentity(int **identityNew);
    void setEdgeDetection(int **edgeDetectionNew);
    void setSharpen(int **sharpenNew);
    void setGBlur(double **gaussBlurNew);
    void setMatSize(unsigned short matSizeNew);
    void setInstances(
        unsigned short edge,
        unsigned short blur, 
        unsigned short sharp
    );
};
#endif