sources=sample.c
CC=mpicc

all: sample2

sample: $(sources)
	$(CC) -g -o sample sample.c -lquo -fopenmp

sample2: sample2.c
	$(CC) -g -o sample2 sample2.c -lquo -fopenmp -L/usr/local/lib 

version:
	$(CC) --version

clean: 
	rm sample sample2
