# Makefile for Writing Make Files Example
 
# *****************************************************
# Variables to control Makefile operation
 
GCC = g++
PROGRAMS = main Tokenizer
 
# ****************************************************
# Targets needed to bring the executable up to date
 
main: main.o Tokenizer.o
	$(GCC) main.o Tokenizer.o -o main.out

main.o: main.cpp
	$(GCC) -c main.cpp

Tokenizer.o: Tokenizer.cpp
	$(GCC) -c Tokenizer.cpp

clean:
	$(RM)  *.o *.out
