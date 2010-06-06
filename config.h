int config_init();

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
};

void config_set_face_attr(enum Face f,int set);

