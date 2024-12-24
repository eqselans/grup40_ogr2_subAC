CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
LDFLAGS =

SRCDIR = src
INCDIR = include
LIBDIR = lib
BINDIR = bin

TARGET = $(BINDIR)/mysh

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(LIBDIR)/%.o, $(SOURCES))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

$(LIBDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(LIBDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(LIBDIR)/*.o $(BINDIR)/mysh

.PHONY: all clean