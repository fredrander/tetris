EXECUTABLE=tetris
CC=gcc
CFLAGS=-c -Wall -pedantic -std=c99
LDFLAGS=

SRC_PATH=../src

SOURCES=$(SRC_PATH)/main.c \
	$(SRC_PATH)/ui.c \
	$(SRC_PATH)/game.c \
	$(SRC_PATH)/highscore.c \
	$(SRC_PATH)/board.c

OBJECTS=$(SOURCES:.c=.o)

LIBS=-lSDL -lSDL_ttf


all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(SRC_PATH)/*.o
	rm -f $(EXECUTABLE)

