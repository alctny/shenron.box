#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

const int LINE = 20;
const int CAP = 25;
const char* DATA_DIR = "document/rngst_data";

void gen_matrix(int n, int m, int matrix[n][m]);
void show_matrix(int n, int m, int matrix[n][m]);
int check_file(char* path);
void matrix_to_string(int n, int m, int matrix[n][m], char* str);

int main(int argc, char const* argv[]) {
  char* path = (char*)malloc(128);
  int status = check_file(path);
  if (status < 0) {
    // read file faild
    return -1;
  } else if (status == 1) {
    // file exists, read from file
    FILE* f = fopen(path, "r");
    if (f == NULL) {
      perror("rngst open file faild");
      return -1;
    }
    char* str = (char*)calloc(sizeof(char), (CAP + 1) * LINE * 3);
    fread(str, sizeof(char), (CAP + 1) * LINE * 3, f);
    fclose(f);
    printf("%s", str);
    free(path);
    return 0;
  }
  // file noe exists, gen and write to file
  int data[LINE][CAP];
  gen_matrix(LINE, CAP, data);
  FILE* fp = fopen(path, "w");
  if (fp == NULL) {
    perror("rngst open file faild");
    return -1;
  }
  char* str = (char*)calloc(sizeof(char), (CAP + 1) * LINE * 3);
  matrix_to_string(LINE, CAP, data, str);
  fputs(str, fp);
  fclose(fp);
  printf("%s\n", str);
  free(path);
  return 0;
}

void gen_matrix(int n, int m, int matrix[n][m]) {
  srand(time(NULL));
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      matrix[i][j] = rand() % 99 + 1;
    }
  }
}

void show_matrix(int n, int m, int matrix[n][m]) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      printf("%02d ", matrix[i][j]);
    }
    printf("\n");
  }
}

void matrix_to_string(int n, int m, int matrix[n][m], char* str) {
  for (size_t i = 0; i < n; i++) {
    char line[m * 3 + 1];
    for (size_t j = 0; j < m; j++) {
      sprintf(line, "%02d ", matrix[i][j]);
      strcat(str, line);
    }
    strcat(str, "\n");
  }
}

int check_file(char* path) {
  struct passwd* pd = getpwuid(getuid());
  if (pd == NULL) {
    return -1;
  }
  int path_len = sizeof(char) * (strlen(pd->pw_dir) + strlen(DATA_DIR) + strlen("1999-01-01.txt") + 2);
  strcat(path, pd->pw_dir);
  strcat(path, "/");
  strcat(path, DATA_DIR);
  strcat(path, "/");
  struct stat buffer;
  if (stat(path, &buffer) != 0 && mkdir(path, 0775) != 0) {
    perror("rngst data dir not exists and create faild");
    return -1;
  }
  time_t t = time(NULL);
  struct tm* tm = localtime(&t);
  char file_name[20];
  strftime(file_name, 20, "%Y-%m-%d.txt", tm);
  strcat(path, file_name);

  if (stat(path, &buffer) == 0) {
    return 1;
  }
  return 0;
}