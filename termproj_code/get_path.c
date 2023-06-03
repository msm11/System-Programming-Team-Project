#include "term_proj.h"
#define MAX_SIZE 100

int searchdir(char* path,char* filename, char** path_list, int* path_num){
 	
	DIR *dir;
    dir=opendir(".");
    struct dirent *dirs=NULL;
    struct stat *status=NULL;
    while((dirs=readdir(dir))!=NULL ){
    	if(strcmp(dirs->d_name,".")==0 ||strcmp(dirs->d_name,"..")==0)
    		continue;
    		
        status=(struct stat*)malloc(sizeof(struct stat));
 	
        stat(dirs->d_name,status);
 		
        if(S_ISDIR(status->st_mode)){
        	char* newpath=(char*)malloc(sizeof(char)*(strlen(path)+strlen(dirs->d_name)+3));
        	strcpy(newpath,path);
			strcat(newpath,dirs->d_name);
         	strcat(newpath,"/");
             	chdir(dirs->d_name);
            	searchdir(newpath,filename, path_list, path_num);
        	chdir("..");
        }
        else if(strcmp(dirs->d_name,filename)==0){
        	char* newpath=(char*)realloc(path,strlen(path)+strlen(dirs->d_name)+3);
			strcat(newpath,dirs->d_name);
			path_list[*path_num]=(char*)malloc(sizeof(char)*strlen(newpath));
			sprintf(path_list[*path_num],"%s",newpath);
			(*path_num)++;
			return 0;
		}
    
    }
    closedir(dir);
	return 0;
}

char** GetPathList(char* filename, int* path_num)
{
	char** path_list;
	path_list = malloc(sizeof(char*) * MAX_SIZE);
	char* path=(char*)malloc(sizeof(char)*100);
	strcpy(path,"");
	strcat(path,"/home/");
    chdir("/home");
	searchdir(path,filename, path_list, path_num);
	return path_list;
}
