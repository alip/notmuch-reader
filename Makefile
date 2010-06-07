all: notmuchreader

notmuchreader: main.o config.o notmuch.o JSON_parser.o
	$(CC) $^ -o $@  -lcurses
clean: 
	rm *.o
