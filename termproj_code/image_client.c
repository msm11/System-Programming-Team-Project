#include "term_proj.h"

#define PORTNUM 15000
#define MAX_SIZ 1000

char** GetBMPList(char* IP, int* token_num){
	struct sockaddr_in servadd;
	struct hostent *hp;
	int sock_id;
	FILE* sock_fpi, *sock_fpo;
	char send_request[20];
	char bmp_list[BUFSIZ*10];
	
	sock_id = socket(PF_INET, SOCK_STREAM, 0);
	if(sock_id == -1) oops("socket");
	bzero(&servadd, sizeof(servadd));
	hp = gethostbyname(IP);
	if(hp == NULL) oops(IP);
	bcopy(hp->h_addr, (struct sockaddr*)&servadd.sin_addr, hp->h_length);
	servadd.sin_port = htons(PORTNUM);
	servadd.sin_family = AF_INET;

	if(connect(sock_id, (struct sockaddr*)&servadd, sizeof(servadd)) != 0)
		oops("connect");
	
	sprintf(send_request, "%s", "print");
	if(write(sock_id, send_request, strlen(send_request)) == -1)
		oops("write");	
	
	if((sock_fpi = fdopen(sock_id, "r")) == NULL)
		oops("fdopen reading");
	
	fgets(bmp_list, BUFSIZ*10, sock_fpi);
	char** bmp_tokens;
	bmp_tokens = malloc(sizeof(char*) * MAX_SIZ);
	char* token = strtok(bmp_list, " ");	
	while (token != NULL){
		bmp_tokens[(*token_num)++] = token;
		token = strtok(NULL, " ");
	}
	return bmp_tokens;
}


void GetBMPFromServer(char* IP, char* filename, fileinfo* file){
	struct sockaddr_in servadd;
	struct hostent *hp;
	int sock_id;
	FILE* sock_fpi, *sock_fpo;
	char send_request[BUFSIZ+20];
	
	sock_id = socket(PF_INET, SOCK_STREAM, 0);
	if(sock_id == -1) oops("failed to socket");
	bzero(&servadd, sizeof(servadd));
	hp = gethostbyname(IP);
	if(hp == NULL) oops(IP);
	bcopy(hp->h_addr, (struct sockaddr*)&servadd.sin_addr, hp->h_length);
	servadd.sin_port = htons(PORTNUM);
	servadd.sin_family = AF_INET;

	if(connect(sock_id, (struct sockaddr*)&servadd, sizeof(servadd)) != 0)
		oops("failed to connect");
	
	sprintf(send_request, "%s%s", "download ", filename);
	
	if(write(sock_id, send_request, strlen(send_request)) == -1)
		oops("failed to write");
	if((sock_fpi = fdopen(sock_id, "r")) == NULL)
		oops("failed to fdopen");

	fread(&(file->header), sizeof(BMPHeader), 1, sock_fpi);
	int w = (file->header).biWidth;
	int h = (file->header).biHeight;
	file->pixels = malloc(sizeof(unsigned char)*(w*h));
	fread(file->palette, sizeof(file->palette), 1, sock_fpi);
	fread(file->pixels, sizeof(unsigned char)*(w*h), 1, sock_fpi);
	
	fclose(sock_fpi);
	close(sock_id);
}
