CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lncurses -lpthread

all: watchpet_server watchpet_dash watchpet_workspace

watchpet_server: watchpet_server.c
	$(CC) $(CFLAGS) -o watchpet_server watchpet_server.c -lpthread

watchpet_dash: watchpet_dash.c
	$(CC) $(CFLAGS) -o watchpet_dash watchpet_dash.c $(LDFLAGS)

watchpet_workspace: watchpet_workspace.c
	$(CC) $(CFLAGS) -o watchpet_workspace watchpet_workspace.c

clean:
	rm -f watchpet_server watchpet_dash watchpet_workspace /tmp/watch_pet_*
