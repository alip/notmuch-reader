notmuchreader: main.o config.o
	$(CC) main.o config.o -o notmuchreader -lcurses
