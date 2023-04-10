#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>

#define ROOM_WIDTH 60
#define ROOM_HEIGHT 20
#define MAX_ROOM_SIZE 1024

#define MAX_OBJECTS 10
#define MAX_TARGETS 2
#define MAX_ENEMIES 3
#define MAX_CRATES 2

#define PLAYER_HP 20
#define ENEMY_HP 10
#define ENEMY_DAMAGE 8

#define MAP_WIDTH 3
#define MAP_HEIGHT 3

#define OFFSET_GAME_WIN_X 1
#define OFFSET_GAME_WIN_Y 1

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
  int room;
} player;


typedef struct {
  position pos;
  char symbol;
  int hp;
  int damage;
  int room;
} enemy;

typedef struct {
  position pos;
  char symbol;
  int room;
} target;


typedef struct {
  position pos;
  char symbol;
  int room;
} crate;


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

void draw_room(WINDOW *game_win, room* m) {
  for (int y = 0; y < ROOM_HEIGHT; y++) {
    for (int x = 0; x < ROOM_WIDTH; x++) {
      mvwaddch(game_win, OFFSET_GAME_WIN_Y + y, OFFSET_GAME_WIN_X + x, m->data[y][x]);
    }
  }

  for (int i = 0; i < m->num_objects; i++) {
    mvwaddch(game_win, OFFSET_GAME_WIN_Y + m->objects[i].pos.y, OFFSET_GAME_WIN_X + m->objects[i].pos.x, m->objects[i].symbol);
  }
}

void draw_player(WINDOW *game_win,player* p) {
  mvwaddch(game_win, OFFSET_GAME_WIN_Y + p->pos.y, OFFSET_GAME_WIN_X + p->pos.x, p->symbol);
}


void draw_enemy(WINDOW *game_win,enemy* e) {
  mvwaddch(game_win, OFFSET_GAME_WIN_Y + e->pos.y, OFFSET_GAME_WIN_X + e->pos.x, e->symbol);
}

void draw_target(WINDOW *game_win, target* t) {
  mvwaddch(game_win, OFFSET_GAME_WIN_Y + t->pos.y, OFFSET_GAME_WIN_X + t->pos.x, t->symbol);
}

void draw_crate(WINDOW *game_win, crate* c) {
  mvwaddch(game_win, OFFSET_GAME_WIN_Y + c->pos.y, OFFSET_GAME_WIN_X + c->pos.x, c->symbol);
}



void move_enemy(enemy* e, room* m) {
  int direction = rand() % 4;
  switch (direction) {
    case 0: // up
      if (e->pos.y > 0 && m->data[e->pos.y - 1][e->pos.x] != '#') {
        e->pos.y--;
      }
      break;
    case 1: // down
      if (e->pos.y < MAP_HEIGHT - 1 && m->data[e->pos.y + 1][e->pos.x] != '#') {
        e->pos.y++;
      }
      break;
    case 2: // left
      if (e->pos.x > 0 && m->data[e->pos.y][e->pos.x - 1] != '#') {
        e->pos.x--;
      }
      break;
    case 3: // right
      if (e->pos.x < MAP_WIDTH - 1 && m->data[e->pos.y][e->pos.x + 1] != '#') {
        e->pos.x++;
      }
      break;
  }
}

int combat(WINDOW *game_win, player* p, enemy* e) {
  noecho();
  while (1) {
    mvwprintw(game_win, 1, 1, "Combat phase!\n");
    mvwprintw(game_win, 2, 1, "Player HP: %d\n", p->hp);
    mvwprintw(game_win, 3, 1, "Enemy HP: %d\n", e->hp);
    mvwprintw(game_win, 4, 1, "Press any key to attack!");

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
    wrefresh(game_win);
  }
}

int check_for_enemy(player* p, enemy* e, int current_room) {
  if (p->pos.x == e->pos.x && p->pos.y == e->pos.y && e->room == current_room) {
    return 1;
  }
  return 0;
}

