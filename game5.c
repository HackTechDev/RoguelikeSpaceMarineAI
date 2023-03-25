#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>

#define MAP_WIDTH 60
#define MAP_HEIGHT 20
#define MAX_MAP_SIZE 1024
#define MAX_OBJECTS 10

typedef struct {
  int x;
  int y;
} position;

typedef struct {
  char data[MAP_HEIGHT][MAP_WIDTH];
  int num_objects;
  struct object {
    position pos;
    char symbol;
  } objects[MAX_OBJECTS];
} map;

typedef struct {
  position pos;
  char symbol;
} player;

void load_map(const char* filename, map* m) {
  FILE* fp;
  char buffer[MAX_MAP_SIZE];

  fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "Failed to open map file: %s\n", filename);
    exit(1);
  }

  int i = 0;
  while (fgets(buffer, MAX_MAP_SIZE, fp)) {
    if (i >= MAP_HEIGHT) {
      fprintf(stderr, "Map file has too many lines\n");
      exit(1);
    }
    if (strlen(buffer) - 1 != MAP_WIDTH) {
      fprintf(stderr, "Map file has incorrect line length\n");
      exit(1);
    }
    strncpy(m->data[i], buffer, MAP_WIDTH);
    i++;
  }

  m->num_objects = 0;

  fclose(fp);
}

void draw_map(map* m) {
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      mvaddch(y, x, m->data[y][x]);
    }
  }

  for (int i = 0; i < m->num_objects; i++) {
    mvaddch(m->objects[i].pos.y, m->objects[i].pos.x, m->objects[i].symbol);
  }
}

void draw_player(player* p) {
  mvaddch(p->pos.y, p->pos.x, p->symbol);
}

int main() {
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(0);

  map m[2];
  player p = {{10, 10}, '@'};

  int current_map = 0;

  load_map("map0.txt", &m[0]);
  load_map("map1.txt", &m[1]);
  load_map("map2.txt", &m[2]);
  load_map("map3.txt", &m[3]);

  while (1) {
    clear();

    draw_map(&m[current_map]);
    draw_player(&p);

    int ch = getch();
    switch (ch) {
      case KEY_UP:
        if ( p.pos.y == 0 && current_map == 2) {
          current_map = 0;
          p.pos.y = MAP_HEIGHT - 1;
        }

        if (p.pos.y > 0 && m[current_map].data[p.pos.y - 1][p.pos.x] != '#') {
          if (m[current_map].data[p.pos.y - 1][p.pos.x] == '*') {
            if (p.pos.y > 1 && m[current_map].data[p.pos.y - 2][p.pos.x] != '#' && m[current_map].data[p.pos.y - 2][p.pos.x] != '*') {
              m[current_map].data[p.pos.y - 2][p.pos.x] = '*';
              m[current_map].data[p.pos.y - 1][p.pos.x] = ' ';
              p.pos.y--;
            }
          }
          else {
            p.pos.y--;
          }
        }
        break;
      case KEY_DOWN:
        if (p.pos.y == MAP_HEIGHT - 1 && current_map == 0) {
          current_map = 2;
          p.pos.y = 0;
        }

        if (p.pos.y < MAP_HEIGHT - 1 && m[current_map].data[p.pos.y + 1][p.pos.x] != '#') {
          if (m[current_map].data[p.pos.y + 1][p.pos.x] == '*') {
            if (p.pos.y < MAP_HEIGHT - 2 && m[current_map].data[p.pos.y + 2][p.pos.x] != '#' && m[current_map].data[p.pos.y + 2][p.pos.x] != '*') {
              m[current_map].data[p.pos.y + 2][p.pos.x] = '*';
              m[current_map].data[p.pos.y + 1][p.pos.x] = ' ';
              p.pos.y++;
            }
          }
          else {
            p.pos.y++;
          }
        } 
        break;
      case KEY_LEFT:

        if ( p.pos.x == 0 && current_map == 1) {
          current_map = 0;
          p.pos.x = MAP_WIDTH - 1;
        }

        if (p.pos.x > 0 && m[current_map].data[p.pos.y][p.pos.x - 1] != '#') {
          if (m[current_map].data[p.pos.y][p.pos.x - 1] == '*') {
            if (p.pos.x > 1 && m[current_map].data[p.pos.y][p.pos.x - 2] != '#' && m[current_map].data[p.pos.y][p.pos.x - 2] != '*') {
              m[current_map].data[p.pos.y][p.pos.x - 2] = '*';
              m[current_map].data[p.pos.y][p.pos.x - 1] = ' ';
              p.pos.x--;
            }
          }
          else {
            p.pos.x--;
          }
        }
        break;
      case KEY_RIGHT:
        if (p.pos.x == MAP_WIDTH - 1 && current_map == 0) {
          current_map = 1;
          p.pos.x = 0;
        }
        if (p.pos.x < MAP_WIDTH - 1 && m[current_map].data[p.pos.y][p.pos.x + 1] != '#') {
          if (m[current_map].data[p.pos.y][p.pos.x + 1] == '*') {
            if (p.pos.x < MAP_WIDTH - 2 && m[current_map].data[p.pos.y][p.pos.x + 2] != '#' && m[current_map].data[p.pos.y][p.pos.x + 2] != '*') {
              m[current_map].data[p.pos.y][p.pos.x + 2] = '*';
              m[current_map].data[p.pos.y][p.pos.x + 1] = ' ';
              p.pos.x++;
            }
          }
          else {
            p.pos.x++;
          }
        } 
        break;
      case 'q':
        endwin();
        exit(0);
        break;
    }
  }

  endwin();
  return 0;
}

