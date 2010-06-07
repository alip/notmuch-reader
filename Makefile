all: notmuchreader

notmuchreader: main.o config.o notmuch.o JSON_parser.o
	$(CC) $^ -o $@  -lcursesw

%.o:%.c
	$(CC) -D_POSIX_SOURCE $(CFLAGS) -c -std=c99 $^ -o $@ 

clean: 
	rm *.o
