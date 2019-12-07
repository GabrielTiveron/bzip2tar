#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ftw.h>

#define MAX_PATH 1024

void abrir_diretorio(char *, char *);
void compactar_arquivos(char *);
void delete_dir(char *);

int main(int argc, char**argv){
  abrir_diretorio(argv[1], argv[2]);
  char fullpath[MAX_PATH];
  sprintf(fullpath, "tar cf %s.bz2.tar %s", argv[1], argv[2]);
  compactar_arquivos(fullpath);
  delete_dir(argv[2]);
  return 0;
}

void abrir_diretorio(char* dir_origin, char *dir_dest){
  dir_dest[strlen(dir_dest)-4] = '\0';
  mkdir(dir_dest, 0777);
  DIR *dir = opendir(dir_origin);
  struct dirent *entrada;
  while((entrada = readdir(dir)) != NULL){
    if(entrada->d_type == DT_DIR){
      if(strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0){
        continue;
      }
      char path[MAX_PATH];
      snprintf(path, sizeof(path), "%s/%s", dir_origin, entrada->d_name);
      abrir_diretorio(path, dir_dest);
    }
    else{
      char *forigin = malloc(10000 * sizeof(char));
      char *fdest = malloc(10000 * sizeof(char));
      sprintf(forigin, "%s/%s", dir_origin, entrada->d_name);
      sprintf(fdest, "%s/%s", dir_dest, entrada->d_name);
      FILE *fp = fopen(forigin, "r");
      FILE *f = fopen(fdest, "w");
      char c;
      while((c = fgetc(fp)) != EOF){
        fputc(c, f);
      }
      fclose(f);
      fclose(fp);
      char fullpath[MAX_PATH];
      sprintf(fullpath, "bzip2 %s", fdest);
      compactar_arquivos(fullpath);
    }
  }
  closedir(dir);
}

void compactar_arquivos(char *file_name){
  FILE *f = popen(file_name, "r");
  pclose(f);
}

int delete_files(const char *dir, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
  int rv = remove(dir);
  if(rv){
    perror(dir);
  }
  return rv;
}

void delete_dir(char *dir){
  nftw(dir, delete_files, 5, FTW_DEPTH);
}
