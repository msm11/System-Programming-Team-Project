#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>
#pragma pack(push, 1) // 구조체 멤버들을 1바이트 크기로 정렬

typedef struct _BMPHeader {
    unsigned short bfType;
    unsigned int bfSize;  // bmp 파일의 전체 사이즈
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
    unsigned int biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int  biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} BMPHeader;

#pragma pack(pop)


void PrintBMPList(FILE*);
void PassFile(char*, FILE*);
void* RequestHandle(void*);
