CC = gcc
CFLAGS = -I ./ -std=c99 -Wall -Wextra -O0 -g
OBJS = number.o lexer.o parser.o display.o main.o

.PHONY: all clean

all: $(OBJS) build

$(OBJS): %.o: %.c

build: $(OBJS)
	$(CC) $(CFLAGS) -o main $^

clean:
	rm -f *.o
