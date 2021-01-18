main.out: operations.h operations.cpp main.cpp
	g++ -o main.out main.cpp operations.cpp
clean:
	rm main.out
