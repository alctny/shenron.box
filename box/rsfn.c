// P.S. 虽然此函数中的方法并不会被抽象为第三方库,但本着函数尽可能要通用的原则
// 所有函数的入参和出参均使用 char/char* 而不使用 wchar_t/wchar_t*

#include "rsfn.h"

#include <dirent.h>
#include <libgen.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wchar.h>

wchar_t *invaild_chars = L"，。【】（）()[]:：「」,|<>《》][;；?！\\ \"'";
int invaild_chars_len = 0;
char *path = "";
char *specific = "";

// 隐藏文件视作保护文件，不做不允许对其进行重命名，避免带来不必要的麻烦
int rsfd(char *path, char *spe) {
  char *base_path;
  char *new_file_path;
  char *new_file_name;
  char *old_file_name;
  DIR *dir;
  struct dirent *de;
  char *path_cpy = (char *)malloc(sizeof(char *) * strlen(path));
  strcpy(path_cpy, path);

  // 普通文件
  if (1 == is_reg(path)) {
    old_file_name = basename(path);
    base_path = dirname(path_cpy);
    new_file_name = (char *)malloc(sizeof(char *) * strlen(old_file_name));
    rm_invaild_chars(new_file_name, old_file_name);
    new_file_path = path_join(base_path, new_file_name);
    // rename(path, new_file_path);
    // printf("src:  %s\ndes:  %s\n\n", old_file_name, new_file_name);
    free(new_file_name);
    return 0;
  }

  // 既不是普通文件，也不是文件夹，不做处理
  if (1 != is_dir(path)) {
    return 1;
  }

  dir = opendir(path);
  if (NULL == dir) {
    return -1;
  }
  while ((de = readdir(dir))) {
    if ('.' == *(de->d_name)) {
      continue;
    }
    switch (de->d_type) {
      case DT_DIR:
        rsfd(path_join(path, de->d_name), spe);
        break;
      case DT_REG:
        rsfd(path_join(path, de->d_name), spe);
        break;
      default:
        continue;
        break;
    }
  }

  closedir(dir);
  return 0;
}

int is_dir(char *path) {
  struct stat st;
  int res;
  res = stat(path, &st);
  return res == 0 ? S_ISDIR(st.st_mode) : res;
}

int is_reg(char *path) {
  struct stat st;
  int res;
  res = stat(path, &st);
  return res == 0 ? S_ISREG(st.st_mode) : res;
}

int rm_invaild_chars(char *des, char *src) {
  wchar_t *ws = (wchar_t *)malloc(sizeof(char *) * (strlen(src) + 1));
  wchar_t *res_ws = (wchar_t *)malloc(sizeof(char *) * (strlen(src) + 1));
  int res_len = 0;
  int i = 0, j = 0;
  int ws_len = 0;
  int code;

  mbstowcs(ws, src, strlen(src));
  ws_len = wcslen(ws);
  for (i = 0; i < ws_len; i++) {
    for (j = 0; j < invaild_chars_len; j++) {
      if (ws[i] == invaild_chars[j]) goto cntn;
    }
    res_ws[res_len++] = ws[i];
    wprintf(L"%lc", ws[i]);
  cntn:
  }
  wcstombs(des, res_ws, 10000);
  wprintf(L"\n%ls\n\n\n", res_ws);
  free(ws);
  free(res_ws);

  return 0;
}

char *path_join(char *base_path, char *path) {
  int len = strlen(base_path) + strlen(path) + 1;
  char *res = (char *)malloc(len * sizeof(char *));

  strcpy(res, base_path);
  strcat(res, "/");
  strcat(res, path);

  return res;
}

// 初始化全局变量
void init(int argc, char *argv[]) {
  switch (argc) {
    case 1:
      // 删除当前目录下所有文件int code;
      // 删除当前目录下所有文件名中的特殊字符，和通过参数传入的字符
      path = ".";
      specific = argv[1];
      break;
    case 3:
      // 删除指定目录下所有文件名中的特殊字符，和通过参数传入的字符
      path = argv[1];
      specific = argv[2];
      break;
    default:
      break;
  }
  setlocale(LC_ALL, "C.UTF-8");
  invaild_chars_len = wcslen(invaild_chars);
}

int main(int argc, char *argv[]) {
  char r[100000] = {};
  init(argc, argv);
  // rsfd(path, specific);

  rm_invaild_chars(r, "[神楽坂 まひろ] - - 01 (B-Global 3840x2160 HEVC AAC MKV) [0B7DC7B6].mkv");
  printf("%s\n", r);

  rm_invaild_chars(r, "[神楽坂 まひろ] - - 02 (B-Global 3840x2160 HEVC AAC MKV) [B997FE12].mkv");
  printf("%s\n", r);

  return 0;
}
