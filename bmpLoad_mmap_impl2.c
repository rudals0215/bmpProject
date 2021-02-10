/*
    mmap 함수를 1차원 배열을 이용하여 구현
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
    struct stat statbuf;
    int size, width, height, padding;
                                                                                                                                                                                                                              
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
    off_t offset = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    if((bmh = (BITMAPHEADER *)mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, (off_t) 0)) == -1){
        perror("bitmap error");
        exit(1);
    }
    write(fdNew, bmh, offset);
    close(fd);
    
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
    
    RGBTRIPLE *bmh_buffer;
    int bufsize = (statbuf.st_size-offset) / 3;
    bmh_buffer = (RGBTRIPLE *)malloc(sizeof(RGBTRIPLE) * bufsize);
    printf("%lu %d\n", sizeof(RGBTRIPLE) * (statbuf.st_size-offset), bufsize);
    for(int i=0;i<bufsize;i++){
        int b, g, r;
        if(i<bufsize/4){
            b = bmh->rgb[i].rgbtBlue + 100;
            g = bmh->rgb[i].rgbtGreen;
            r = bmh->rgb[i].rgbtRed;
        }
        else if(bufsize/4 <=i && i<bufsize/2) {
            b = bmh->rgb[i].rgbtBlue;
            g = bmh->rgb[i].rgbtGreen + 100;
            r = bmh->rgb[i].rgbtRed;
        }else if(bufsize/2 <=i && i<bufsize*3/4){
            b = bmh->rgb[i].rgbtBlue;
            g = bmh->rgb[i].rgbtGreen;
            r = bmh->rgb[i].rgbtRed + 100;
        }else if(bufsize*3/4 <=i && i<bufsize){
            b = bmh->rgb[i].rgbtBlue + 100;
            g = bmh->rgb[i].rgbtGreen + 100;
            r = bmh->rgb[i].rgbtRed + 100;
        }

        if(b > 255) b = 255;
        if(g > 255) g = 255;
        if(r > 255) r = 255;

        bmh_buffer[i].rgbtBlue = b;
        bmh_buffer[i].rgbtGreen = g;
        bmh_buffer[i].rgbtRed = r;
    }

    write(fdNew, bmh_buffer, sizeof(RGBTRIPLE) * bufsize);

    free(bmh_buffer);
    close(fdNew);
    return 0;
}


