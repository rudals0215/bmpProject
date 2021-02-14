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

void divideSection(int index[][2], int width, int height, int numSection);

void* fun(void *arg);
// void* funRed(void *arg);
// void* funGreen(void *arg);
// void* funBlue(void *arg);
// void* funWhite(void *arg);

int THR_SIZE = 4;
pthread_mutex_t lock;

int main(int argc, char *argv[]){
    clock_t start, end;
    float res;

    int fd;
    int fdNew;
    int size, width, height, padding;
    
    BITMAPFILEHEADER fh;
    BITMAPINFOHEADER ih;
    RGBTRIPLE * img;

    // Load a image
    fd = open("sample.bmp", O_RDONLY);
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

    for(int i=0;i<5;i++)
        printf("img BGR[%d] : %u %u %u\n",i, img[i].rgbtBlue,img[i].rgbtGreen,img[i].rgbtRed);
    
    // Modify through threads
    pthread_t t_id[THR_SIZE];
    cpu_set_t cpuset;

    int index[THR_SIZE][2]; // To divide sections
    // int light[] = {50, 75, 25, 100}; // percentage to change light of bitmap images
    int numLight[] = {0,1,2,3};
    THREADARGS* param[THR_SIZE];

    divideSection(index, width, height, THR_SIZE);

    void* (*func[4])(void *);
    // func[0] = funRed;
    // func[1] = funGreen;
    // func[2] = funBlue;
    // func[3] = funWhite;

    start = clock();
    for(int i=0;i<THR_SIZE;i++){
        
        param[i] = (THREADARGS*) malloc(sizeof(THREADARGS));
        printf("## Thread %d ##\n",i);
        param[i]->start = index[i][0]; // start
        param[i]->end = index[i][1];   // end
        param[i]->width = width;
        param[i]->img = img;            // img
        param[i]->numLight = numLight[i];
        // param->light = light[i];     // light

        printf("In main, img: %p %d\n",param[i]->img, param[i]->start);

	    if (pthread_create(&t_id[i], NULL, fun, (void*)param[i]) != 0){
            printf("pthread_create() error");
            exit(1);
	    }

        // set the thread affinity
        CPU_ZERO(&cpuset);
        CPU_SET(i, &cpuset);
        pthread_setaffinity_np(t_id[i], sizeof(cpuset), &cpuset);

        // Thread Affinity Check
        pthread_getaffinity_np(t_id[i], sizeof(cpuset), &cpuset);
        for (int j = 0; j < 4; j++)
            if (CPU_ISSET(j, &cpuset))
                printf("\t CPU %d\n", j);
    }
    
    for(int i=0;i<THR_SIZE;i++){
	    if(pthread_join(t_id[i], NULL)!=0){
            printf("pthread_join() error");
            exit(1);
        }
    }

    end = clock();    
	res = (float)(end - start)/CLOCKS_PER_SEC;
    printf("Executed time: %f\n", res);
    FILE * time = fopen("time.txt","at");
    fprintf(time, "Thread: %d, Time: %f\n", THR_SIZE, res);
    fclose(time);

    // Write a new image
    fdNew = open("sample_new.bmp", O_WRONLY | O_CREAT | O_SYNC | O_TRUNC, S_IRUSR | S_IWUSR | S_IRWXG | S_IWGRP | S_IROTH);
    if(fdNew == -1){
        perror("open fdNew");
        exit(1);
    }

    FILE * fpNew = fopen("sample_newtry.bmp", "w");
    
    fwrite(&fh, sizeof(unsigned char), sizeof(BITMAPFILEHEADER), fpNew);
    fwrite(&ih, sizeof(unsigned char), sizeof(BITMAPINFOHEADER), fpNew);
    fwrite(img, sizeof(unsigned char), sizeof(RGBTRIPLE)*width*height, fpNew);

    // void * addrNew = mmap(NULL, buf.st_size, PROT_WRITE, MAP_SHARED, fdNew, 0);
    // memcpy(addrNew, &fh, sizeof(BITMAPFILEHEADER));
    // memcpy(addrNew + sizeof(BITMAPFILEHEADER), &ih, sizeof(BITMAPINFOHEADER));
    // memcpy(addrNew + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), img, sizeof(RGBTRIPLE) * size);

    close(fdNew);
    free(img);

    munmap(addr, buf.st_size);
    // munmap(addrNew, buf.st_size);
    return 0;
}

