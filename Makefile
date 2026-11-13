.RECIPEPREFIX = >

CC      = gcc
CFLAGS  = -Wall -Wextra -g -Iinclude
LDFLAGS =
TARGET  = bin/myshell
SRCS    = $(wildcard src/*.c)
OBJS    = $(patsubst src/%.c,obj/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
> @mkdir -p bin
> $(CC) $(OBJS) -o $@ $(LDFLAGS)

obj/%.o: src/%.c include/shell.h
> @mkdir -p obj
> $(CC) $(CFLAGS) -c $< -o $@

clean:
> rm -rf obj bin

.PHONY: all clean
