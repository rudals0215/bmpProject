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
    
    caddr_t addr;
    struct stat statbuf;

    unsigned char *image;
    int size, width, height, padding;
    char ascii[] = { '#', '#', '@', '%', '=', '+', '*', ':', '-', '.', ' ' };    // 11개

    char buf[BUF_MAX];
                                                                                                                                                                                                                             
    BITMAPFILEHEADER fh;                                                                                                                                                                                                                           
    BITMAPINFOHEADER ih;   
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

    addr = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, (off_t)0);
    write(fdNew, addr, statbuf.st_size);

    close(fd);
    close(fdNew);

    return 0;
}


