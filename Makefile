all: notmuchreader

notmuchreader: main.o config.o notmuch.o JSON_parser.o search-window.o
	$(CC) $^ -o $@  -lcursesw

%.o:%.c
	$(CC) -D_POSIX_C_SOURCE=2 $(CFLAGS) -c -std=c99 $^ -o $@ 

clean: 
	rm *.o
