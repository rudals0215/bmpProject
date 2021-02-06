#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#define BUF_MAX 100

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

    for(int x=0;x<ih.width;x++){
        for(int y=0;y<ih.height;y++){
            // if(x<3 && y<3)
            printf("img[%d,%d] BGR : %u %u %u\n",x,y,img[x][y].b,img[x][y].g,img[x][y].r);
        }
    }
    // while(!feof(fp)){
    //     fread(buf, sizeof(char), BUF_MAX, fp);
    //     printf("%s\n",buf);
    // }
    return 0;
}