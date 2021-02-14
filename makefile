bmpMain.out : bmpMain.o
	gcc -o bmpMain.out bmpMain.o -lpthread

bmpMain.o: bmpMain.c
	gcc -c -o bmpMain.o bmpMain_mmap.c -std=c99

clean:
	rm *.o bmpMain.out