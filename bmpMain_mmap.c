#define _GNU_SOURCE 
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "bmpStruct.h"

void divideSection(int index[][2], int width, int height, int numSection, int how);
void* func(void *arg);

#define THREAD_MAX 4
#define BUF_MAX 100
int THREAD_SIZE;

int main(int argc, char *argv[]){
    clock_t start, end;
    float res;

    int fd;
    int fdNew;
    int size, width, height, padding;
    char filename[BUF_MAX];         // bitmap image file name
    char newfilename[BUF_MAX];      // new bitmap image file name
    
    BITMAPFILEHEADER fh;
    BITMAPINFOHEADER ih;
    RGBTRIPLE * img;

	if (argc != 4) {
		printf("Usage : %s <bitmap file> <new file> <Num of Thread>\n", argv[0]); 
		exit(1);	
	}

    // load the filename and thread size
    strcpy(filename, argv[1]);
    strcpy(newfilename, argv[2]);
    THREAD_SIZE = atoi(argv[3]);

    if(THREAD_SIZE > THREAD_MAX){
        printf("Thread size is more than maximum\n");
        exit(1);
    }

    // Load a image
    fd = open(filename, O_RDONLY);
    if(fd == -1){
        perror("open fd");
        exit(1);
    }

    struct stat buf;
    if (fstat(fd, &buf)) {
        perror("fstat failed on bitmap!");
        close(fd);
        exit(1);
    }

    void * addr = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    memcpy(&fh, addr, sizeof(BITMAPFILEHEADER));
    memcpy(&ih, addr + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));

    printf("### Header ###\n");
    printf("bfType = %c, bfSize = %u, bfOffBits = %u\n", fh.bfType, fh.bfSize, fh.bfOffBits);                                                                         
    printf("w = %d, h = %d\n", ih.biWidth, ih.biHeight);
    printf("biBitCount = %u\n", ih.biBitCount);

    width = ih.biWidth;
    height = ih.biHeight;
    size = width*height;

    img = (RGBTRIPLE *)malloc(sizeof(RGBTRIPLE)*size);
    memcpy(img, addr + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), sizeof(RGBTRIPLE)*size);

    // Modify through threads
    pthread_t t_id[THREAD_SIZE];

    int index[THREAD_SIZE][2]; // To divide sections
    int numLight[] = {0,1,2,3}; // To select colors to apply
    THREADARGS* param[THREAD_SIZE];

    divideSection(index, width, height, THREAD_SIZE, 1);

    for(int i=0;i<THREAD_SIZE;i++){
        param[i] = (THREADARGS*) malloc(sizeof(THREADARGS));
        printf("## Thread %d ##\n",i);
        param[i]->start = index[i][0]; // start
        param[i]->end = index[i][1];   // end
        param[i]->width = width;
        param[i]->img = img;            // img
        param[i]->numLight = (numLight[i] % THREAD_SIZE);

        printf("In main, img: %p %d\n",param[i]->img, param[i]->start);
    }
    start = clock();
    for(int i=0;i<THREAD_SIZE;i++){
        if (pthread_create(&t_id[i], NULL, func, (void*)param[i]) != 0){
            printf("pthread_create() error");
            exit(1);
	    }
    }

    for(int i=0;i<THREAD_SIZE;i++){
        if(pthread_join(t_id[i], NULL)!=0){
            printf("pthread_join() error");
            exit(1);
        }
        free(param[i]);
    }
    end = clock();    
	res = (float)(end - start)/CLOCKS_PER_SEC;
    printf("Executed time: %f\n", res);

    // Save the time table
    FILE * time = fopen("time.csv","at");
    fprintf(time, "%d, %f\n", THREAD_SIZE, res);
    fclose(time);

    // Write a new image
    fdNew = open(newfilename, O_WRONLY | O_CREAT | O_SYNC | O_TRUNC, S_IRUSR | S_IWUSR | S_IRWXG | S_IWGRP | S_IROTH);
    if(fdNew == -1){
        perror("open fdNew");
        exit(1);
    }

    write(fdNew, &fh, sizeof(BITMAPFILEHEADER));
    write(fdNew, &ih, sizeof(BITMAPINFOHEADER));
    write(fdNew, img, sizeof(RGBTRIPLE)*size);
    close(fdNew);
    free(img);

    munmap(addr, buf.st_size);
    return 0;
}

