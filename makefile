TARGET?=gomoku
OBJ= main.o engine.o graphics.o networking.o
CC?=gcc
CFLAGS?= -Werror -Wall -std=c11 -O3 $(shell pkg-config gtk+-3.0 --cflags)
LDLIBS= $(shell pkg-config gtk+-3.0 --libs)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDLIBS)

clean:
	@-rm -f $(TARGET) $(OBJ)

run: $(TARGET)
	./$(TARGET)

main.o: engine.o graphics.o networking.o
engine.o: engine.h
graphics.o: graphics.h engine.o
networking.o: networking.h