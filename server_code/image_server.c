#include "image_server.h"
#define PORTNUM 15000
#define HOSTLEN 256
#define oops(msg) {perror(msg); exit(1);}

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int client_num = 0;

int main(int argc, char *argv[]){
	struct sockaddr_in saddr;
	struct hostent *hp;
	char hostname[HOSTLEN];
	int sock_id, sock_fd;
	pthread_t t1;
	
	sock_id = socket(PF_INET, SOCK_STREAM, 0);
	if (sock_id == -1) oops("socket");

	bzero((void*)&saddr, sizeof(saddr));
	gethostname(hostname, HOSTLEN);
	hp = gethostbyname(hostname);

	bcopy((void*)hp->h_addr, (void*)&saddr.sin_addr, hp->h_length);
	saddr.sin_port = htons(PORTNUM);
	saddr.sin_family = AF_INET;

	if(bind(sock_id, (struct sockaddr*)&saddr, sizeof(saddr)) != 0)
		oops("bind");

	if(listen(sock_id, 1) != 0) oops("listen");

	while(1){		
		sock_fd = accept(sock_id, NULL, NULL);
		pthread_create(&t1, NULL, RequestHandle, (void*)&sock_fd);
		pthread_detach(t1);
	}
}

void* RequestHandle(void* fd){
	char request[BUFSIZ+20];
	char cmd[20];		
	char bmp_name[BUFSIZ];
	FILE* sock_fpi, *sock_fpo;
	
	int sock_fd = *((int*)fd);	
	if((sock_fpo = fdopen(sock_fd, "w")) == NULL)
		oops("fdopen");
		
	memset(request, 0, sizeof(request));
   	memset(cmd, 0, sizeof(cmd));
	

	if(read(sock_fd, request, BUFSIZ + 20) <= 0)
		oops("read");
		
	pthread_mutex_lock(&lock);
	printf("Recieve Client %d's request\n", ++client_num);	
	pthread_mutex_unlock(&lock);
	sleep(3);
	if(sscanf(request, "%s", cmd) != 1)
			oops("sscanf");
	
	if(!strcmp(cmd, "download")){
		if(sscanf(request, "%*s%s", bmp_name) != 1)
			oops("sscanf");
		PassFile(bmp_name, sock_fpo);
	}
	else if(!strcmp(cmd, "print"))
		PrintBMPList(sock_fpo);
	else
		oops("invalid cmd");		
	fclose(sock_fpo);
	close(sock_fd);
}	

void PrintBMPList(FILE* fpo){
	DIR* dir_ptr;
	struct dirent* direntp;
	if((dir_ptr = opendir(".")) == NULL){
		fprintf(stderr, "cannot open current directory\n");
		exit(1);
	}
	while((direntp = readdir(dir_ptr)) != NULL){
		int d_length = strlen(direntp->d_name);
		if(!strcmp(".bmp", direntp->d_name + (d_length - 4))){
			fputs(direntp->d_name, fpo);
			fputs(" ", fpo);
		}
	}
	closedir(dir_ptr);
}

void PassFile(char* bmp_name, FILE* fpo){
	FILE* fp_in = fopen(bmp_name, "rb");
	if(fp_in == NULL){
		oops("cannot find file");
	}
	BMPHeader header;
	fread(&header, sizeof(BMPHeader), 1, fp_in);
	if (header.bfType != 0x4d42 || header.biBitCount != 8){
		fclose(fp_in);
		fclose(fpo);
		oops("invalid bmp file");
	}
	fwrite(&header, sizeof(BMPHeader), 1, fpo);
	unsigned char palette[256][4];
	fread(palette, sizeof(palette), 1, fp_in);
	fwrite(palette, sizeof(palette), 1, fpo);
	unsigned char pixels[header.biWidth*header.biHeight];
	fread(pixels, sizeof(pixels), 1, fp_in);
	fwrite(pixels, sizeof(pixels), 1, fpo);
}
