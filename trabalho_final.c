#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<dirent.h>
#include<string.h>
#include<bzlib.h>
#include<sys/sysinfo.h>

#define MAX_PATH 1024

void abrir_diretorio(char*);
void compactar_arquivos(char*);

typedef struct info{
  int infd;
  int outfd;
  char filename[MAX_PATH];
}info;

int main(int argc, char**argv){
  int core;
  core = get_nprocs_conf(); //Numero de processadores
  char bz2append[5] = ".bz2";
  char oldName[MAX_PATH];
  strcpy(oldName, argv[1]);
  strncat(argv[1], bz2append, 4);
  rename(oldName, argv[1]);
  abrir_diretorio(argv[1]);
  char fullpath[MAX_PATH];
  sprintf(fullpath, "tar -cf %s.tar %s", argv[1], argv[1]);
  compactar_arquivos(fullpath);
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
      //printf("DIR - %s\n", path);
      abrir_diretorio(path);
    }
    else
    {
      //printf("File - %s/%s\n", dir_name, entrada->d_name);
      char fullpath[MAX_PATH];
      sprintf(fullpath, "bzip2 %s/%s", dir_name, entrada->d_name);
      compactar_arquivos(fullpath);
    }
  }
  closedir(dir);
}

void compactar_arquivos(char *file_name){
  FILE * f = popen(file_name, "r");
  pclose(f);
}
