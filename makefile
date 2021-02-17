CC = gcc

bmpMain.out : bmpMain.o
	gcc -o bmpMain.out bmpMain.o -lpthread -lm

bmpMain.o: bmpMain.c
	gcc -c -o bmpMain.o bmpMain_mmap.c -std=c99

cpu_affinity_thread: cpu_affinity_thread.c
	gcc -o cpu_affinity_thread cpu_affinity_thread.c

clean:
	rm bmpMain.out *.o 