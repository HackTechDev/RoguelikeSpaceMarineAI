#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define ROOM_WIDTH 60
#define ROOM_HEIGHT 20
#define MAX_ROOM_SIZE 1024

char room[ROOM_HEIGHT][ROOM_WIDTH];

void load_room(const char* filename) {
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
    strncpy(room[i], buffer, ROOM_WIDTH);
    i++;
  }

  fclose(fp);
}

int main() {
  // Initialize curses
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(0);

  // Allocate memory for the room

  memset(room, ' ', sizeof(room));


  load_room("room.txt"); 

  // Draw the initial room
  for (int y = 0; y < ROOM_HEIGHT; y++) {
    for (int x = 0; x < ROOM_WIDTH; x++) {
      mvaddch(y, x, room[y][x]);
    }
  }

  // Draw the instructions
  mvprintw(ROOM_HEIGHT + 1, 0, "Press arrow keys to move, 'w' to add a wall, and 's' to save and exit.");

  // Initialize the cursor position
  int cursor_x = 0;
  int cursor_y = 0;
  mvaddch(cursor_y, cursor_x, '#');

  // Enter the main loop
  while (1) {
    // Get user input
    int key = getch();

    // Move the cursor based on user input
    switch (key) {
      case KEY_UP:
        if (cursor_y > 0) {
          mvaddch(cursor_y, cursor_x, room[cursor_y][cursor_x]);
          cursor_y--;
          mvaddch(cursor_y, cursor_x, '#');
        }
        break;
      case KEY_DOWN:
        if (cursor_y < ROOM_HEIGHT - 1) {
          mvaddch(cursor_y, cursor_x, room[cursor_y][cursor_x]);
          cursor_y++;
          mvaddch(cursor_y, cursor_x, '#');
        }
        break;
      case KEY_LEFT:
        if (cursor_x > 0) {
          mvaddch(cursor_y, cursor_x, room[cursor_y][cursor_x]);
          cursor_x--;
          mvaddch(cursor_y, cursor_x, '#');
        }
        break;
      case KEY_RIGHT:
        if (cursor_x < ROOM_WIDTH - 1) {
          mvaddch(cursor_y, cursor_x, room[cursor_y][cursor_x]);
          cursor_x++;
          mvaddch(cursor_y, cursor_x, '#');
        }
        break;
      case 'w':
        room[cursor_y][cursor_x] = '#';
        mvaddch(cursor_y, cursor_x, '#');
        break;
      case 'o':
        room[cursor_y][cursor_x] = '*';
        mvaddch(cursor_y, cursor_x, '*');
        break;
      case 'q':
        // Exit the program
        endwin();
        return 0;  
        break;
      case 'l':
        char filename[256];
        echo();
        mvprintw(ROOM_HEIGHT + 2, 0, "Enter filename to load: ");
        getstr(filename);
        noecho();
        // Load the room from a file
        load_room(filename);
        // Redraw the room on the screen
        for (int y = 0; y < ROOM_HEIGHT; y++) {
          for (int x = 0; x < ROOM_WIDTH; x++) {
            mvaddch(y, x, room[y][x]);
          }
        }
        // Move the cursor back to its original position
        cursor_x = 0;
        cursor_y = 0;
        mvaddch(cursor_y, cursor_x, '#');
        break;
      case 's':
        // Save the room to a file
        FILE *file = fopen("room.txt", "w");
        for (int y = 0; y < ROOM_HEIGHT; y++) {
          fwrite(room[y], sizeof(char), ROOM_WIDTH, file);
          fwrite("\n", sizeof(char), 1, file);
        }
        fclose(file);
        break;
    }
  }
}

