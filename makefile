TARGET?=gomoku
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
CC?=gcc
CFLAGS?= -Wall -std=c11 -O3 $(shell pkg-config gtk+-3.0 --cflags)
LDLIBS= $(shell pkg-config gtk+-3.0 --libs)

.PHONY: all install clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDLIBS)


install:
	@-install -m 755 $(TARGET)

clean:
	@-rm -f $(TARGET) $(OBJ) AtoB BtoA

run: $(TARGET)
	./$(TARGET)

main.o: engine.h engine.o graphics.h graphics.o
engine.o: engine.h
graphics.o: graphics.h engine.o engine.h
