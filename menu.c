#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CHOICES 3

char *choices[MAX_CHOICES] = {
  "Play the Game",
  "Room Editor",
  "Quit"
};

char *commands[MAX_CHOICES] = {
  "./RoguelikeSpaceMarineAI",
  "./MapEditor",
  ""
};

char *title_text = "Roguelike SpaceMarine";
char *menu_text  = "=====================";
char *footer_text = "Choose an option (use 'a' and 'q' to navigate, press enter to select)";

int main() {
  int choice = -1;
  int highlight = 0;
  int ch;

  initscr();
  clear();
  noecho();
  cbreak();
  curs_set(0);


  int max_y, max_x;

  mvprintw(0, 1, "%s", title_text);
  mvprintw(1, 1, "%s", menu_text);
  mvprintw(8, 1, "%s", footer_text);


  while(choice == -1) {
    for(int i = 0; i < MAX_CHOICES; i++) {
      if(i == highlight) {
        attron(A_REVERSE);
      }
      mvprintw(i + 3, 1, "%s", choices[i]);
      attroff(A_REVERSE);
    }

  
    ch = getch();
   switch(ch) {
      case 'a':
        if(highlight > 0) {
          highlight--;
        }
        break;
      case 'q':
        if(highlight < MAX_CHOICES - 1) {
          highlight++;
        }
        break;
      case 10: // Enter key
        choice = highlight;
        break;
      default:
        break;
    }
  }

  endwin();


  if(choice < MAX_CHOICES - 1) {
    char *command = (char *)malloc(strlen(commands[choice]) + 1);
    strcpy(command, commands[choice]);
    system(command);
    free(command);
  }

  return 0;
}

