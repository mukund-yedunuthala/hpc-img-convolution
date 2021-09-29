# Image convolution using MPI
### Technische Universität Bergakademie Freiberg

Project submitted as a part of the module *High performance computing and optimisation* in Winter Semester 2020-2021.

This programming project applies three specific convolution kernels — blurring, sharpening and edge detection, on two grayscale images. The program has been implemented in parallel with Message Passing Interface (MPI) using C++ programming language on the high performance computing clusters of TU Bergakademie Freiberg. No suited external libraries have been used. 

The following images show the original image with 512 x 512 pixels size, as well as the results upon application of edge detection, gaussian blur, and all three convolutions applied simultaneously. The same has then been implemented on the cluster on an image of size 8162 x 8162 pixels on varying number of processes. The walltime is measured and compared. 