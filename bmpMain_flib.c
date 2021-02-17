#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "bmpStruct.h"

void divideSection(int index[][4], int width, int height, int numSection);
void* func(void *arg);

#define BUF_MAX 100
#define PIXEL_SIZE 3

int THR_SIZE = 1;

int main(int argc, char *argv[]){
    clock_t start, end;
    float res;

    FILE *fp;                       // bitmap image file pointer
    FILE *fpNew;                    // new bitmap image file pointer
    char filename[BUF_MAX];         // bitmap image file name
    char newfilename[BUF_MAX];      // new bitmap image file name
    
    int size, width, height, padding;
                                                                                                                                                                                                                             
    BITMAPFILEHEADER fh;   // file header                                                                                                                                                                                                                        
    BITMAPINFOHEADER ih;   // info header

	if (argc != 4) {
		printf("Usage : %s <bitmap file> <new file> <Num of Thread>\n", argv[0]); 
		exit(1);	
	}

    // File Load
    strcpy(filename, argv[1]);
    strcpy(newfilename, argv[2]);
    THR_SIZE = atoi(argv[3]);
    
    fp = fopen(filename,"r");

    fread(&fh, sizeof(unsigned char), sizeof(BITMAPFILEHEADER), fp);
    fread(&ih, sizeof(unsigned char), sizeof(BITMAPINFOHEADER), fp);

    // Header
    printf("### Header ###\n");
    printf("bfType = %c, bfSize = %u, bfOffBits = %u\n", fh.bfType, fh.bfSize, fh.bfOffBits);                                                                         
    printf("w = %d, h = %d\n", ih.biWidth, ih.biHeight);
    printf("biBitCount = %u\n", ih.biBitCount);

    size = ih.biSizeImage;    // Pixel data size
    width = ih.biWidth;       // image width
    height = ih.biHeight;     // image height
    padding = 0;

    if (size == 0)
        size = (width * PIXEL_SIZE + padding) * height;

    // IMG data
    RGBTRIPLE img[height][width];
    fread(img, sizeof(unsigned char), sizeof(RGBTRIPLE)*width*height, fp);

    printf("### Image ###\n");
    for(int x = 0 ; x < width ; x++){
        for(int y = 0 ; y < height ; y++){
            if(x<5 && y<5)
            printf("img[%d,%d] BGR : %u %u %u\n",y,x,img[y][x].rgbtBlue,img[y][x].rgbtGreen,img[y][x].rgbtRed);
        }
    }


    // Modify through threads
    pthread_t t_id[THR_SIZE];
    cpu_set_t cpuset;

    int index[THR_SIZE][4]; // To divide sections
    int light[] = {50, 75, 25, 100}; // percentage to change light of bitmap images
    THREADARGS* param = (THREADARGS*) malloc(sizeof(THREADARGS));


    divideSection(index, width, height, THR_SIZE);

    start = clock();
    for(int i=0;i<THR_SIZE;i++){
        printf("## Thread %d ##\n",i);
        // param->startX = index[i][0]; // startX
        // param->startY = index[i][1]; // startY
        // param->endX = index[i][2];   // endX
        // param->endY = index[i][3];   // endY
        // param->img = img;            // img
        // param->light = light[i];     // light

        // printf("In main, img: %p %u\n",param->img, param->img[0][0].rgbtBlue);

	    if (pthread_create(&t_id[i], NULL, func, (void*)param) != 0){
            printf("pthread_create() error");
            exit(1);
	    }
	    if(pthread_join(t_id[i], NULL)!=0){
            printf("pthread_join() error");
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
    end = clock();    
	res = (float)(end - start)/CLOCKS_PER_SEC;
    printf("Executed time: %f\n", res);
    FILE * time = fopen("time.txt","at");
    fprintf(time, "Thread: %d, Time: %f\n", THR_SIZE, res);
    fclose(time);

    // Save the new img into a new file
    fpNew = fopen(newfilename, "w");
    fwrite(&fh, sizeof(unsigned char), sizeof(BITMAPFILEHEADER), fpNew);
    fwrite(&ih, sizeof(unsigned char), sizeof(BITMAPINFOHEADER), fpNew);
    fwrite(&img, sizeof(unsigned char), sizeof(RGBTRIPLE)*width*height, fpNew);

    fclose(fpNew);
    fclose(fp);
    return 0;
}


void* func(void *arg){
    THREADARGS* args = (THREADARGS*) arg;
    printf("In Thread, [%d, %d]->[%d, %d] img: %p, light: %d\n",args->startX,args->startY,args->endX,args->endY, args->img, args->light);

    int width = 1440;
    RGBTRIPLE (*img)[1440] = args->img;
    // printf("img: %p *img: %p\n",img, *img);
    printf("before BGR %u %u %u\n",img[args->startY][args->startX].rgbtBlue, img[args->startY][args->startX].rgbtGreen, img[args->startY][args->startX].rgbtRed);

    for(int x=args->startX; x<args->endX; x++){
        for(int y=args->startY; y<args->endY; y++){
            // Change Light
            // printf("change light %d %d\n",x,y);
            img[y][x].rgbtBlue = img[y][x].rgbtBlue * args->light / 100;
            img[y][x].rgbtGreen = img[y][x].rgbtGreen * args->light / 100;
            img[y][x].rgbtRed = img[y][x].rgbtRed * args->light / 100;
        }
    }
    printf("after BGR %u %u %u\n",img[args->startY][args->startX].rgbtBlue, img[args->startY][args->startX].rgbtGreen, img[args->startY][args->startX].rgbtRed);
	return NULL;
}

void divideSection(int index[][4], int width, int height, int numSection){
    printf("divide Section %d\n", numSection);
    printf("width X height %d X %d\n", width, height);
    for(int i=0 ; i < numSection ; i++){
        index[i][0] = width / THR_SIZE * i;
        index[i][1] = 0;
        index[i][2] = width / THR_SIZE * (i + 1);
        index[i][3] = height;
        printf("Section %d: [%d,%d]-> [%d,%d]\n", i, index[i][0],index[i][1],index[i][2],index[i][3]);
    }    
}

    