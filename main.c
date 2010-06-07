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
#include "notmuch.h"
#include <stdio.h>

#include "config.h"
#include "curses.h"
#include "search-window.h"

//drawing


int lines,cols,current_line;


void draw_status_win(){
    config_set_face_attr(StatusWin,1);
    mvwprintw (stdscr,lines-1,0, "tag:inbox");
    wprintw (stdscr, "    (%i/%i)  ",current_line+1,thread_count);
    pad_right(0);
    config_set_face_attr(StatusWin,0);
}

//main

int action_quit(void * c_){
    return 1;
}
static struct nmr_notmuch nm;

void nmr_callback(struct nmr_notmuch_thread* t){
    thread_count++;
    if(threads==NULL){
        threads=malloc(sizeof(void*));
    }else{
        threads=realloc(threads,sizeof(void*)*thread_count);
    }
    threads[thread_count-1]=t;
    nmr_search_draw_line(thread_count-1);
    draw_status_win();
    refresh();
}

static void cleanup(){
    curs_set(1);
    clrtoeol();
    refresh();
    endwin();
}

int main(int argc,char**argv){
    setlocale(LC_ALL,"");

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

    cols=getmaxx(stdscr);
    lines=getmaxy(stdscr);

    draw_status_win();
    nmr_search_draw_line(0);
    refresh();

    struct actions a;
    a.quit=&action_quit;
    a.up=&nmr_search_action_up;
    a.down=&nmr_search_action_down;

    nmr_notmuch_search(&nm,"tag:inbox OR tag:todo");
    nm.callback=&nmr_callback;

    fd_set rfds;
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
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
            if(config_handle_key_action(c,&a,0)){
                break;
            }
            wrefresh(stdscr);
            refresh();
        }
        if(nm.p!=NULL  && FD_ISSET(fileno(nm.p),&rfds)){
            nmr_notmuch_activate(&nm);
            if(expected_threads==0){
                expected_threads=nm.count;
            }
        }

    }
    cleanup();
    return 0;
}
