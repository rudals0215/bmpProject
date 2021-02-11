#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(){
    int fd;
    int pagesize;
    caddr_t addr;
    struct stat statbuf;

    pagesize = sysconf(_SC_PAGE_SIZE);

    fd = open("9-2.dat", O_RDWR | O_CREAT | O_TRUNC , 0666);
    if(fd == -1){
        perror("open");
        exit(1);
    }

    if(ftruncate(fd, (off_t)(6*pagesize)) == -1){
        perror("ftruncate");
        exit(1);
    }

    addr = mmap(0, 6*pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t) 0);
    if(addr == MAP_FAILED){
        perror("mmap");
        exit(1);
    }

    close(fd);

    strcpy(addr, "Ftruncate Test\n");
}