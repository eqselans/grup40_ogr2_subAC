CC = gcc
CFLAGS = -Wall -Wextra -g
SRC = src/main.c src/commands.c src/redirection.c src/background.c src/pipe.c
INC = -Iinclude

all: shell

shell: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o shell $(INC)

clean:
	rm -f shell