void init_player_from_file(const char* filename, player* p) {
  FILE* fp;
  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Failed to open file: %s\n", filename);
    exit(1);
  }
  fscanf(fp, "%d %d %c %s %d %d", &(p->pos.x), &(p->pos.y), &(p->symbol), p->name, &(p->hp), &(p->room));
  fclose(fp);
}


void init_enemies_from_file(const char* filename, enemy* e, int num_enemies) {
  FILE* fp;
  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Failed to open file: %s\n", filename);
    exit(1);
  }
  for (int i = 0; i < num_enemies; i++) {
    fscanf(fp, "%d %d %c %d %d %d", &e[i].pos.x, &e[i].pos.y, &e[i].symbol, &e[i].hp, &e[i].damage, &e[i].room);
  }
  fclose(fp);
}


void init_targets_from_file(const char* filename, target* t, int num_targets) {
  FILE* fp;
  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Failed to open file: %s\n", filename);
    exit(1);
  }
  for (int i = 0; i < num_targets; i++) {
    fscanf(fp, "%d %d %c %d", &t[i].pos.x, &t[i].pos.y, &t[i].symbol, &t[i].room);
  }
  fclose(fp);
}


void init_crates_from_file(const char* filename, crate* c, int num_crates) {
  FILE* fp;
  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Failed to open file: %s\n", filename);
    exit(1);
  }
  for (int i = 0; i < num_crates; i++) {
    fscanf(fp, "%d %d %c %d", &c[i].pos.x, &c[i].pos.y, &c[i].symbol, &c[i].room);
  }
  fclose(fp);
}


void clear_window(WINDOW *win, int y, int x) {
  int row, col;
  for (row = 0; row < y; row++) {
    for (col = 0; col < x; col++) {
      mvwaddch(win, OFFSET_GAME_WIN_Y + row, OFFSET_GAME_WIN_Y + col, ' ');
    }
  }
  wrefresh(win);
}


