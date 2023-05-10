#include <wchar.h>

// 移除 src 中的特殊字符特殊字符, 特殊字符由 invaild_chars 所指定
int rm_invaild_chars(char *des, char *src);

// 移除 src 中的 spe 子串, 并返回移除后的结果
char *rrm_specific_chars(char *src, char *spe);

// 路径拼接
char *path_join(char *base_path, char *path);

// 删除文件中的特殊符号和指定符号, 返回值 1 表示成功, 0 表示失败
int rsfd(char *file_path, char *spe);

// 判断 path 是否指向文件夹，是则返回 1, 否则返回 0
int is_dir(char *path);

// 判断 path 是否指向普通文件，是则返回 1, 否则返回 0
int is_reg(char *path);