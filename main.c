#include <sys/select.h>
#include <curses.h>
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <locale.h>

#include "config.h"
#include "notmuch.h"
#include "search_window.h"
#include "status_bar.h"

struct nmr_notmuch_thread **threads = NULL;
int thread_count = 0;

static search_window_t search_window;
static status_bar_t status_bar;

/* Actions */
int action_quit(void * c_) {
    return 1;
}

int action_next_line(void * c_){
    if (search_window.current < thread_count - 1) {
        ++search_window.current;
        search_window_draw_line(&search_window, search_window.current);
        search_window_draw_line(&search_window, search_window.current - 1);
        status_bar_message(&status_bar, "tag:inbox %i/%i", search_window.current, thread_count);
    }
    return 0;
}

int action_previous_line(void * c_) {
    if (search_window.current > 0) {
        --search_window.current;
        search_window_draw_line(&search_window, search_window.current);
        search_window_draw_line(&search_window, search_window.current + 1);
        status_bar_message(&status_bar, "tag:inbox %i/%i", search_window.current, thread_count);
    }
    return 0;
}

static void cleanup() {
    search_window_deinit(&search_window);
    status_bar_deinit(&status_bar);
    endwin();
}

static struct nmr_notmuch nm;

void nmr_callback(struct nmr_notmuch_thread *t) {
    thread_count++;

    if (!threads)
        threads = malloc (sizeof(struct nmr_thread *));
    else
        threads = realloc(threads, sizeof(struct nmr_thread *) * thread_count);

    threads[thread_count - 1] = t;
    search_window_draw_line(&search_window, thread_count - 1);
    status_bar_message(&status_bar, "tag:inbox %i/%i", search_window.current, thread_count);
    refresh();
}

int main(int argc,char**argv){
    initscr();
    curs_set(0);
    start_color();
    use_default_colors();
    assume_default_colors(-4,-1);
    config_init();

    clear();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    typeahead(-1);

    search_window_init(&search_window);
    status_bar_init(&status_bar, COLS, LINES - 1, 0);

    search_window_draw_line(&search_window, 0);
    status_bar_message(&status_bar, "tag:inbox %i/%i", search_window.current, thread_count);
    refresh();

    struct actions a;
    a.quit = action_quit;
    a.previous_line = action_previous_line;
    a.next_line = action_next_line;

    nm.callback = &nmr_callback;
    nmr_notmuch_search(&nm, "tag:inbox");

    fd_set rfds;
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
    fcntl(fileno(nm.p), F_SETFL, fcntl(fileno(nm.p), F_GETFL) | O_NONBLOCK);
    for(;;) {
        int maxfd=0;
        FD_ZERO (&rfds);
        FD_SET  (0,&rfds);
        if(nm.p!=NULL){
            FD_SET  (fileno(nm.p),&rfds);
            maxfd=fileno(nm.p);
        }

        if (select(maxfd+2, &rfds, NULL, NULL, NULL) < 0){
            perror("select");
            exit(1);
        }
        if(FD_ISSET(0, &rfds)){
            int c=getch();
            if(config_handle_key_action(c, &a, 0)) {
                break;
            }
            refresh();
        }
        if(nm.p != NULL && FD_ISSET(fileno(nm.p), &rfds)){
            nmr_notmuch_activate(&nm);
        }

    }
    cleanup();
    return 0;
}