void* fun(void *arg){
    THREADARGS* args = (THREADARGS*) arg;
    int start = args->start;
    int end = args->end;
    int width = args->width;
    RGBTRIPLE * img = args->img;
    
    int startX = start / width;
    int startY = start % width;
    int endX = end / width;
    int endY = end % width;
    printf("In Thread, %d[%d, %d]-> %d[%d, %d] img: %p, light: %d\n",start, startX, startY, end, endX, endY, args->img, args->numLight);
    printf("%d before BGR %u %u %u\n",args->numLight, img[start].rgbtBlue, img[start].rgbtGreen, img[start].rgbtRed);

    for(int x=startX; x<=endX; x++){
        for(int y=startY; y<=endY; y++){
            // Change Light
            // printf("change light %d %d\n",x,y);
            int p = width*y+x;
            switch (args->numLight){
            case 0: 
                img[p].rgbtRed += 50;
                if(img[p].rgbtRed > 255) img[p].rgbtRed = 255;
                break;
            case 1:
                img[p].rgbtBlue += 50;
                if(img[p].rgbtBlue > 255) img[p].rgbtBlue = 255;
                break;
            case 2:
                img[p].rgbtGreen += 50;
                if(img[p].rgbtGreen > 255) img[p].rgbtBlue = 255;
                break;
            default:
                img[p].rgbtRed += 50;
                if(img[p].rgbtRed > 255) img[p].rgbtRed = 255;
                
                img[p].rgbtGreen += 50;
                if(img[p].rgbtGreen> 255) img[p].rgbtGreen = 255;
                
                img[p].rgbtBlue += 50;
                if(img[p].rgbtBlue > 255) img[p].rgbtBlue = 255;
                break;
            }
        }   
    }

    printf("%d after BGR %u %u %u\n",args->numLight, img[start].rgbtBlue, img[start].rgbtGreen, img[start].rgbtRed);
	return NULL;
}


// void* funRed(void *arg){
//     THREADARGS* args = (THREADARGS*) arg;
//     int start = args->start;
//     int end = args->end;
//     int width = args->width;
//     RGBTRIPLE * img = args->img;
    
//     int startX = start / width;
//     int startY = start % width;
//     int endX = end / width;
//     int endY = end % width;
//     printf("In Thread, %d[%d, %d]-> %d[%d, %d] img: %p, light: Red\n",start, startX, startY, end, endX, endY, args->img);

//     printf("before BGR %u %u %u\n",img[start].rgbtBlue, img[start].rgbtGreen, img[start].rgbtRed);

//     for(int x=startX; x<=endX; x++){
//         for(int y=startY; y<=endY; y++){
//             // Change Light
//             // printf("change light %d %d\n",x,y);
//             int p = width*y+x;
//             img[p].rgbtRed *= 2;
//             if(img[p].rgbtRed > 255) img[p].rgbtRed = 255;
//         }
//     }

//     printf("after BGR %u %u %u\n",img[start].rgbtBlue, img[start].rgbtGreen, img[start].rgbtRed);
// 	return NULL;
// }

// void* funGreen(void *arg){
//     THREADARGS* args = (THREADARGS*) arg;
//     int start = args->start;
//     int end = args->end;
//     int width = args->width;
//     RGBTRIPLE * img = args->img;
    
//     int startX = start / width;
//     int startY = start % width;
//     int endX = end / width;
//     int endY = end % width;
//     printf("In Thread, %d[%d, %d]-> %d[%d, %d] img: %p, light: Green\n",start, startX, startY, end, endX, endY, args->img);

