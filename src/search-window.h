extern struct nmr_notmuch_thread ** threads;
extern int thread_count; 
extern int expected_threads;

void nmr_search_draw_line(int line);
int nmr_search_action_down(void *c);
int nmr_search_action_up(void *c);
