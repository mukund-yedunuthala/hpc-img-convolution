#!/bin/bash
g++ -o main.out main.cpp &&
./main.out >> ./output.txt &&
echo "Converting 1" &&
convert result01-edgedetection-512.pgm result01-edgedetection-512.png &&
echo "Converting 2" &&
convert result02-blur-512.pgm result02-blur-512.png &&
echo "Converting 3" &&
convert result03-all-512.pgm result03-all-512.png &&
echo "Job finished"
echo "Generating updated report" &&
if command -v pdflatex &> /dev/null
then
   pdflatex Documentation.tex
   echo "Report updated"
   exit
fi
