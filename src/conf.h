#ifndef NMR_GUARD_CONF_H
#define NMR_GUARD_CONF_H 1

int config_init(void);

const char *config_notmuch_call(void);

struct actions{
    int (*quit)(void * context);
    int (*up)(void * context);
    int (*down)( void * context);
};
int config_handle_key_action(int key,struct actions * actions, void * context);

enum Face{
    Line,
    LineSelected,
    StatusWin,
    Tag,
    TagSelected,
    Authors
};

void config_set_face_attr(enum Face f,int set);

#endif /* !NMR_GUARD_CONF_H */
