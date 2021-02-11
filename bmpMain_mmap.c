/*
    mmap 1차원 배열을 이용하여 구현
*/

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
    int size, width, height, padding;
    
    BITMAPFILEHEADER * fh;
    BITMAPINFOHEADER * ih;
    RGBTRIPLE * img;
                                                                                                                                                                                                                              
    fd = open("sample.img", O_RDONLY);
    if(fd == -1){
        perror("open fd");
        exit(1);
    }

    fdNew = open("sample_new.img", O_WRONLY | O_CREAT | O_SYNC | O_TRUNC, S_IRUSR | S_IWUSR | S_IRWXG | S_IWGRP | S_IROTH);
    if(fdNew == -1){
        perror("open fdNew");
        exit(1);
    }

    if((fh = (BITMAPFILEHEADER *)mmap(0, sizeof(BITMAPFILEHEADER), PROT_READ, MAP_SHARED, fd, (off_t) 0))){
        perror("mmap bitmapfileheader error");
        exit(1);
    }
    if((ih = (BITMAPINFOHEADER *)mmap(0, sizeof(BITMAPINFOHEADER), PROT_READ, MAP_SHARED, fd, (off_t) 0))){
        perror("mmap bitmapinfoheader error");
        exit(1);
    }

    printf("### Header ###\n");
    printf("bfType = %c, bfSize = %u, bfOffBits = %u\n", img->fh.bfType, img->fh.bfSize, img->fh.bfOffBits);                                                                         
    printf("w = %d, h = %d\n", img->ih.biWidth, img->ih.biHeight);
    printf("biBitCount = %u\n", img->ih.biBitCount);

    width = ih.biWidth;       // image width
    height = ih.biHeight;     // image height
    padding = 0;
    
    if (size == 0)
        size = (width * PIXEL_SIZE + padding) * height;


    size_t headerSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    if((img = (BITMAP *)mmap(0, size, PROT_READ, MAP_SHARED, fd, (off_t) 0))){
        perror("bitmap error");
        exit(1);
    }
    write(fdNew, img, headerSize);
    close(fd);
    
    
    size = img->ih.biSizeImage;    // 픽셀 데이터 크기
    width = img->ih.biWidth;       // 비트맵 이미지의 가로 크기
    height = img->ih.biHeight;     // 비트맵 이미지의 세로 크기
    padding = 0;

    if (size == 0)    // 픽셀 데이터 크기가 0이라면
    {
        // 이미지의 가로 크기 * 픽셀 크기에 남는 공간을 더해주면 완전한 가로 한 줄 크기가 나옴
        // 여기에 이미지의 세로 크기를 곱해주면 픽셀 데이터의 크기를 구할 수 있음
        size = (width * PIXEL_SIZE + padding) * height;
    }
    printf("size : %d, width : %d, height : %d, padding : %d\n", size, width, height, padding);
    
    RGBTRIPLE *img_buffer;
    int bufsize = (size-headerSize) / 3;
    img_buffer = (RGBTRIPLE *)malloc(sizeof(RGBTRIPLE) * bufsize);
    printf("%lu %d\n", sizeof(RGBTRIPLE) * (size-headerSize), bufsize);

    for(int i=0;i<bufsize;i++){
        int b, g, r;
        if(i<bufsize/4){
            b = img->rgb[i].rgbtBlue + 100;
            g = img->rgb[i].rgbtGreen;
            r = img->rgb[i].rgbtRed;
        }
        else if(bufsize/4 <=i && i<bufsize/2) {
            b = img->rgb[i].rgbtBlue;
            g = img->rgb[i].rgbtGreen + 100;
            r = img->rgb[i].rgbtRed;
        }else if(bufsize/2 <=i && i<bufsize*3/4){
            b = img->rgb[i].rgbtBlue;
            g = img->rgb[i].rgbtGreen;
            r = img->rgb[i].rgbtRed + 100;
        }else if(bufsize*3/4 <=i && i<bufsize){
            b = img->rgb[i].rgbtBlue + 100;
            g = img->rgb[i].rgbtGreen + 100;
            r = img->rgb[i].rgbtRed + 100;
        }

        if(b > 255) b = 255;
        if(g > 255) g = 255;
        if(r > 255) r = 255;

        img_buffer[i].rgbtBlue = b;
        img_buffer[i].rgbtGreen = g;
        img_buffer[i].rgbtRed = r;
    }

    write(fdNew, img_buffer, sizeof(RGBTRIPLE) * bufsize);

    free(img_buffer);
    close(fdNew);
    return 0;
}


