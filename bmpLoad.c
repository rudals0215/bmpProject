
#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>
#include <errno.h>

#define BUF_MAX 100
#define THR_SIZE 4

typedef struct __attribute__((__packed__)) {                                                                                                                                                                                                                             
    unsigned char fileMarker1;                                                                                                                                                                                              
    unsigned char fileMarker2;                                                                                                                                                                                               
    unsigned int   bfSize;                                                                                                                                                                                                                   
    uint16_t unused1;                                                                                                                                                                                                                        
    uint16_t unused2;                                                                                                                                                                                                                        
    unsigned int   imageDataOffset;                                                                                                                                                            
} FILEHEADER;                                                                                                                                                                                                                                

typedef struct __attribute__((__packed__)) {                                                                                                                                                                                                                             
    unsigned int   biSize;                                                                                                                                                                                                                   
    int            width;                                                                                                                                                                
    int            height;                                                                                                                                                                     
    uint16_t planes;                                                                                                                                                                                                                         
    uint16_t bitPix;                                                                                                                                                                                                                         
    unsigned int   biCompression;                                                                                                                                                                                                            
    unsigned int   biSizeImage;                                                                                                                                                                                                              
    int            biXPelsPerMeter;                                                                                                                                                                                                          
    int            biYPelsPerMeter;                                                                                                                                                                                                          
    unsigned int   biClrUsed;                                                                                                                                                                                                                
    unsigned int   biClrImportant;                                                                                                                                                                                                           
} INFOHEADER;                                                                                                                                                                                                                                

typedef struct __attribute__((__packed__)) {                                                                                                                                                                                                                             
    unsigned char  b;                                                                                                                                                                                                                        
    unsigned char  g;                                                                                                                                                                                                                        
    unsigned char  r;                                                                                                                                                                                                                        
} IMAGE;


void divideSection(int index[][4], int width, int height, int numSection);
void* func(void *arg);

int main(){
    char buf[BUF_MAX];
                                                                                                                                                                                                                             
    FILEHEADER fh;                                                                                                                                                                                                                           
    INFOHEADER ih;   
    FILE *fp = fopen("sample.bmp","rb");
    
    fread(&fh, sizeof(unsigned char), sizeof(FILEHEADER), fp);
    fread(&ih, sizeof(unsigned char), sizeof(INFOHEADER), fp);
    printf("### Header ###\n");
    printf("fM1 = %c, fM2 = %c, bfS = %u, un1 = %hu, un2 = %hu, iDO = %u\n", fh.fileMarker1, fh.fileMarker2, fh.bfSize, fh.unused1, fh.unused2, fh.imageDataOffset);                                                                         
    printf("w = %d, h = %d\n", ih.width, ih.height);

    IMAGE img[ih.width][ih.height];
    fread(img, sizeof(unsigned char), sizeof(IMAGE)*ih.width*ih.height, fp);

    printf("### Color Palette ###\n");
    for(int x=0;x<ih.width;x++){
        for(int y=0;y<ih.height;y++){
            if(x<3 && y<3)
            printf("img[%d,%d] BGR : %u %u %u\n",x,y,img[x][y].b,img[x][y].g,img[x][y].r);
        }
    }

	pthread_t t_id[THR_SIZE];
    cpu_set_t cpuset;

    int index[THR_SIZE][4];
    divideSection(index, ih.width, ih.height, THR_SIZE);
    for(int i=0;i<THR_SIZE;i++){
	    if (pthread_create(&t_id[i], NULL, func, (void**)&index[i]) != 0){
            puts("pthread_create() error");
            return -1;
	    }
	    if(pthread_join(t_id[i], NULL)!=0){
            puts("pthread_join() error");
            return -1;
        }

        CPU_ZERO(&cpuset);
        CPU_SET(i, &cpuset);
        pthread_setaffinity_np(t_id[i], sizeof(cpuset), &cpuset);

        // Thread Affinity Check
        pthread_getaffinity_np(t_id[i], sizeof(cpuset), &cpuset);
        for (int j = 0; j < 4; j++)
            if (CPU_ISSET(j, &cpuset))
                printf("\t CPU %d\n", j);
    }

    return 0;
}

void* func(void *arg){
    int startX = ((int*)arg)[0];
    int startY = ((int*)arg)[1];
	int endX = ((int*)arg)[2];
    int endY = ((int*)arg)[3];

    for(int x=startX; x<=endX; x++)
        for(int y=startY; y<=endY; y++)
        ; // Change Color
            printf("change Color\n");
    
	return NULL;
}

void divideSection(int index[][4], int width, int height, int numSection){
    printf("divide Section %d\n", numSection);
    printf("width X height %d X %d\n", width, height);
    for(int i=0;i<numSection;i++){
        index[i][0] = width / THR_SIZE * i;
        index[i][1] = height;
        index[i][2] = width / THR_SIZE * (i + 1);
        index[i][3] = height;

        printf("Section %d: [%d,%d]-> [%d,%d]\n", i, index[i][0],index[i][1],index[i][2],index[i][3]);
    }    
}