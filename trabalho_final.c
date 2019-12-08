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
#include <wait.h>

#define MAX_PATH 100000

int count_child;
pid_t filho;
pid_t glob;

void abrir_diretorio(char *, char *);
void compactar_arquivos(char *);
void delete_dir(char *);
void copy_file(char*,char*);
void cnt_child();
void abre_diretorio(char*,char*, char*);
void abre_arquivo(char*,char*, char*);

int main(int argc, char**argv){
  count_child = 0;
  int a;
  char *dir_dest = malloc(MAX_PATH * sizeof(char));
  strcpy(dir_dest, argv[2]);
  dir_dest[strlen(dir_dest)-4] = '\0';
  mkdir(dir_dest, 0777);
  abrir_diretorio(argv[1], dir_dest);
  char fullpath[MAX_PATH];
  sprintf(fullpath, "tar cf %s %s", argv[2], dir_dest);
  while((glob = wait(&a)) > 0);
  compactar_arquivos(fullpath);
  delete_dir(dir_dest);
  return 0;
}

void abrir_diretorio(char* dir_origin, char *dir_dest){
  DIR *dir = opendir(dir_origin);
  struct dirent *entrada;
  while((entrada = readdir(dir)) != NULL){
    if(entrada->d_type == DT_DIR){
      if(strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0){
        continue;
      }
      abre_diretorio(dir_origin, dir_dest, entrada->d_name);
    }
    else{
      abre_arquivo(dir_origin, dir_dest, entrada->d_name);
    }
  }
  closedir(dir);
}

void cnt_child(){
  count_child--;
}

void abre_diretorio(char*dir_origin, char*dir_dest, char*entrada){
  char path[MAX_PATH];
  char dir_aux[MAX_PATH];
  snprintf(path, sizeof(path), "%s/%s", dir_origin, entrada);
  snprintf(dir_aux, sizeof(dir_aux), "%s/%s", dir_dest, entrada);
  mkdir(dir_aux, 0777);
  abrir_diretorio(path, dir_aux);
}

void abre_arquivo(char*dir_origin, char*dir_dest, char*entrada){
  char *forigin = malloc(MAX_PATH * sizeof(char));
  char *fdest = malloc(MAX_PATH * sizeof(char));
  sprintf(forigin, "%s/%s", dir_origin, entrada);
  sprintf(fdest, "%s/%s", dir_dest, entrada);
  if(count_child < 3){
    signal(SIGINT, cnt_child);
    count_child++;
    if((filho = fork()) == 0){
      copy_file(forigin, fdest);
      char fullpath[MAX_PATH];
      sprintf(fullpath, "bzip2 %s", fdest);
      compactar_arquivos(fullpath);
      kill(getppid(), SIGINT);
      exit(1);
    }
  } else {
    copy_file(forigin, fdest);
    char fullpath[MAX_PATH];
    sprintf(fullpath, "bzip2 %s", fdest);
    compactar_arquivos(fullpath);
  }
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
