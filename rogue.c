#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>

#define ROOM_WIDTH 60
#define ROOM_HEIGHT 20
#define MAX_ROOM_SIZE 1024
#define MAX_OBJECTS 10

typedef struct {
  int x;
  int y;
} position;

typedef struct {
  char data[ROOM_HEIGHT][ROOM_WIDTH];
  int num_objects;
  struct object {
    position pos;
    char symbol;
  } objects[MAX_OBJECTS];
} room;

typedef struct {
  position pos;
  char symbol;
} player;

void load_room(const char* filename, room* m) {
  FILE* fp;
  char buffer[MAX_ROOM_SIZE];

  fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "Failed to open room file: %s\n", filename);
    exit(1);
  }

  int i = 0;
  while (fgets(buffer, MAX_ROOM_SIZE, fp)) {
    if (i >= ROOM_HEIGHT) {
      fprintf(stderr, "Map file has too many lines\n");
      exit(1);
    }
    if (strlen(buffer) - 1 != ROOM_WIDTH) {
      fprintf(stderr, "Map file has incorrect line length\n");
      exit(1);
    }
    strncpy(m->data[i], buffer, ROOM_WIDTH);
    i++;
  }

  m->num_objects = 0;

  fclose(fp);
}

void draw_room(room* m) {
  for (int y = 0; y < ROOM_HEIGHT; y++) {
    for (int x = 0; x < ROOM_WIDTH; x++) {
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

  room m[4];
  player p = {{10, 10}, '@'};

  int current_room = 0;

  load_room("room0.txt", &m[0]);
  load_room("room1.txt", &m[1]);
  load_room("room2.txt", &m[2]);
  load_room("room3.txt", &m[3]);

  while (1) {
    clear();

    draw_room(&m[current_room]);
    draw_player(&p);

    int ch = getch();
    switch (ch) {
      case KEY_UP:
        if ( p.pos.y == 0 && current_room == 2) {
          current_room = 0;
          p.pos.y = ROOM_HEIGHT - 1;
        }

        if (p.pos.y > 0 && m[current_room].data[p.pos.y - 1][p.pos.x] != '#') {
          if (m[current_room].data[p.pos.y - 1][p.pos.x] == '*') {
            if (p.pos.y > 1 && m[current_room].data[p.pos.y - 2][p.pos.x] != '#' && m[current_room].data[p.pos.y - 2][p.pos.x] != '*') {
              m[current_room].data[p.pos.y - 2][p.pos.x] = '*';
              m[current_room].data[p.pos.y - 1][p.pos.x] = ' ';
              p.pos.y--;
            }
          }
          else {
            p.pos.y--;
          }
        }
        break;
      case KEY_DOWN:
        if (p.pos.y == ROOM_HEIGHT - 1 && current_room == 0) {
          current_room = 2;
          p.pos.y = 0;
        }

        if (p.pos.y < ROOM_HEIGHT - 1 && m[current_room].data[p.pos.y + 1][p.pos.x] != '#') {
          if (m[current_room].data[p.pos.y + 1][p.pos.x] == '*') {
            if (p.pos.y < ROOM_HEIGHT - 2 && m[current_room].data[p.pos.y + 2][p.pos.x] != '#' && m[current_room].data[p.pos.y + 2][p.pos.x] != '*') {
              m[current_room].data[p.pos.y + 2][p.pos.x] = '*';
              m[current_room].data[p.pos.y + 1][p.pos.x] = ' ';
              p.pos.y++;
            }
          }
          else {
            p.pos.y++;
          }
        } 
        break;
      case KEY_LEFT:

        if ( p.pos.x == 0 && current_room == 1) {
          current_room = 0;
          p.pos.x = ROOM_WIDTH - 1;
        }

        if (p.pos.x > 0 && m[current_room].data[p.pos.y][p.pos.x - 1] != '#') {
          if (m[current_room].data[p.pos.y][p.pos.x - 1] == '*') {
            if (p.pos.x > 1 && m[current_room].data[p.pos.y][p.pos.x - 2] != '#' && m[current_room].data[p.pos.y][p.pos.x - 2] != '*') {
              m[current_room].data[p.pos.y][p.pos.x - 2] = '*';
              m[current_room].data[p.pos.y][p.pos.x - 1] = ' ';
              p.pos.x--;
            }
          }
          else {
            p.pos.x--;
          }
        }
        break;
      case KEY_RIGHT:
        if (p.pos.x == ROOM_WIDTH - 1 && current_room == 0) {
          current_room = 1;
          p.pos.x = 0;
        }
        if (p.pos.x < ROOM_WIDTH - 1 && m[current_room].data[p.pos.y][p.pos.x + 1] != '#') {
          if (m[current_room].data[p.pos.y][p.pos.x + 1] == '*') {
            if (p.pos.x < ROOM_WIDTH - 2 && m[current_room].data[p.pos.y][p.pos.x + 2] != '#' && m[current_room].data[p.pos.y][p.pos.x + 2] != '*') {
              m[current_room].data[p.pos.y][p.pos.x + 2] = '*';
              m[current_room].data[p.pos.y][p.pos.x + 1] = ' ';
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

