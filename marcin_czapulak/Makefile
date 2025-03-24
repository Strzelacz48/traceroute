CC = gcc
CFLAGS = -Wall -Wextra -O2
SRC = main.c send.c recive.c
OBJ = $(SRC:.c=.o)
TARGET = traceroute

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c header.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
