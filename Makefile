all: notmuchreader notmuchjson

CFLAGS= $(shell pkg-config --cflags jansson)
LDFLAGS= $(shell pkg-config --libs jansson)

notmuchreader: main.o config.o
	$(CC) main.o config.o -o notmuchreader -lcurses
notmuchjson: json.o
	$(CC) json.o $(LDFLAGS) -o notmuchjson
