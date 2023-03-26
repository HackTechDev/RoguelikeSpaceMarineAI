CC = gcc
TARGET1 = game
OBJ1 = game.o
SRC1 = game.c
DEPS1 = $(OBJ1)

TARGET2 = editor
OBJ2 = editor.o
SRC2 = editor.c
DEPS2 = $(OBJ2)

TARGET3 = menu
OBJ3 = menu.o
SRC3 = menu.c
DEPS3 = $(OBJ3)

$(OBJ1): $(SRC1)
	$(CC) -c $< -o $@

$(TARGET1): $(DEPS1)
	$(CC) -o $@ $^ -lncurses

$(OBJ2): $(SRC2)
	$(CC) -c $< -o $@

$(TARGET2): $(DEPS2)
	$(CC) -o $@ $^ -lncurses

$(OBJ3): $(SRC3)
	$(CC) -c $< -o $@

$(TARGET3): $(DEPS3)
	$(CC) -o $@ $^ -lncurses

all: $(TARGET1) $(TARGET2) $(TARGET3)

clean:
	rm -f $(OBJ1) $(TARGET1) $(OBJ2) $(TARGET2) $(OBJ3) $(TARGET3)

