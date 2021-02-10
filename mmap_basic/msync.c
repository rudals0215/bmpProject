#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int fd;
    caddr_t addr;
    struct stat statbuf;

    if(argc != 2){
        fprintf(stderr, "Usage : mymmap filename\n");
        exit(1);
    }

    if(stat(argv[1], &statbuf) == -1){
        perror("stat");
        exit(1);
    }

    fd = open(argv[1], O_RDWR);
    if(fd == -1){
        perror("open");
        exit(1);
    }

    addr = mmap(0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)0);
    if(addr == MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    close(fd);

    write(1, addr, statbuf.st_size);
    addr[0] = 'x';

    msync(addr, statbuf.st_size, MS_SYNC);
}

// 파일의 이름을 명령행 인자로 받아 메모리에 매핑시킨 뒤 매핑된 내용을 일부 수정하고 이를 msync()함수를 사용하여 백업저장장치로 보내는 프로그램이다.