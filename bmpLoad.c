
#include <stdio.h>
#include "bmpStruct.h"

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
    printf("size : %d, width : %d, height : %d, padding : %d\n", size, width, height, padding);
    
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

    fpNew = fopen("sample_new.bmp", "w");

    // Modify
    for(int x=50;x<100;x++){
        for(int y=50;y<300;y++){
            img[y][x].rgbtBlue = 0;
            img[y][x].rgbtGreen = 0;
            img[y][x].rgbtRed = 255;
        }
    }   
    
    // SAVED
    fwrite(&fh, sizeof(unsigned char), sizeof(BITMAPFILEHEADER), fpNew);
    fwrite(&ih, sizeof(unsigned char), sizeof(BITMAPINFOHEADER), fpNew);
    fwrite(&img, sizeof(unsigned char), sizeof(RGBTRIPLE)*width*height, fpNew);
    fclose(fp);
    fclose(fpNew);

    return 0;
}


