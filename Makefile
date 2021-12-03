# Makefile for Writing Make Files Example
 
# *****************************************************
# Variables to control Makefile operation
 
GCC = g++
PROGRAMS = MainParser Tokenizer
 
# ****************************************************
# Targets needed to bring the executable up to date
 
main: MainParser.o Tokenizer.o
	$(GCC) MainParser.o Tokenizer.o -o MainParser.out

MainParser.o: MainParser.cpp
	$(GCC) -c MainParser.cpp

Tokenizer.o: Tokenizer.cpp
	$(GCC) -c Tokenizer.cpp

clean:
	$(RM)  *.o *.out
