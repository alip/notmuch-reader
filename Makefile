all: notmuchreader

CFLAGS= -Wall -Wextra

notmuchreader: main.o config.o notmuch.o JSON_parser.o search_window.o status_bar.o
	$(CC) $^ -o $@  -lcurses

%.o: %.c
	$(CC) $(CFLAGS) -c -std=c99 $^ -o $@

clean:
	rm -f *.o || true
