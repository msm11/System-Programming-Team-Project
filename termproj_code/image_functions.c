#include "term_proj.h"

unsigned char** Allocate2D(int w, int h){
	unsigned char* p;
	p = (unsigned char*)calloc(w*h ,sizeof(unsigned char)); 
	unsigned char** p2;
	p2 = (unsigned char**)calloc(h, sizeof(unsigned char*));
	p2[0] = &p[0];
	for(int i = 1; i < h; i++)
		p2[i] = p2[i-1] + w;
	return p2;
}

unsigned char** Reshape2D(unsigned char* p, int w, int h){
	unsigned char** p2;
	p2 = (unsigned char**)calloc(h, sizeof(unsigned char*));
	p2[0] = &p[0];
	for(int i = 1; i < h; i++)
		p2[i] = p2[i-1] + w;
	return p2;
}

unsigned char* Allocate1D(int w, int h){
	unsigned char* p;
	p = (unsigned char*)calloc(w*h, sizeof(unsigned char));
	return p;
}

int bound(int num){
	if(num > 255) num = 255;
	else  if (num < 0) num = 0;
	return num;	
}

// 화질 향상 기법들 // 
void Inverse(unsigned char* src, int w, int h){
	for (int i = 0; i < w*h; i++)
		src[i] = 255 - src[i];
}

void Brighten(unsigned char* src, int w, int h, int n){
	for (int i = 0; i < w*h; i++)
		src[i] = bound(src[i]+ n);
}

void Contrast(unsigned char* src, int w, int h, int n){
	for (int i = 0; i < w*h; i++)
		src[i] = (unsigned char) bound(src[i] + (src[i] - 128) * n/100);
}

void GammaFunc(unsigned char* src, int w, int h, float gamma){
	float inv_gamma = 1.f / gamma;
	for (int i = 0;
	 i < w*h; i++)
		src[i] = (unsigned char) bound((pow(src[i] / 255.f, inv_gamma)* 255 + 0.5f));
}

// 산술 연산 기법 //

// 이항 연산에서 두 이미지의 사이즈 비교는 이전에 한다고 가정
void Add(unsigned char* src1, unsigned char* src2, int w, int h){
	for (int i = 0; i < w*h; i++)
		src1[i] = bound(src1[i]+src2[i]);
}

void Subtract(unsigned char* src1, unsigned char* src2, int w, int h){
	for (int i = 0; i < w*h; i++)
		src1[i] = bound(src1[i]-src2[i]);
}

void Avg(unsigned char* src1, unsigned char* src2, int w, int h){
	for (int i = 0; i < w*h; i++)
		src1[i] = (src1[i] + src2[i]) / 2;
}

void Diff(unsigned char* src1, unsigned char* src2, int w, int h){
	for(int i = 0; i < w*h; i++){
		int temp = src1[i] - src2[i];
		src1[i] = (unsigned char)abs(temp);
	}
}

void And(unsigned char* src1, unsigned char* src2, int w, int h){
	for(int i = 0; i < w*h; i++)
		src1[i] = src1[i] & src2[i];
}

void Or(unsigned char* src1, unsigned char* src2, int w, int h){
	for(int i = 0; i < w*h; i++)
		src1[i] = src1[i] | src2[i];
}

// 공간적 필터링 //

unsigned char* UnSharpMask(unsigned char* src, int w, int h){
	unsigned char** dst = Allocate2D(w, h);
	unsigned char** src2d = Reshape2D(src, w, h);

	for (int j = 1; j < h-1; j++){
		for(int i = 1; i < w-1; i++){
			dst[j][i] = bound(5*src2d[j][i] - src2d[j+1][i] - src2d[j-1][i] - src2d[j][i+1] - src2d[j][i-1]);
		}
	}
	free(src);
	return dst[0];
}

unsigned char* HighBoost(unsigned char* src, int w, int h, float alpha){
	float sum;
	unsigned char** dst = Allocate2D(w, h);
	unsigned char** src2d = Reshape2D(src, w, h);

	for (int j = 1; j < h-1; j++){
		for(int i = 1; i < w-1; i++){
			sum = (4+alpha)*src2d[j][i] - src2d[j+1][i] - src2d[j-1][i] - src2d[j][i+1] - src2d[j][i-1];
			dst[j][i] = (unsigned char)bound(sum+0.5f);
		
		}
	}
	free(src);
	return dst[0];
}
// 이동 변환 //

unsigned char* Translate(unsigned char* src, int w, int h, int delta_x, int delta_y){
	int x, y;
	src = Flip(src, w, h);
	unsigned char** dst = Allocate2D(w, h);
	unsigned char** src2d = Reshape2D(src, w, h);
	for (int j = 0; j < h; j++){
		for(int i = 0; i < w; i++){
			x = i - delta_x;
			y = j - delta_y;
			if( x>= 0 && x < w && y >= 0 && y < h)
				dst[j][i] = src2d[y][x];
		}
	}
	dst[0] = Flip(dst[0], w, h);
	free(src);
	return dst[0];
}


unsigned char* Rotate90(unsigned char* src, int w, int h){
	unsigned char** dst = Allocate2D(h, w);
	unsigned char** src2d = Reshape2D(src, w, h);
	
	for(int j = 0; j < w; j++){
		for (int i = 0; i < h; i++)
			dst[j][i] = src2d[i][w - 1- j];
	}
	free(src);
	return dst[0];
}

unsigned char* Rotate180(unsigned char* src, int w, int h){
	unsigned char** dst = Allocate2D(w, h);
	unsigned char** src2d = Reshape2D(src, w, h);

	for(int j = 0; j < h; j++){
		for(int i = 0; i < w; i++)
			dst[j][i] = src2d[h-1-j][w-1-i];
	}
	free(src);
	return dst[0];
}

unsigned char* Rotate270(unsigned char* src, int w, int h){
	unsigned char** dst = Allocate2D(h, w);
	unsigned char** src2d = Reshape2D(src, w, h);

	for(int j = 0; j < w; j++){
		for (int i = 0; i < h; i++)
			dst[j][i] = src2d[h-1-i][j];
	}
	free(src);
	return dst[0];
}

unsigned char* Mirror(unsigned char* src, int w, int h){
	unsigned char** dst = Allocate2D(w, h);
	unsigned char** src2d = Reshape2D(src, w, h);

	for(int j = 0; j < h; j++){
		for(int i = 0; i < w; i++)
			dst[j][i] = src2d[j][w-1-i];
	}
	free(src);
	return dst[0];
}

unsigned char* Flip(unsigned char* src, int w, int h){
	unsigned char** dst = Allocate2D(w, h);
	unsigned char** src2d = Reshape2D(src, w, h);
	
	for(int j = 0; j < h; j++){
		for(int i = 0; i < w; i++)
			dst[j][i] = src2d[h-1-j][i];
	}
	free(src);
	return dst[0];
}