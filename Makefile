CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = memSim
OBJS = main.o memSim.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c memSim.h
	$(CC) $(CFLAGS) -c main.c

memSim.o: memSim.c memSim.h
	$(CC) $(CFLAGS) -c memSim.c

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
