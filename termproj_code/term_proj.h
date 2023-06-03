#include <curses.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dirent.h>
#include <pthread.h>
#include <math.h>
#include <signal.h>
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

typedef struct fileinfo{

	BMPHeader header;
	unsigned char palette[256][4];
	unsigned char* pixels;
    char name[256];

} fileinfo;

// in interface.c
void Start(fileinfo* );
void UseServer(fileinfo*);
void FileNotExist(fileinfo*);
void SearchPath(fileinfo*);
void OnePhotoUsage(fileinfo*);
void TwoPhotoUsage(fileinfo*);
void HLBright(int);
void HLCont(int);
void HLGamma(float);
void HLHigh(float);
int GetBrightenVal();
int GetContVal();
float GetGammaVal();
float GetHighVal();
void Highlight(int, char**, int);
void Highlight2(int, char**, int);
void SignalSetup();
// ------------------
// in bmp_IO.c
int InputFileName(fileinfo*);
void ReadFileFromName(char*, fileinfo*);
char* SaveBMPFile(fileinfo*);
void oops(char* errmsg);
// ------------------
// in image_client.c
char** GetBMPList(char* , int* );
void GetBMPFromServer(char* , char* , fileinfo* );
// ------------------
// in get_path.c
char** GetPathList(char* filename, int* path_num);
int searchdir(char* path,char* filename, char** path_list, int* path_num);
// ------------------
// in image_functions.c
unsigned char** Allocate2D(int w, int h);
unsigned char** Reshape2D(unsigned char* p, int w, int h);
int bound(int num);
void Inverse(unsigned char* src, int w, int h);
void Brighten(unsigned char* src, int w, int h, int n);
void Contrast(unsigned char* src, int w, int h, int n);
void GammaFunc(unsigned char* src, int w, int h, float gamma);
void Add(unsigned char* src1, unsigned char* src2, int w, int h);
void Subtract(unsigned char* src1, unsigned char* src2, int w, int h);
void Avg(unsigned char* src1, unsigned char* src2, int w, int h);
void Diff(unsigned char* src1, unsigned char* src2, int w, int h);
void And(unsigned char* src1, unsigned char* src2, int w, int h);
void Or(unsigned char* src1, unsigned char* src2, int w, int h);
unsigned char* UnSharpMask(unsigned char* src, int w, int h);
unsigned char* HighBoost(unsigned char* src, int w, int h, float alpha);
unsigned char* Translate(unsigned char* src, int w, int h, int delta_x, int delta_y);
unsigned char* Rotate90(unsigned char* src, int w, int h);
unsigned char* Rotate180(unsigned char* src, int w, int h);
unsigned char* Rotate270(unsigned char* src, int w, int h);
unsigned char* Mirror(unsigned char* src, int w, int h);
unsigned char* Flip(unsigned char* src, int w, int h);
// ------------------
