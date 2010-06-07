#include <stdlib.h>
#include <stdio.h>
#include "JSON_parser.h"

struct nmr_notmuch_thread{
    char * thread_id;
    int timestamp;
    int matched;
    int total;
    char * authors;
    char * subject;
    char ** tags;
    int tags_l;
};

struct nmr_notmuch{
    int type;
    FILE * p;
    struct JSON_parser_struct* jc;
    int state;
    void (*callback)(struct nmr_notmuch_thread*);
    struct nmr_notmuch_thread * next_thread;
};

extern struct nmr_notmuch_thread **threads;
extern int thread_count;

int nmr_notmuch_search(struct nmr_notmuch *, const char * query);
int nmr_notmuch_activate(struct nmr_notmuch *);

