#!/bin/bash
g++ -o main.out main.cpp &&
./main.out &&
echo "Converting 1" &&
convert result01-edgedetection.pgm result01-edgedetection.jpg &&
echo "Converting 2" &&
convert result02-blur.pgm result02-blur.jpg &&
echo "Converting 3" &&
convert result03-all.pgm result03-all.jpg &&
echo "Job finished"