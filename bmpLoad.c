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
#define BUF_MAX 100


int main(int argc, char *argv[]){
    FILE *fp;                       // bitmap image file pointer
    FILE *fpNew;                    // new bitmap image file pointer
    char filename[BUF_MAX];         // bitmap image file name
    char newfilename[BUF_MAX];      // new bitmap image file name
    
    int size, width, height, padding;
                                                                                                                                                                                                                             
    BITMAPFILEHEADER fh;   // file header                                                                                                                                                                                                                        
    BITMAPINFOHEADER ih;   // info header

	if (argc != 2) {
		printf("Usage : %s <bitmap file>\n", argv[0]); 
		exit(1);	
	}

    // File Load
    strcpy(filename, argv[1]);
    
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

    size = width * height;

    // IMG data
    RGBTRIPLE img[height][width];
    fread(img, sizeof(unsigned char), sizeof(RGBTRIPLE)*width*height, fp);

    printf("### Image ###\n");
    for(int x = 0 ; x < width ; x++){
        for(int y = 0 ; y < height ; y++){
            // if(x<10 && y<10)
            printf("img[%d,%d] BGR : %u %u %u\n",y,x,img[y][x].rgbtBlue,img[y][x].rgbtGreen,img[y][x].rgbtRed);
        }
    }

}
