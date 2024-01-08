#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

const char* DATA_DIR = "document/rngst_data";
const int LINX_MAX = 3 * 50;
const char* OPTSTRING = "s:";

extern char* optarg;
extern int optind, opterr, optopt;

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

int read_from_file(char* path) {
  FILE* f = fopen(path, "r");
  if (f == NULL) {
    return -1;
  }
  char str[LINX_MAX];
  while (fgets(str, LINX_MAX, f) != NULL) {
    printf("%s", str);
  }
  fclose(f);
  return 0;
}

int gen_and_write_to_file(char* path, int line, int cap) {
  int data[line][cap];
  gen_matrix(line, cap, data);
  FILE* fp = fopen(path, "w");
  if (fp == NULL) {
    perror("rngst open file faild");
    return -1;
  }
  char* str = (char*)calloc(sizeof(char), (cap + 1) * line * 3);
  matrix_to_string(line, cap, data, str);
  fputs(str, fp);
  fclose(fp);
  printf("%s\n", str);
  free(str);
  return 0;
}

void usage() {
  printf("Usage: rngst [-s line x cap]\n");
  printf("  -s line x cap: set the size of matrix\n");
}

int get_size_from_args(int argc, char const* argv[], int* line, int* cap) {
  if (argc < 2) {
    *line = 20;
    *cap = 25;
    return 0;
  }

  int opt;
  while ((opt = getopt(argc, argv, OPTSTRING)) != -1) {
    switch (opt) {
      case 's':
        int ret = sscanf(optarg, "%dx%d", line, cap);
        if (ret != 2) {
          return -1;
        }
        break;
      case '?':
        return -1;
      default:
        break;
        return 0;
    }
  }

  return 0;
}

int main(int argc, char const* argv[]) {
  char* path = (char*)malloc(128);
  int exists = check_file(path);
  int status = 0;
  switch (exists) {
    case 1:
      if (read_from_file(path) != 0) {
        perror("read file error: ");
        status = -1;
      }
      break;
    case 0:
      int line = 0;
      int cap = 0;
      int ret = get_size_from_args(argc, argv, &line, &cap);
      if (ret != 0) {
        usage();
        status = -1;
        break;
      }

      ret = gen_and_write_to_file(path, line, cap);
      if (ret != 0) {
        status = -1;
        perror("write file error: ");
      }
      break;
    case -1:
      status = -1;
      break;
    default:
      perror("some error happend in check file");
      status = -1;
      break;
  }
  free(path);
  return 0;
}
