#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_X 40
#define MAX_Y 40

#define STEP 1

// 蛇的身体组成部分
typedef struct snake {
  int x;
  int y;
  struct snake *next;
} Snake;

// 每个格子可以包含的内容
typedef enum { C_EMPTY, C_SNAKE, C_FOOD, C_WALL_V, C_WALL_H } CONTX;

// 地图新息
CONTX map[MAX_Y][MAX_X];

// 上一次安健的方向
int wasd = KEY_LEFT;

// 蛇
Snake *snk;

// 初始化 ncurses 库
void initNcurses() {
  initscr();
  keypad(stdscr, 1);
}

// 初始化地图
void initMap() {
  for (int x = 0; x < MAX_X; x++) {
    for (int y = 0; y < MAX_Y; y++) {
      if (x == 0 || x == MAX_X - 1) {
        map[x][y] = C_WALL_V;
        continue;
      }
      if (y == 0 || y == MAX_Y - 1) {
        map[x][y] = C_WALL_H;
        continue;
      }
    }
  }
  map[15][15] = C_FOOD;
}

// 初始化蛇
void initSnake() {
  snk = (Snake *)malloc(sizeof(Snake *));
  snk->x = 16;
  snk->y = 10;
  map[10][16] = C_SNAKE;

  Snake *tail = snk;
  for (int i = 1; i < 4; i++) {
    Snake *s = (Snake *)malloc(sizeof(Snake *));
    s->x = tail->x + 1;
    s->y = tail->y;
    tail->next = s;
    tail = s;
    map[s->y][s->x] = C_SNAKE;
  }
}

// 初始化
void init() {
  initNcurses();
  initMap();
  initSnake();
}

void genFood() {
  int rx = rand() % MAX_X;
  int ry = rand() % MAX_Y;
  map[ry][rx] = C_FOOD;
}

// 绘制画面，包括地图，食物，蛇
void draw() {
  move(0, 0);
  for (int x = 0; x < MAX_X; x++) {
    for (int y = 0; y < MAX_Y; y++) {
      switch (map[x][y]) {
        case C_FOOD:
          printw("%s", "##");
          break;
        case C_WALL_H:
          printw("%s", "|");
          break;
        case C_WALL_V:
          printw("%s", "--");
          break;
        case C_SNAKE:
          printw("%s", "[]");
          break;
        case C_EMPTY:
          printw("%s", "  ");
          break;

        default:
          break;
      }
    }
    printw("\n");
  }
  printw("have a good time!!!\n");
  refresh();
}

// 蛇的移动 wasd 表示移动方向
// 1-向上2-向下 3-向左 4-向右
int moveSnake() {
  int head_x = 0;
  int head_y = 0;
  switch (wasd) {
    case KEY_UP:
      head_y = snk->y - 1;
      head_x = snk->x;
      break;
    case KEY_DOWN:
      head_y = snk->y + 1;
      head_x = snk->x;
      break;
    case KEY_LEFT:
      head_x = snk->x - 1;
      head_y = snk->y;
      break;
    case KEY_RIGHT:
      head_x = snk->x + 1;
      head_y = snk->y;
      break;
    default:
      return 0;
  }

  switch (map[head_y][head_x]) {
    case C_EMPTY:
      Snake *p = snk;
      Snake *tail = NULL;
      // 因为蛇的初始长度大于 2，因此此处不考虑 p->next == NULL 的情况
      while (p->next->next != NULL) p = p->next;
      tail = p->next;
      p->next = NULL;
      map[tail->y][tail->x] = C_EMPTY;
      map[head_y][head_x] = C_SNAKE;
      tail->next = snk;
      snk = tail;
      snk->x = head_x;
      snk->y = head_y;
      break;
    case C_FOOD:
      Snake *head = (Snake *)malloc(sizeof(Snake *));
      head->x = head_x;
      head->y = head_y;
      head->next = snk;
      snk = head;
      map[head_y][head_x] = C_SNAKE;
      // 生成新的食物
      genFood();
      break;
    case C_SNAKE:
      return 0;
    case C_WALL_H:
    case C_WALL_V:
      return -1;
      break;
    default:
      break;
  }
  return 0;
}

void keepMvSnake() {
  while (1) {
    moveSnake();
    draw();
    usleep(1000 * 500);
  }
}

int main(int argc, char const *argv[]) {
  init();
  draw();

  pthread_t *mvSnake;
  int status = pthread_create(mvSnake, NULL, (void *)&keepMvSnake, NULL);
  if (status != 0) {
    printw("create pthread faild!, result code: %d", status);
    exit(1);
  }

  while (1) {
    wasd = getch();
  }

  endwin();
  return 0;
}