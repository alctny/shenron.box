#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char const *argv[]) {
  time_t *tr;
  struct tm *now;
  int h, m, s;

  time(tr);
  now = localtime(tr);
  h = now->tm_hour < 12 ? 11 - now->tm_hour : 23 - now->tm_hour;
  m = 59 - now->tm_min;
  s = 59 - now->tm_sec;
  printf("%02d:%02d:%02d\n", h, m, s);
  return 1;
}
