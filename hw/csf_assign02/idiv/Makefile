CC = gcc
CFLAGS = -g -no-pie
LDFLAGS = -g -no-pie

EXES = idiv

%.o  : %.S
	$(CC) -c $(CFLAGS) -o $*.o $*.S

all : $(EXES)

idiv : idiv.o
	$(CC) $(LDFLAGS) -o $@ idiv.o

clean :
	rm -f $(EXES) *.o
