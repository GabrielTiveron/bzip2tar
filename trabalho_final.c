#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ftw.h>

#define MAX_PATH 1024

int count_child;
pid_t filho;

void abrir_diretorio(char *, char *);
void compactar_arquivos(char *);
void delete_dir(char *);
void copy_file(char*,char*);
void cnt_child();

int main(int argc, char**argv){
  count_child = 0;
  char *dir_dest = malloc(MAX_PATH * sizeof(char));
  strcpy(dir_dest, argv[2]);
  dir_dest[strlen(dir_dest)-4] = '\0';
  mkdir(dir_dest, 0777);
  abrir_diretorio(argv[1], dir_dest);
  char fullpath[MAX_PATH];
  sprintf(fullpath, "tar cf %s %s", argv[2], dir_dest);
  compactar_arquivos(fullpath);
  delete_dir(dir_dest);
  return 0;
}

void abrir_diretorio(char* dir_origin, char *dir_dest){
  DIR *dir = opendir(dir_origin);
  struct dirent *entrada;
  while((entrada = readdir(dir)) != NULL){
    if(entrada->d_type != DT_REG){
      if(strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0){
        continue;
      }
      char path[MAX_PATH];
      char dir_aux[MAX_PATH];
      snprintf(path, sizeof(path), "%s/%s", dir_origin, entrada->d_name);
      snprintf(dir_aux, sizeof(dir_aux), "%s/%s", dir_dest, entrada->d_name);
      mkdir(dir_aux, 0777);
      abrir_diretorio(path, dir_aux);
    }
    else{
      char *forigin = malloc(MAX_PATH * sizeof(char));
      char *fdest = malloc(MAX_PATH * sizeof(char));
      sprintf(forigin, "%s/%s", dir_origin, entrada->d_name);
      sprintf(fdest, "%s/%s", dir_dest, entrada->d_name);
      copy_file(forigin, fdest);
      char fullpath[MAX_PATH];
      sprintf(fullpath, "bzip2 %s", fdest);
      if(count_child < 4){
        signal(SIGINT, cnt_child);
        if((filho = fork()) == 0){
          compactar_arquivos(fullpath);
          kill(getppid(), SIGINT);
          exit(1);
        }
        else{count_child++;}
      }else{
        compactar_arquivos(fullpath);
      }
    }
  }
  closedir(dir);
}

void cnt_child(){
  count_child--;
}

void copy_file(char*orig, char*dest){
  FILE *fp = fopen(orig, "rb");
  FILE *f = fopen(dest, "wb");
  char buffer[BUFSIZ];
  size_t buff;
  while((buff = fread(buffer, 1, BUFSIZ, fp)) != 0){
    fwrite(buffer, 1, buff, f);
  }
  fclose(f);
  fclose(fp);

}

void compactar_arquivos(char *file_name){
  FILE *f = popen(file_name, "w");
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
