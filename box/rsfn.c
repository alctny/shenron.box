// P.S. 虽然此函数中的方法并不会被抽象为第三方库,但本着函数尽可能要通用的原则
// 所有函数的入参和出参均使用 char/char* 而不使用 wchar_t/wchar_t*

#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wchar.h>

char *path_rename(char *old);  // 移除文件名中的特殊字符
int is_dir(char *path);        // 判断 path 是否指向文件夹，是则返回 1, 否则返回 0
int is_reg(char *path);        // 判断 path 是否指向普通文件，是则返回 1, 否则返回 0
int rsfd(char *file_path, char *spe);  // 删除文件中的特殊符号和指定符号, 返回值 1 表示成功, 0 表示失败
int rm_invaild_chars(char *des, char *src);  // 移除 src 中的特殊字符特殊字符, 特殊字符由 invaild_chars 所指定
char *path_join(char *base_path, char *path);  // 路径拼接
// char *rm_specific_chars(char *src, char *spe);  // 移除 src 中的 spe 子串, 并返回移除后的结果

wchar_t *invaild_chars = L"，。【】（）()[]:：「」,|<>《》][;?\\ \"'－；！";
int invaild_chars_len = 0;
char *path = "";
char *specific = "";

char *path_rename(char *path) {
  char *base_path = dirname(strdup(path));
  char *old_name = basename(strdup(path));
  char *new_name = (char *)malloc(sizeof(char) * strlen(old_name));
  char *new_path;

  if (rm_invaild_chars(new_name, old_name) != 0) {
    return NULL;
  }
  if (strcmp(old_name, new_name) == 0) {
    return path;
  }

  new_path = path_join(base_path, new_name);
  if (rename(path, new_path) != 0) {
    return NULL;
  }
  return new_path;
}

// 隐藏文件视作保护文件，不做不允许对其进行重命名，避免带来不必要的麻烦
int rsfd(char *path, char *spe) {
  DIR *dir;
  struct dirent *de;

  printf("%s\n", path);

  // 有枣没枣打三杆子
  path = path_rename(path);
  if (is_reg(path) == 0) {
    return 0;
  }
  if (!is_dir(path)) {
    return -1;
  }

  dir = opendir(path);
  if (NULL == dir) {
    return -1;
  }

  while ((de = readdir(dir))) {
    if ('.' == *(de->d_name)) continue;
    if (rsfd(path_join(path, de->d_name), spe) != 0) return -1;
  }

  closedir(dir);

  return 0;
}

// 判断路径是不是文件夹
int is_dir(char *path) {
  struct stat st;
  return stat(path, &st) == 0 ? S_ISDIR(st.st_mode) : -1;
}

// 判断路径是不是普通文件
int is_reg(char *path) {
  struct stat st;
  int res;
  res = stat(path, &st);

  return res == 0 ? S_ISREG(st.st_mode) : -1;
}

// 移除字符串中的非法字符
int rm_invaild_chars(char *des, char *src) {
  wchar_t *ws = (wchar_t *)malloc(sizeof(wchar_t) * (strlen(src) + 1));
  wchar_t *res_ws = (wchar_t *)malloc(sizeof(wchar_t) * (strlen(src) + 1));
  int res_len = 0, i = 0, j = 0, ws_len = 0, code = 0;

  mbstowcs(ws, src, strlen(src));
  ws_len = wcslen(ws);
  for (i = 0; i < ws_len; i++) {
    for (j = 0; j < invaild_chars_len; j++) {
      if (ws[i] == invaild_chars[j]) goto cntn;
    }
    res_ws[res_len++] = ws[i];
  cntn:;
  }
  wcstombs(des, res_ws, 10000);
  return 0;
}

// 路径字拼接
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
  int code;
  char *path = "/home/ant/codespace/lwifew/shenron.box/box/test";

  init(argc, argv);
  code = rsfd(path, specific);

  printf("%d\n", code);
  printf("%d\n", is_reg(path));
  return 0;
}
