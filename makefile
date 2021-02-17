all: bmpMain.out auto.out cpu_affinity_thread.out

bmpMain.out : bmpMain_mmap.c
	gcc -o bmpMain.out bmpMain_mmap.c -lpthread -std=c99

auto.out: auto.c
	gcc -o auto.out auto.c

cpu_affinity_thread.out : cpu_affinity_thread.c
	gcc -o cpu_affinity_thread.out cpu_affinity_thread.c -lpthread -lm

clean:
	rm *.out time.csv