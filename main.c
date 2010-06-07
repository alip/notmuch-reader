#include <sys/select.h>
#include <curses.h>
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include "config.h"
#include <locale.h>
#include "notmuch.h"


//drawing
static int lines,cols,current_line_selected;
static int fg,bg;

static struct nmr_notmuch_thread ** threads=0;
static int thread_count=0; 

static void pad_right(int to_right){
    int l,c;
    getyx(stdscr, l,c);
    while(c<(cols-to_right)){
        mvaddch(l,c++,' ');
    }
}
static void pad_left(int to_left){
    int l,c;
    getyx(stdscr, l,c);
    while(c<to_left){
        mvaddch(l,c++,' ');
    }
}



static void draw_line(int line){
    if(line>lines-2)
        return;

    if(line==current_line_selected){
        config_set_face_attr(LineSelected,1);
    }else{
        config_set_face_attr(Line,1);
    }

    if(line<thread_count){
        struct nmr_notmuch_thread * th=threads[line];
        mvwprintw (stdscr,line,0, "[%i/%i]",th->matched,th->total);
    }else{
        mvwprintw (stdscr,line,0, "...");
    }

    pad_left(8);
    if(line<thread_count){
        struct nmr_notmuch_thread * th=threads[line];
        wprintw (stdscr, "%s %s  ",th->authors,th->subject);


        wprintw (stdscr, "(");
        if(line==current_line_selected){
            config_set_face_attr(TagSelected,1);
        }else{
            config_set_face_attr(Tag,1);
        }
        for(int i=0;i<th->tags_l;){
            wprintw(stdscr,"%s",th->tags[i]);
            if(++i<th->tags_l){
                wprintw(stdscr," ",th->tags[i]);
            }
        }
        if(line==current_line_selected){
            config_set_face_attr(TagSelected,0);
        }else{
            config_set_face_attr(Tag,0);
        }
        wprintw (stdscr, ")");
    }
    pad_right(0);
    if(line==current_line_selected){
        config_set_face_attr(LineSelected,0);
    }else{
        config_set_face_attr(Line,0);
    }
}

void draw_status_win(){
    config_set_face_attr(StatusWin,1);
    mvwprintw (stdscr,lines-1,0, "tag:inbox");
    //    pad_line(7);
    wprintw (stdscr, "    (%i/%i)  ",current_line_selected+1,thread_count);
    pad_right(0);
    config_set_face_attr(StatusWin,0);
}

//main


int action_quit(void * c_){
    return 1;
}
int action_next_line(void * c_){
    if(current_line_selected>0){
        --current_line_selected;
        draw_line(current_line_selected);
        draw_line(current_line_selected+1);
        draw_status_win();
    }
    return 0;
}
int action_previous_line(void * c_){
    if(current_line_selected<(thread_count-1)){
        ++current_line_selected;
        draw_line(current_line_selected);
        draw_line(current_line_selected-1);
        draw_status_win();
    }
    return 0;
}

static void cleanup(){
    curs_set(1);
    clrtoeol();
    refresh();
    endwin();
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
    draw_line(thread_count-1);
    draw_status_win();
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

    cols=getmaxx(stdscr);
    lines=getmaxy(stdscr);

    draw_status_win();
    draw_line(0);
    refresh();

    struct actions a;
    a.quit=action_quit;
    a.previous_line=action_previous_line;
    a.next_line=action_next_line;

    nm.callback=&nmr_callback;
    nmr_notmuch_search(&nm,"tag:inbox");

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
            if(config_handle_key_action(c,&a,0)){
                break;
            }
            wrefresh(stdscr);
            refresh();
        }
        if(nm.p!=NULL  && FD_ISSET(fileno(nm.p),&rfds)){
            nmr_notmuch_activate(&nm);
        }

    }
    cleanup();
    return 0;
}
