#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>
#include <errno.h>
#include "bmpStruct.h"

#define THR_SIZE 4

void divideSection(int index[][4], int width, int height, int numSection);
void* func(void *arg);

#define BUF_MAX 100
#define PIXEL_SIZE 3

int main(){
    FILE *fp;                    // 비트맵 파일 포인터
    FILE *fpNew;                    // 텍스트 파일 포인터

    unsigned char *image;
    int size, width, height, padding;
    char ascii[] = { '#', '#', '@', '%', '=', '+', '*', ':', '-', '.', ' ' };    // 11개

    char buf[BUF_MAX];
                                                                                                                                                                                                                             
    BITMAPFILEHEADER fh;                                                                                                                                                                                                                           
    BITMAPINFOHEADER ih;   
    fp = fopen("sample.bmp","r");

    fread(&fh, sizeof(unsigned char), sizeof(BITMAPFILEHEADER), fp);
    fread(&ih, sizeof(unsigned char), sizeof(BITMAPINFOHEADER), fp);

    printf("### Header ###\n");
    printf("bfType = %c, bfSize = %u, bfOffBits = %u\n", fh.bfType, fh.bfSize, fh.bfOffBits);                                                                         
    printf("w = %d, h = %d\n", ih.biWidth, ih.biHeight);
    printf("biBitCount = %u\n", ih.biBitCount);

    size = ih.biSizeImage;    // 픽셀 데이터 크기
    width = ih.biWidth;       // 비트맵 이미지의 가로 크기
    height = ih.biHeight;     // 비트맵 이미지의 세로 크기
    padding = 0;

    if (size == 0)    // 픽셀 데이터 크기가 0이라면
    {
        // 이미지의 가로 크기 * 픽셀 크기에 남는 공간을 더해주면 완전한 가로 한 줄 크기가 나옴
        // 여기에 이미지의 세로 크기를 곱해주면 픽셀 데이터의 크기를 구할 수 있음
        size = (width * PIXEL_SIZE + padding) * height;
    }

    // IMG data
    RGBTRIPLE img[height][width];
    fread(img, sizeof(unsigned char), sizeof(RGBTRIPLE)*width*height, fp);

    printf("### Image ###\n");
    for(int x=0 ; x<width ; x++){
        for(int y=0 ; y<height ; y++){
            // if(x>1430 && y>1070)
            if(x<5 && y<5)
            printf("img[%d,%d] BGR : %u %u %u\n",y,x,img[y][x].rgbtBlue,img[y][x].rgbtGreen,img[y][x].rgbtRed);
        }
    }


    // Modify through 4 threads
    pthread_t t_id[THR_SIZE];
    cpu_set_t cpuset;

    int index[THR_SIZE][4];

    divideSection(index, width, height, THR_SIZE);

    int light[] = {50, 75, 25, 100};
    
    THREADARGS* param = (THREADARGS*) malloc(sizeof(THREADARGS));

    for(int i=0;i<THR_SIZE;i++){
        param->startX = index[i][0];
        param->startY = index[i][1];
        param->endX = index[i][2];
        param->endY = index[i][3];
        param->img = img;
        param->light = light[i];

        printf("In main, img: %p %u\n",param->img, param->img[0][0].rgbtBlue);

	    if (pthread_create(&t_id[i], NULL, func, (void*)param) != 0){
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

    
    // SAVED
    
    fpNew = fopen("sample_new.bmp", "w");
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

    