#!/bin/bash
g++ -o main.out main.cpp &&
./main.out &&
echo "Converting 1" &&
convert result01-edgedetection.pgm result01-edgedetection.png &&
echo "Converting 2" &&
convert result02-blur.pgm result02-blur.png &&
echo "Converting 3" &&
convert result03-all.pgm result03-all.png &&
echo "Job finished" &&
echo "Generating updated report" &&
if command -v pdflatex &> /dev/null
then
    pdflatex Documentation.tex
    echo "Report updated"
    exit
fi
