#include "term_proj.h"

extern int width; 
extern int height;

int InputFileName(fileinfo* file){
	clear();
	curs_set(1);
	echo();
	mvprintw(height/2, (width - strlen("Enter file name: "))/2 - 6, "Enter file name: ");
	getstr(file->name);
	int fd_in = open(file->name, O_RDONLY);
    if (fd_in == -1) {
		close(fd_in);
    	return 0;
	}
    	
    read(fd_in, &(file->header), sizeof(BMPHeader));
    if ((file->header).bfType != 0x4d42 || (file->header).biBitCount != 8) {
		close(fd_in);
		clear();
		mvprintw(height/2, (width - strlen("Invalid format"))/2,"Invalid format");
		return 0;
    }
	int w = (file->header).biWidth;
	int h = abs((file->header).biHeight);
	
	read(fd_in, file->palette, sizeof(file->palette));
	file->pixels = malloc(sizeof(unsigned char)*(w*h));
	read(fd_in, file->pixels, sizeof(unsigned char)* (w * h));
	close(fd_in);
	return 1;
}

void ReadFileFromName(char* abs_path, fileinfo* file){
	int fd_in = open(abs_path, O_RDONLY);
	if (fd_in == -1) {
		close(fd_in);
		oops("Failed to open from path!");
	}
	read(fd_in, &(file->header), sizeof(BMPHeader));
    if ((file->header).bfType != 0x4d42 || (file->header).biBitCount != 8) {
		close(fd_in);
		oops("Invalid format");
    }

	int w = (file->header).biWidth;
	int h = abs((file->header).biHeight);
	
	read(fd_in, file->palette, sizeof(file->palette));
	file->pixels = malloc(sizeof(unsigned char)*(w*h));
	read(fd_in, file->pixels, sizeof(unsigned char)* (w * h));
	close(fd_in);
}

char* SaveBMPFile(fileinfo* file){
	clear();
	curs_set(1);
	echo();
	char* filename;
	filename = (char*)malloc(sizeof(char)*256);
	mvprintw(height/2, (width - strlen("Enter output file name: "))/2 - 6, "Enter output file name: ");
    getstr(filename);
	clear();
	int fd_out = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd_out == -1){
        close(fd_out);
        oops("Failed to open output file");
    }
    write(fd_out, &(file->header), sizeof(BMPHeader));
    write(fd_out, file->palette, sizeof(file->palette));
    write(fd_out, file->pixels, sizeof(unsigned char)*(file->header.biWidth*file->header.biHeight));
	mvprintw(height/2, (width - strlen("Save Done"))/2 - 6, "Save Done");
    curs_set(0);
	noecho();
    close(fd_out);
	return filename;
}

void oops(char* errmsg){
	clear();
	mvprintw(height/2, (width - strlen("ERROR:  !") - strlen(errmsg))/2, "ERROR: %s!", errmsg);
	getch();
	endwin();
	exit(-1);
}