#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "bmpStruct.h"

#define BUF_MAX 100
#define PIXEL_SIZE 3

int main(){
    int fd;
    int fdNew;
    struct stat statbuf;
    int size, width, height, padding;
                                                                                                                                                                                                                             
    BITMAPFILEHEADER * fh;                                                                                                                                                                                                                           
    BITMAPINFOHEADER * ih;   

    fd = open("sample.bmp", O_RDONLY);
    if(fd == -1){
        perror("open fd");
        exit(1);
    }

    if(stat("sample.bmp", &statbuf) == -1){
        perror("stat");
        exit(1);
    }
    printf("file size : %ld\n", statbuf.st_size);

    fdNew = open("sample_new.bmp", O_WRONLY | O_CREAT | O_SYNC | O_TRUNC, S_IRUSR | S_IWUSR | S_IRWXG | S_IWGRP | S_IROTH);
    if(fdNew == -1){
        perror("open fdnew");
        exit(1);
    }

    BITMAPHEADER * bmh;
    bmh= (BITMAPHEADER *)malloc(sizeof(statbuf.st_size));
    if((bmh = (BITMAPHEADER *)mmap(0, sizeof(statbuf.st_size), PROT_READ, MAP_SHARED, fd, (off_t) 0)) == -1){
        perror("bitmap error");
        exit(1);
    }
    write(fdNew, bmh, statbuf.st_size);

    printf("### Header ###\n");
    printf("bfType = %c, bfSize = %u, bfOffBits = %u\n", bmh->fh.bfType, bmh->fh.bfSize, bmh->fh.bfOffBits);                                                                         
    printf("w = %d, h = %d\n", bmh->ih.biWidth, bmh->ih.biHeight);
    printf("biBitCount = %u\n", bmh->ih.biBitCount);
        
    size = bmh->ih.biSizeImage;    // 픽셀 데이터 크기
    width = bmh->ih.biWidth;       // 비트맵 이미지의 가로 크기
    height = bmh->ih.biHeight;     // 비트맵 이미지의 세로 크기
    padding = 0;

    if (size == 0)    // 픽셀 데이터 크기가 0이라면
    {
        // 이미지의 가로 크기 * 픽셀 크기에 남는 공간을 더해주면 완전한 가로 한 줄 크기가 나옴
        // 여기에 이미지의 세로 크기를 곱해주면 픽셀 데이터의 크기를 구할 수 있음
        size = (width * PIXEL_SIZE + padding) * height;
    }
    printf("size : %d, width : %d, height : %d, padding : %d\n", size, width, height, padding);

    
    close(fd);
    close(fdNew);
    
    // IMG data
    // RGBTRIPLE img[height][width];
    // fread(img, sizeof(unsigned char), sizeof(RGBTRIPLE)*width*height, fp);

    // printf("### Image ###\n");
    // for(int x=0 ; x<width ; x++){
    //     for(int y=0 ; y<height ; y++){
    //         // if(x>1430 && y>1070)
    //         if(x<5 && y<5)
    //         printf("img[%d,%d] BGR : %u %u %u\n",y,x,img[y][x].rgbtBlue,img[y][x].rgbtGreen,img[y][x].rgbtRed);
    //     }
    // }

    // // Modify
    // for(int x=50;x<100;x++){
    //     for(int y=50;y<300;y++){
    //         img[y][x].rgbtBlue = 0;
    //         img[y][x].rgbtGreen = 0;
    //         img[y][x].rgbtRed = 255;
    //     }
    // }   

    return 0;
}


