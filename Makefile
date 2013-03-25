CFLAGS=-Wall -O2

all:		mempig

mempig:		mempig.o

clean:
	rm -rf mempig *.o
