#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char const *argv[]) {
  time_t tr;
  struct tm *now;
  int h, m, s;

  tr = time(NULL);
  now = localtime(&tr);
  if (now->tm_hour < 12) {
    h = 11 - now->tm_hour;
  } else if (now->tm_hour < 18) {
    h = 17 - now->tm_hour;
  } else {
    h = 23 - now->tm_hour;
  }
  m = 59 - now->tm_min;
  s = 59 - now->tm_sec;
  printf("%02d:%02d:%02d\n", h, m, s);
  return 1;
}
