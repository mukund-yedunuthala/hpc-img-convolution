/**
 * \file
 * \brief Header file defining Kernels struct.
 * \author Venkata Mukund Kashyap Yedunuthala
 * \date 23 January 2024
 */
struct Kernels {
    /**
     * \brief 2D array representing the identity kernel.
     */
    int **identity;

    /**
     * \brief 2D array representing the edge detection kernel.
     */
    int **edgeDetection;

    /**
     * \brief 2D array representing the sharpening kernel.
     */
    int **sharpen;

    /**
     * \brief 2D array representing the Gaussian blur kernel.
     */
    double **gaussBlur;

    /**
     * \brief Array storing the number of instances for each type of kernel.
     */
    unsigned short* instancesArray;

    /**
     * \brief Common size for all the matrices (kernels).
     */
    unsigned short matSize;

    /**
     * \brief Default constructor for the Kernels struct.
     */
    Kernels();

    /**
     * \brief Sets the identity kernel.
     * \param identityNew New identity kernel to set.
     */
    void setIdentity(int **identityNew);

    /**
     * \brief Sets the edge detection kernel.
     * \param edgeDetectionNew New edge detection kernel to set.
     */
    void setEdgeDetection(int **edgeDetectionNew);

    /**
     * \brief Sets the sharpening kernel.
     * \param sharpenNew New sharpening kernel to set.
     */
    void setSharpen(int **sharpenNew);

    /**
     * \brief Sets the Gaussian blur kernel.
     * \param gaussBlurNew New Gaussian blur kernel to set.
     */
    void setGBlur(double **gaussBlurNew);

    /**
     * \brief Sets the common size for all the kernels.
     * \param matSizeNew New common size for the kernels.
     */
    void setMatSize(unsigned short matSizeNew);

    /**
     * \brief Sets the number of instances for each type of kernel.
     * \param edge Number of instances for edge detection kernel.
     * \param blur Number of instances for Gaussian blur kernel.
     * \param sharp Number of instances for sharpening kernel.
     */
    void setInstances(
        unsigned short edge,
        unsigned short blur,
        unsigned short sharp
    );
};
