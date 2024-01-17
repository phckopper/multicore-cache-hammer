all: benchmark;
benchmark: benchmark.o
	gcc -pthread main.o auxiliar_functions.o -o benchmark
benchmark.o: main.c auxiliar_functions.c
	gcc -g -c main.c;
	gcc -g -c auxiliar_functions.c;
clean:
	rm *.o; rm benchmark; rm *.csv;

