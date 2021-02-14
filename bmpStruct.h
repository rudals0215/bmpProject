
#pragma pack(push, 1)                // 구조체를 1바이트 크기로 정렬

typedef struct _BITMAPFILEHEADER     // BMP 비트맵 파일 헤더 구조체
{
    unsigned short bfType;           // BMP 파일 매직 넘버(2)
    unsigned int   bfSize;           // 파일 크기(4)
    unsigned short bfReserved1;      // 예약(2)
    unsigned short bfReserved2;      // 예약(2)
    unsigned int   bfOffBits;        // 비트맵 데이터의 시작 위치(4)
} BITMAPFILEHEADER; // (14)

typedef struct _BITMAPINFOHEADER     // BMP 비트맵 정보 헤더 구조체(DIB 헤더)
{
    unsigned int   biSize;           // 현재 구조체의 크기 (4)
    int            biWidth;          // 비트맵 이미지의 가로 크기 (4)
    int            biHeight;         // 비트맵 이미지의 세로 크기 (4)
    unsigned short biPlanes;         // 사용하는 색상판의 수 (2)
    unsigned short biBitCount;       // 픽셀 하나를 표현하는 비트 수 (2)
    unsigned int   biCompression;    // 압축 방식 (4)
    unsigned int   biSizeImage;      // 비트맵 이미지의 픽셀 데이터 크기 (4)
    int            biXPelsPerMeter;  // 그림의 가로 해상도(미터당 픽셀) (4)
    int            biYPelsPerMeter;  // 그림의 세로 해상도(미터당 픽셀) (4)
    unsigned int   biClrUsed;        // 색상 테이블에서 실제 사용되는 색상 수 (4)
    unsigned int   biClrImportant;   // 비트맵을 표현하기 위해 필요한 색상 인덱스 수 (4)
} BITMAPINFOHEADER; // (40)

typedef struct _RGBTRIPLE            // 24비트 비트맵 이미지의 픽셀 구조체
{
    unsigned char rgbtBlue;          // 파랑 (1)
    unsigned char rgbtGreen;         // 초록 (1)
    unsigned char rgbtRed;           // 빨강 (1)
} RGBTRIPLE; // (3)

#pragma pack(pop)


typedef struct _THREADARGS
{
    int start;
    int end;
    int width;
    RGBTRIPLE* img;
    int numLight;
} THREADARGS;