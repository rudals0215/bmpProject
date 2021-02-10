// #include <sys/types.h>
// #include <sys/mman.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <string.h>
// #include <stdlib.h>
// #include "bmpStruct.h"

// #define BUF_MAX 100
// #define PIXEL_SIZE 3

// int main(){
//     int fd;
//     int fdNew;
//     struct stat statbuf;
//     int size, width, height, padding;

//     fd = open("sample.bmp", O_RDONLY);
//     if(fd == -1){
//         perror("open fd");
//         exit(1);
//     }

//     if(stat("sample.bmp", &statbuf) == -1){
//         perror("stat");
//         exit(1);
//     }
//     printf("file size : %ld\n", statbuf.st_size);

//     fdNew = open("sample_new.bmp", O_WRONLY | O_CREAT | O_SYNC | O_TRUNC, S_IRUSR | S_IWUSR | S_IRWXG | S_IWGRP | S_IROTH);
//     if(fdNew == -1){
//         perror("open fdnew");
//         exit(1);
//     }

//     BITMAPFILEHEADER fh;                                                                                                                                                                                                                           
//     BITMAPINFOHEADER ih;   

//     read(fd, &fh, sizeof(BITMAPFILEHEADER));
//     read(fd, &ih, sizeof(BITMAPINFOHEADER));

//     write(fdNew, &fh, sizeof(BITMAPFILEHEADER));
//     write(fdNew, &ih, sizeof(BITMAPINFOHEADER));

//     printf("### Header ###\n");
//     printf("bfType = %c, bfSize = %u, bfOffBits = %u\n", fh.bfType, fh.bfSize, fh.bfOffBits);                                                                         
//     printf("w = %d, h = %d\n", ih.biWidth, ih.biHeight);
//     printf("biBitCount = %u\n", ih.biBitCount);

//     size = ih.biSizeImage;    // 픽셀 데이터 크기
//     width = ih.biWidth;       // 비트맵 이미지의 가로 크기
//     height = ih.biHeight;     // 비트맵 이미지의 세로 크기
//     padding = 0;

//     if (size == 0)    // 픽셀 데이터 크기가 0이라면
//     {
//         // 이미지의 가로 크기 * 픽셀 크기에 남는 공간을 더해주면 완전한 가로 한 줄 크기가 나옴
//         // 여기에 이미지의 세로 크기를 곱해주면 픽셀 데이터의 크기를 구할 수 있음
//         size = (width * PIXEL_SIZE + padding) * height;
//     }
//     printf("size : %d, width : %d, height : %d, padding : %d\n", size, width, height, padding);

//     RGBTRIPLE * bmh;
//     // bmh= (RGBTRIPLE **)malloc(sizeof(RGBTRIPLE *) * height);
//     // for(int i=0;i<height;i++){
//     //     bmh[i] = (RGBTRIPLE *)malloc(sizeof(RGBTRIPLE) * width);
//     // }
//     off_t offset, pa_offset;
//     offset = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
//     pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1); /* offset for mmap() must be page aligned */
//     if((bmh = (RGBTRIPLE *)mmap(0, statbuf.st_size - offset, PROT_READ , MAP_SHARED, fd, (off_t) pa_offset)) == -1){
//         perror("bitmap error");
//         exit(1);
//     }

//     write(fdNew, bmh, statbuf.st_size - offset);

    
//     close(fd);
//     close(fdNew);
    
//     // IMG data
//     // printf("### Image ###\n");
//     // for(int x=0 ; x<height ; x++){
//     //     for(int y=0 ; y<width ; y++){
//     //         // if(x>1430 && y>1070)
//     //         if(x<5 && y<5)
//     //             printf("bmh[%d,%d] BGR : %u %u %u\n",x,y,bmh[x][y].rgbtBlue,bmh[x][y].rgbtGreen,bmh[x][y].rgbtRed);
//     //     }
//     // }


//     return 0;
// }


