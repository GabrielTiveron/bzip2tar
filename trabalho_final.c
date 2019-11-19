#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<dirent.h>
#include<string.h>

#define MAX_PATH 1024

void abrir_diretorio(char*);

typedef struct info{
  int infd;
  int outfd;
  char filename[MAX_PATH];
}info;

int main(int argc, char**argv){
  abrir_diretorio(argv[1]);


  return 0;
}

void abrir_diretorio(char*dir_name){
  DIR * dir;
  dir = opendir(dir_name);
  struct dirent *entrada;
  while((entrada = readdir(dir)) != NULL){
    if(entrada->d_type == DT_DIR){
      if(strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0)continue;
      char path[MAX_PATH];
      snprintf(path, sizeof(path), "%s/%s", dir_name, entrada->d_name);
      printf("DIR - %s\n", path);
      abrir_diretorio(path);
    }
    else{
      printf("File - %s/%s\n", dir_name, entrada->d_name);
    }
  }
  closedir(dir);
}
