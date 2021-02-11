#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    int fd;
    caddr_t addr;
    struct stat statbuf;

    if(argc != 2){
        fprintf(stderr,"Usage : mymmap file name\n");
        exit(1);
    }
    
    // 파일의 크기 정보를 구하기 (statbuf.st_size)
    if(stat(argv[1], &statbuf) == -1){
        perror("stat");
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);
    if(fd == -1){
        perror("open");
        exit(1);
    }

    addr = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, (off_t)0);
    if(addr == MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    close(fd);

    write(1, addr, statbuf.st_size);
}