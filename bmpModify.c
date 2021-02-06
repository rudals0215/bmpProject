#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>
#include <errno.h>

#define THR_SIZE 4

void divideSection(int index[][4], int width, int height, int numSection);
void* func(void *arg);
#define MAIN 1

#ifdef MAIN

int main(){
	pthread_t t_id[THR_SIZE];
    cpu_set_t cpuset;

    int index[THR_SIZE][4];

    int width = 100;
    int height = 100;

    divideSection(index, width, height, THR_SIZE);

    for(int i=0;i<THR_SIZE;i++){

	    if (pthread_create(&t_id[i], NULL, func, (void**)&index[i]) != 0){
            puts("pthread_create() error");
            return -1;
	    }
	    if(pthread_join(t_id[i], NULL)!=0){
            puts("pthread_join() error");
            return -1;
        }

        CPU_ZERO(&cpuset);
        CPU_SET(i, &cpuset);
        pthread_setaffinity_np(t_id[i], sizeof(cpuset), &cpuset);

        // Thread Affinity Check
        pthread_getaffinity_np(t_id[i], sizeof(cpuset), &cpuset);
        for (int j = 0; j < 4; j++)
            if (CPU_ISSET(j, &cpuset))
                printf("\t CPU %d\n", j);
    }
    return 0;
}

#endif
    

void* func(void *arg){
    int startX = ((int*)arg)[0];
    int startY = ((int*)arg)[1];
	int endX = ((int*)arg)[2];
    int endY = ((int*)arg)[3];

    for(int x=startX; x<=endX; x++)
        for(int y=startY; y<=endY; y++)
        ; // Change Color
            printf("change Color\n");
    
	return NULL;
}

void divideSection(int index[][4], int width, int height, int numSection){
    printf("divide Section %d\n", numSection);
    printf("width X height %d X %d\n", width, height);
    for(int i=0 ; i < numSection ; i++){
        index[i][0] = width / THR_SIZE * i;
        index[i][1] = height;
        index[i][2] = width / THR_SIZE * (i + 1);
        index[i][3] = height;
        printf("Section %d: [%d,%d]-> [%d,%d]\n", i, index[i][0],index[i][1],index[i][2],index[i][3]);
    }    
}