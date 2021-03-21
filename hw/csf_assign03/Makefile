CC = g++
CONSERVATIVE_FLAGS = -std=c++11 -Wall -Wextra -pedantic
DEBUGGING_FLAGS = -g -O0
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)

csim: csim.o cache_sim.o
	$(CC) csim.o cache_sim.o -o csim

csim.o: csim.cpp csim.h
	$(CC) $(CFLAGS) -c csim.cpp csim.h

cache_sim.o: cache_sim.cpp csim.h
	$(CC) $(CFLAGS) -c cache_sim.cpp -o cache_sim.o 

clean:
	rm -f *.o csim