void* func(void *arg){
    THREADARGS* args = (THREADARGS*) arg;
    int start = args->start;
    int end = args->end;
    int width = args->width;
    RGBTRIPLE * img = args->img;
    
    int startX = start % width;
    int startY = start / width;
    int endX = end % width;
    int endY = end / width;
    printf("In Thread, %d[%d, %d]-> %d[%d, %d] img: %p, light: %d\n",start, startX, startY, end, endX, endY, args->img, args->numLight);
    printf("%d before BGR %u %u %u\n",args->numLight, img[start].rgbtBlue, img[start].rgbtGreen, img[start].rgbtRed);

    unsigned long mask = 1;
    mask = mask << (args->numLight);
    printf("mask : %lu\n", mask);

    if(pthread_setaffinity_np(pthread_self(), sizeof(mask), (cpu_set_t *)&mask) < 0){
        perror("pthread_setaffinity_np");
    }

    for(int x=startX; x<=endX; x++){
        for(int y=startY; y<=endY; y++){
            // Change Light
            int p = width*y+x;
            switch (args->numLight){
            case 0: 
                if(img[p].rgbtRed < 155) img[p].rgbtRed += 100;
                else img[p].rgbtRed = 255;
                break;
            case 1:
                if(img[p].rgbtGreen < 155) img[p].rgbtGreen += 100;
                else img[p].rgbtGreen = 255;
                break;
            case 2:
                if(img[p].rgbtBlue < 155) img[p].rgbtBlue += 100;
                else img[p].rgbtBlue = 255;
                break;
            case 3:
                if(img[p].rgbtRed < 155) img[p].rgbtRed += 100;
                else img[p].rgbtRed = 255;
                if(img[p].rgbtGreen < 155) img[p].rgbtGreen += 100;
                else img[p].rgbtGreen = 255;
                if(img[p].rgbtBlue < 155) img[p].rgbtBlue += 100;
                else img[p].rgbtBlue = 255;
            
                break;
            }
        }  
    }

    printf("%d after BGR %u %u %u\n",args->numLight, img[start].rgbtBlue, img[start].rgbtGreen, img[start].rgbtRed);
	return NULL;
}

void divideSection(int index[][2], int width, int height, int numSection, int how){
    printf("divide Section = %d\n", numSection);
    printf("width X height = %d X %d\n", width, height);

    for(int i=0 ; i < THREAD_SIZE ; i++){
        int startX,endX,startY,endY;
        switch(how){
            case 0:
            // Horizontal division
            startX = 0;
            endX = width - 1;
            startY = height / numSection * (i % numSection);
            endY = (height / numSection * ((i % numSection) + 1)) - 1;
            break;
            
            case 1:
            // vertical division
            startX = width / numSection * i;
            endX = width / numSection * (i + 1) - 1;
            startY = 0;
            endY = height - 1;
            break;
            
            default:
            break;
        }

        // window-like division
        // numSection = 2;
        // int startX = width / numSection * (i%numSection);
        // int endX = width / numSection * (i%numSection + 1) - 1;
        // int startY = height / numSection * (i/numSection);
        // int endY = height / numSection * (i/numSection + 1) - 1;
        

        index[i][0] = width * startY + startX;
        index[i][1] = width * endY + endX;
        printf("Section %d: %d[%d,%d]-> %d[%d,%d]\n", i, index[i][0], startX, startY, index[i][1], endX, endY);
    }
    
}