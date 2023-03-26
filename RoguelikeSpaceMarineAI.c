#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>

#define ROOM_WIDTH 60
#define ROOM_HEIGHT 20
#define MAX_ROOM_SIZE 1024
#define MAX_OBJECTS 10

#define MAX_ENEMIES 10
#define PLAYER_HP 20
#define ENEMY_HP 10
#define ENEMY_DAMAGE 8

#define MAP_WIDTH 3
#define MAP_HEIGHT 3

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
  char name[20];
  int hp;
} player;


typedef struct {
  position pos;
  char symbol;
  int hp;
  int damage;
} enemy;


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


void draw_enemy(enemy* e, int i) {
  mvaddch(e[i].pos.y, e[i].pos.x, e[i].symbol);
}

int combat(player* p, enemy* e) {
  while (1) {
    clear();
    mvprintw(0, 0, "Combat phase!\n");
    mvprintw(1, 0, "Player HP: %d\n", p->hp);
    mvprintw(2, 0, "Enemy HP: %d\n", e->hp);
    mvprintw(3, 0, "Press any key to attack!");

    int combat_ch = getch();
    int player_damage = rand() % 5 + 1;
    int enemy_damage = rand() % 3 + 1;
    if (combat_ch != ERR) {
      e->hp -= player_damage;
      if (e->hp <= 0) {
        return 1;
      }
      p->hp -= enemy_damage;
      if (p->hp <= 0) {
        return -1;
      }
    }
    refresh();
  }
}

int check_for_enemy(player* p, enemy* e) {
  if (p->pos.x == e->pos.x && p->pos.y == e->pos.y) {
    return 1;
  }
  return 0;
}


