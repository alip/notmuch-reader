SUBDIRS= .
AM_CFLAGS= @NMR_CFLAGS@

bin_PROGRAMS= notmuch-reader
noinst_HEADERS=\
	       conf.h \
	       curses.h \
	       notmuch.h \
	       search-window.h

notmuch_reader_SOURCES=\
		       conf.c \
		       notmuch.c \
		       search-window.c \
		       main.c

# JSON_parser
AM_CFLAGS+= -I3rdparty
noinst_HEADERS+= 3rdparty/JSON_parser.h
notmuch_reader_SOURCES+= 3rdparty/JSON_parser.c
