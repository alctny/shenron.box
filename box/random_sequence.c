#include <stdio.h>
#include <stdlib.h>

// 生成指定范围内无遗漏无序数列，用来帮助数字记忆宫殿训练
int main(int argc, char const *argv[]) {
  if (argc < 2) {
    printf("rseq <number>");
    exit(1);
  }
  int num = atoi(argv[1]);
  int arr[num];
  for (int i = 0; i < num; i++) arr[i] = i + 1;
  for (int i = 0; i < num; i++) {
    int r = rand() % num;
    if (r == i) continue;
    arr[i] = arr[i] + arr[r];
    arr[r] = arr[i] - arr[r];
    arr[i] = arr[i] - arr[r];
  }
  for (int i = 0; i < num; i++) printf("%d\t", arr[i]);

  return 0;
}
