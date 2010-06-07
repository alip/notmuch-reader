all: notmuchreader
OBJECTS=main.o config.o notmuch.o search-window.o \
		3rdparty/JSON_parser.o \
		3rdparty/iniparser3.0b/src/dictionary.o \
		3rdparty/iniparser3.0b/src/iniparser.o



CFLAGS+=-Wall
CFLAGS+=-I3rdparty

notmuchreader: $(OBJECTS)
	$(CC) $^ -o $@  -lcursesw

%.o:%.c
	$(CC) -D_POSIX_C_SOURCE=2 $(CFLAGS) -c -std=c99 $^ -o $@ 

clean: 
	rm *.o
