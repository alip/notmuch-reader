#include <ncurses.h>

int config_init();

const char * config_notmuch_call();

struct actions{
    int (*quit)(void * context);
    int (*next_line)(void * context);
    int (*previous_line)( void * context);
};
int config_handle_key_action(int key,struct actions * actions, void * context);

enum Face{
    Line,
    LineSelected,
    StatusWin,
    Tag,
    TagSelected
};

void config_face_attr(enum Face f,int set);

void config_set_face_attr(WINDOW *w, enum Face f, int set);
