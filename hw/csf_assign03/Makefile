CC = g++
CONSERVATIVE_FLAGS = -std=c++11 -Wall -Wextra -pedantic
DEBUGGING_FLAGS = -g -O0
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)

csim: csim.o
	$(CC) csim.o -o csim

csim.o: csim.cpp csim.h
	$(CC) $(CFLAGS) -c csim.cpp

clean:
	rm -f *.o csim