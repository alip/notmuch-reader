all: notmuchreader

notmuchreader: main.o config.o notmuch.o JSON_parser.o
	$(CC) $^ -o $@  -lcurses

%.o:%.c
	$(CC) -c -std=c99 $^ -o $@ 

clean: 
	rm *.o