//     printf("before BGR %u %u %u\n",img[start].rgbtBlue, img[start].rgbtGreen, img[start].rgbtRed);

//     for(int x=startX; x<=endX; x++){
//         for(int y=startY; y<=endY; y++){
//             // Change Light
//             // printf("change light %d %d\n",x,y);
//             int p = width*y+x;
//             img[p].rgbtGreen *= 2;
//             if(img[p].rgbtGreen > 255) img[p].rgbtGreen = 255;
//         }
//     }

//     printf("after BGR %u %u %u\n",img[start].rgbtBlue, img[start].rgbtGreen, img[start].rgbtRed);
// 	return NULL;
// }

// void* funBlue(void *arg){
//     THREADARGS* args = (THREADARGS*) arg;
//     int start = args->start;
//     int end = args->end;
//     int width = args->width;
//     RGBTRIPLE * img = args->img;
    
//     int startX = start / width;
//     int startY = start % width;
//     int endX = end / width;
//     int endY = end % width;
//     printf("In Thread, %d[%d, %d]-> %d[%d, %d] img: %p, light: Blue\n",start, startX, startY, end, endX, endY, args->img);

//     printf("before BGR %u %u %u\n",img[start].rgbtBlue, img[start].rgbtGreen, img[start].rgbtRed);

//     for(int x=startX; x<=endX; x++){
//         for(int y=startY; y<=endY; y++){
//             // Change Light
//             // printf("change light %d %d\n",x,y);
//             int p = width*y+x;
//             img[p].rgbtBlue += 100;
//             if(img[p].rgbtBlue > 255) img[p].rgbtBlue = 255;
//         }
//     }

//     printf("after BGR %u %u %u\n",img[start].rgbtBlue, img[start].rgbtGreen, img[start].rgbtRed);
// 	return NULL;
// }

// void* funWhite(void *arg){
//     THREADARGS* args = (THREADARGS*) arg;
//     int start = args->start;
//     int end = args->end;
//     int width = args->width;
//     RGBTRIPLE * img = args->img;
    
//     int startX = start / width;
//     int startY = start % width;
//     int endX = end / width;
//     int endY = end % width;
//     printf("In Thread, %d[%d, %d]-> %d[%d, %d] img: %p, light: White\n",start, startX, startY, end, endX, endY, args->img);

//     printf("before BGR %u %u %u\n",img[start].rgbtBlue, img[start].rgbtGreen, img[start].rgbtRed);

//     for(int x=startX; x<=endX; x++){
//         for(int y=startY; y<=endY; y++){
//             // Change Light
//             // printf("change light %d %d\n",x,y);
//             int p = width*y+x;
//             img[p].rgbtRed *= 2;
//             img[p].rgbtGreen *= 2;
//             img[p].rgbtBlue *= 2;

//             if(img[p].rgbtRed > 255) img[p].rgbtRed = 255;
//             if(img[p].rgbtGreen > 255) img[p].rgbtGreen = 255;
//             if(img[p].rgbtBlue > 255) img[p].rgbtBlue = 255;

//         }
//     }

//     printf("after BGR %u %u %u\n",img[start].rgbtBlue, img[start].rgbtGreen, img[start].rgbtRed);
// 	return NULL;
// }


void divideSection(int index[][2], int width, int height, int numSection){
    printf("divide Section %d\n", numSection);
    printf("width X height %d X %d\n", width, height);

    for(int i=0 ; i < numSection ; i++){
        int startX = width / numSection * i;
        int startY = 0;
        int endX = width / numSection * (i + 1) - 1;
        int endY = height - 1;

        index[i][0] = width*startX + startY;
        index[i][1] = width*endX + endY;
        printf("Section %d: %d[%d,%d]-> %d[%d,%d]\n", i, index[i][0], startX, startY, index[i][1], endX, endY);
    }    
}

    