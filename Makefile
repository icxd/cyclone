CC := gcc

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)

TARGET := bin/$(shell basename $(CURDIR))

CFLAGS := -Wall -Wextra -Werror -Wpedantic -std=c99 -g -Wno-unused-parameter
LDFLAGS := -lm

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) $(TARGET)