int main() {
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(0);

  player p = {{10, 10}, '@', "", PLAYER_HP};
  char name[50];

  // Welcome Screen
  printw("Roguelike\n\n");
  printw("Quel est ton nom, aventurier ? ");
  refresh();
  echo();
  getstr(p.name);
  noecho();


  int choice = 0;
  while (1) {
    clear();

    printw("Roguelike\n\n");
    printw("1. Partons à l'aventure !!\n");
    printw("2. Quitter\n");
    refresh();

    choice = getch() - '0';

    if (choice == 1) {
      break;
    } else if (choice == 2) {
      endwin();
      return 0;
    }
  }




  room m[9];

  int current_room = 0;

  load_room("room0.txt", &m[0]);
  load_room("room1.txt", &m[1]);
  load_room("room2.txt", &m[2]);
  load_room("room3.txt", &m[3]);
  load_room("room4.txt", &m[4]);
  load_room("room5.txt", &m[5]);
  load_room("room6.txt", &m[6]);
  load_room("room7.txt", &m[7]);
  load_room("room8.txt", &m[8]);


  // Initialize enemies
  enemy e1 = {{20, 5}, 'E', ENEMY_HP, ENEMY_DAMAGE};
  enemy e2 = {{40, 15}, 'E', ENEMY_HP, ENEMY_DAMAGE};
  enemy e3 = {{5, 18}, 'E', ENEMY_HP, ENEMY_DAMAGE};


  while (1) {
    clear();

    mvprintw(ROOM_HEIGHT, 0,  "# SpaceMarine: %s", p.name);
    mvprintw(ROOM_HEIGHT, 30, "# Pos: %d/%d", p.pos.x + 1,p.pos.y + 1); 
    mvprintw(ROOM_HEIGHT, 45, "# Salle: %d", current_room);
    mvprintw(ROOM_HEIGHT, 59, "#");
    mvprintw(ROOM_HEIGHT + 1, 0, "############################################################");

    draw_room(&m[current_room]);
    draw_player(&p);

    if (current_room == 0) {
      draw_enemy(&e1, 0);
      draw_enemy(&e2, 0);
      draw_enemy(&e3, 0);
    }

    int ch = getch();
    switch (ch) {
      case KEY_UP:
        if ( p.pos.y == 0) {
          current_room = current_room - MAP_WIDTH;
          p.pos.y = ROOM_HEIGHT - 1;
        }

        if (p.pos.y > 0 && m[current_room].data[p.pos.y - 1][p.pos.x] != '#' && !check_for_enemy(&p, &e1) && !check_for_enemy(&p, &e2) && !check_for_enemy(&p, &e3)) {
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
        if (p.pos.y == ROOM_HEIGHT - 1) {
          current_room = current_room + MAP_WIDTH;
          p.pos.y = 0;
        }

        if (p.pos.y < ROOM_HEIGHT - 1 && m[current_room].data[p.pos.y + 1][p.pos.x] != '#' && !check_for_enemy(&p, &e1) && !check_for_enemy(&p, &e2) && !check_for_enemy(&p, &e3)) {
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

        if ( p.pos.x == 0) {
          current_room = current_room - 1;
          p.pos.x = ROOM_WIDTH - 1;
        }

        if (p.pos.x > 0 && m[current_room].data[p.pos.y][p.pos.x - 1] != '#' && !check_for_enemy(&p, &e1) && !check_for_enemy(&p, &e2) && !check_for_enemy(&p, &e3)) {
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
        if (p.pos.x == ROOM_WIDTH - 1) {
          current_room = current_room + 1;
          p.pos.x = 0;
        }
        if (p.pos.x < ROOM_WIDTH - 1 && m[current_room].data[p.pos.y][p.pos.x + 1] != '#' && !check_for_enemy(&p, &e1) && !check_for_enemy(&p, &e2) && !check_for_enemy(&p, &e3)) {
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


    if (current_room == 0) {
      int direction = rand() % 4;
      switch (direction) {
        case 0: // up
          if (e1.pos.y > 0 && m[0].data[e1.pos.y - 1][e1.pos.x] != '#') {
            e1.pos.y--;
          }
          break;
        case 1: // down
          if (e1.pos.y < MAP_HEIGHT - 1 && m[0].data[e1.pos.y + 1][e1.pos.x] != '#') {
            e1.pos.y++;
          }
          break;
        case 2: // left
          if (e1.pos.x > 0 && m[0].data[e1.pos.y][e1.pos.x - 1] != '#') {
            e1.pos.x--;
          }
          break;
        case 3: // right
          if (e1.pos.x < MAP_WIDTH - 1 && m[0].data[e1.pos.y][e1.pos.x + 1] != '#') {
            e1.pos.x++;
          }
          break;
      }


      direction = rand() % 4;
      switch (direction) {
        case 0: // up
          if (e2.pos.y > 0 && m[0].data[e1.pos.y - 1][e2.pos.x] != '#') {
            e2.pos.y--;
          }
          break;
        case 1: // down
          if (e2.pos.y < MAP_HEIGHT - 1 && m[0].data[e2.pos.y + 1][e2.pos.x] != '#') {
            e2.pos.y++;
          }
          break;
        case 2: // left
          if (e2.pos.x > 0 && m[0].data[e2.pos.y][e2.pos.x - 1] != '#') {
            e2.pos.x--;
          }
          break;
        case 3: // right
          if (e2.pos.x < MAP_WIDTH - 1 && m[0].data[e2.pos.y][e2.pos.x + 1] != '#') {
            e2.pos.x++;
          }
          break;
      }


      direction = rand() % 4;
      switch (direction) {
        case 0: // up
          if (e3.pos.y > 0 && m[0].data[e3.pos.y - 1][e3.pos.x] != '#') {
            e3.pos.y--;
          }
          break;
        case 1: // down
          if (e3.pos.y < MAP_HEIGHT - 1 && m[0].data[e3.pos.y + 1][e3.pos.x] != '#') {
            e3.pos.y++;
          }
          break;
        case 2: // left
          if (e3.pos.x > 0 && m[0].data[e3.pos.y][e3.pos.x - 1] != '#') {
            e3.pos.x--;
          }
          break;
        case 3: // right
          if (e3.pos.x < MAP_WIDTH - 1 && m[0].data[e3.pos.y][e3.pos.x + 1] != '#') {
            e3.pos.x++;
          }
          break;
      }





      // Check if player is in combat with an enemy
      if (check_for_enemy(&p, &e1)) {
        combat(&p, &e1);
        if (e1.hp <= 0) {
          e1.pos.x = -1;
          e1.pos.y = -1;
        }
      } else if (check_for_enemy(&p, &e2)) {
        combat(&p, &e2);
        if (e2.hp <= 0) {
          e2.pos.x = -1;
          e2.pos.y = -1;
        }
      } else if (check_for_enemy(&p, &e3)) {
        combat(&p, &e3);
        if (e3.hp <= 0) {
          e3.pos.x = -1;
          e3.pos.y = -1;
        }
      }
    }

  }

  endwin();
  return 0;
}

