#include <stdlib.h>
int main(){
    int i;
    for(i=0;i<10;i++){
        system("./bmpMain.out sample.bmp sample_test1.bmp 1");    
        system("./bmpMain.out sample.bmp sample_test2.bmp 2"); 
        system("./bmpMain.out sample.bmp sample_test3.bmp 3"); 
        system("./bmpMain.out sample.bmp sample_test4.bmp 4"); 
    }
}