int main() {
  WINDOW *mainwin, *game_win, *info_win, *side_win;

  int can_go = 0;

  int map_width = 60, map_height = 20;
  int char_x = map_width / 2, char_y = map_height / 2;

  if ((mainwin = initscr()) == NULL) {
    fprintf(stderr, "Error initialising ncurses.\n");
    exit(EXIT_FAILURE);
  }

  raw();
  keypad(mainwin, TRUE);
  noecho();
  curs_set(0);


  game_win = subwin(mainwin, map_height+2, map_width+2, 0, 0);
  box(game_win, 0, 0);

  info_win = subwin(mainwin, 6, map_width+2, map_height+2, 0);
  box(info_win, 0, 0);

  side_win = subwin(mainwin, map_height+8, 30, 0, map_width+3);
  box(side_win, 0, 0);




  player p;
  char name[50];

  init_player_from_file("player/player.txt", &p);

  bool no_enemies = true;

  wrefresh(mainwin);

  // Welcome Screen
  mvwprintw(info_win, 1, 1, "Roguelike SpaceMarine");
  mvwprintw(info_win, 2, 1, "Quel est votre identification, SpaceMarine ? ");
  echo();
  mvwgetstr(info_win, 3, 1, p.name);
  //noecho();
  wrefresh(info_win);
  getch();

  clear_window(info_win, 3, 50);
  wrefresh(info_win);

  int choice = 0;
  while (1) {
    wrefresh(info_win);

    mvwprintw(info_win, 1, 1, "Roguelike SpaceMarine");
    mvwprintw(info_win, 2, 1, "1. A la guerre !!");
    mvwprintw(info_win, 3, 1, "2. Quitter");
    wrefresh(info_win);
    noecho();
    choice = getch() - '0';

    if (choice == 1) {
      break;
    } else if (choice == 2) {
      endwin();
      return 0;
    }
  }

  clear_window(info_win, 3, 50);
  wrefresh(info_win);

  room m[9];
  enemy e[3];
  target t[2];
  crate c[2];

  load_room("rooms/room0.txt", &m[0]);
  load_room("rooms/room1.txt", &m[1]);
  load_room("rooms/room2.txt", &m[2]);
  load_room("rooms/room3.txt", &m[3]);
  load_room("rooms/room4.txt", &m[4]);
  load_room("rooms/room5.txt", &m[5]);
  load_room("rooms/room6.txt", &m[6]);
  load_room("rooms/room7.txt", &m[7]);
  load_room("rooms/room8.txt", &m[8]);

  // Initialize enemies
  init_enemies_from_file("enemies/enemies.txt", e, MAX_ENEMIES);

  // Initialize targets
  init_targets_from_file("targets/targets.txt", t, MAX_TARGETS);
  init_crates_from_file("crates/crates.txt", c, MAX_CRATES);


  int current_room = p.room;

  while (1) {

    mvwprintw(info_win, 1, 1,  "SpaceMarine: %s", p.name);
    mvwprintw(info_win, 2, 1, "Pos: %d/%d", p.pos.x + 1,p.pos.y + 1); 
    mvwprintw(info_win, 3, 1, "Salle: %d", current_room);

    wrefresh(info_win);


    draw_room(game_win, &m[current_room]);


    for (int i = 0; i < MAX_ENEMIES; i++) {
      if (e[i].room == current_room) {
        draw_enemy(game_win, &e[i]);
      }
    }


    for (int i = 0; i < MAX_TARGETS; i++) {
      if (t[i].room == current_room) {
        draw_target(game_win, &t[i]);
      }
    }

    for (int i = 0; i < MAX_CRATES; i++) {
      if (c[i].room == current_room) {
        draw_crate(game_win, &c[i]);
      }
    }


    draw_player(game_win, &p);

    wrefresh(game_win);


    int ch = getch();
    switch (ch) {
      case KEY_UP:
        if ( p.pos.y == 0) {
          current_room = current_room - MAP_WIDTH;
          p.pos.y = ROOM_HEIGHT - 1;
        }

        no_enemies = true;
        for (int i = 0; i < MAX_ENEMIES; i++) {
          if (check_for_enemy(&p, &e[i], current_room)) {
            no_enemies = false;
            break; 
          }
        }






        can_go = 1; // We don't know if it is okay or not
        if (p.pos.y > 0) { // Is in the map
          if (m[current_room].data[p.pos.y - 1][p.pos.x] == '#' || no_enemies == false) { // There are a wall and enemies just nearby
            can_go = 0;
          } else { // There is not a wall or enemie
            for (int i = 0; i < MAX_CRATES; i++) { // Test all crates
              if (c[i].room == current_room) { // The create is in the same room
                if (p.pos.x == c[i].pos.x) {
                  if (p.pos.y - 1 == c[i].pos.y) {
                    if (m[current_room].data[p.pos.y - 2 ][p.pos.x] == '#') {
                      can_go = 0;
                    } else {
                      if (p.pos.y == 1) {
                        can_go = 0;
                      } else {
                        c[i].pos.y = c[i].pos.y - 1;
                      }                      
                    }
                  }
                } 
              }
          } // Check all crates
        } // Wall detection        
      } // Map border

      if (can_go == 1) {
        p.pos.y--;
      }







        break;
      case KEY_DOWN:
        if (p.pos.y == ROOM_HEIGHT - 1) {
          current_room = current_room + MAP_WIDTH;
          p.pos.y = 0;
        }

        no_enemies = true;
        for (int i = 0; i < MAX_ENEMIES; i++) {
          if (check_for_enemy(&p, &e[i], current_room)) {
            no_enemies = false;
            break; 
          }
        }





        can_go = 1; // We don't know if it is okay or not
        if (p.pos.y < ROOM_HEIGHT - 1) { // Is in the map
          if (m[current_room].data[p.pos.y + 1][p.pos.x] == '#' || no_enemies == false) { // There are a wall and enemies just nearby
            can_go = 0;
          } else { // There is not a wall or enemie
            for (int i = 0; i < MAX_CRATES; i++) { // Test all crates
              if (c[i].room == current_room) { // The create is in the same room
                if (p.pos.x == c[i].pos.x) {
                  if (p.pos.y + 1 == c[i].pos.y) {
                    if (m[current_room].data[p.pos.y + 2][p.pos.x] == '#' ) {
                      can_go = 0;
                    } else {
                      if (p.pos.y == ROOM_HEIGHT - 2) {
                        can_go = 0;
                      } else {
                        c[i].pos.y = c[i].pos.y + 1;
                      }                        
                    }
                  }
                } 
              }
          } // Check all crates
        } // Wall detection        
      } // Map border

      if (can_go == 1) {
        p.pos.y++;
      }




        break;
      case KEY_LEFT:

        if ( p.pos.x == 0) {
          current_room = current_room - 1;
          p.pos.x = ROOM_WIDTH - 1;
        }

        no_enemies = true;
        for (int i = 0; i < MAX_ENEMIES; i++) {
          if (check_for_enemy(&p, &e[i], current_room)) {
            no_enemies = false;
            break; 
          }
        }



        can_go = 1; // We don't know if it is okay or not
        if (p.pos.x > 0) { // Is in the map
          if (m[current_room].data[p.pos.y][p.pos.x - 1] == '#' || no_enemies == false) { // There are a wall and enemies just nearby
            can_go = 0;
          } else { // There is not a wall or enemie
            for (int i = 0; i < MAX_CRATES; i++) { // Test all crates
              if (c[i].room == current_room) { // The create is in the same room
                if (p.pos.y == c[i].pos.y) {
                  if (p.pos.x -1 == c[i].pos.x) {
                    if (m[current_room].data[p.pos.y][p.pos.x - 2] == '#') {
                      can_go = 0;
                    } else {
                      if (p.pos.x == 1) {
                        can_go = 0;
                      } else {
                        c[i].pos.x = c[i].pos.x - 1;                        
                      }        
                    }
                  }
                } 
              }
          } // Check all crates
        } // Wall detection        
      } // Map border

      if (can_go == 1) {
        p.pos.x--;
      }


        break;
      case KEY_RIGHT:
        if (p.pos.x == ROOM_WIDTH - 1) {
          current_room = current_room + 1;
          p.pos.x = 0;
        }

        no_enemies = true;
        for (int i = 0; i < MAX_ENEMIES; i++) {
          if (check_for_enemy(&p, &e[i], current_room)) {
            no_enemies = false;
            break; 
          }
        }


        can_go = 1; // We don't know if it is okay or not
        if (p.pos.x < ROOM_WIDTH - 1) { // Is in the map
          if (m[current_room].data[p.pos.y][p.pos.x + 1] == '#' || no_enemies == false) { // There are a wall and enemies just nearby
            can_go = 0;
          } else { // There is not a wall or enemie
            for (int i = 0; i < MAX_CRATES; i++) { // Test all crates
              if (c[i].room == current_room) { // The create is in the same room
                if (p.pos.y == c[i].pos.y) {
                  if (p.pos.x + 1 == c[i].pos.x) {
                    if (m[current_room].data[p.pos.y][p.pos.x + 2] == '#') {
                      can_go = 0;
                    } else {
                      if (p.pos.x == ROOM_WIDTH - 2) {
                        can_go = 0;
                      } else {
                        c[i].pos.x = c[i].pos.x + 1;                        
                      }  

                                     
                    }
                  }
                } 
              }
          } // Check all crates
        } // Wall detection        
      } // Map border

      if (can_go == 1) {
        p.pos.x++;
      }

    break;
    case 'q':
    endwin();
    exit(0);
    break;
  }


  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (e[i].room == current_room) {
      move_enemy(&e[i], &m[current_room]);
    }
    if (check_for_enemy(&p, &e[i], current_room)) {
      combat(game_win, &p, &e[i]);
      if (e[i].hp <= 0) {
        // Enemy display outside the screen
        e[i].pos.x = -999;
        e[i].pos.y = -999;
      }
    }   
  }

}


wrefresh(game_win);
wrefresh(side_win);
wrefresh(info_win);
endwin();
return 0;
}
