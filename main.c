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

//drawing
static int lines,cols,current_line_selected;


static void pad_line(int to_right){
    int l,c;
    getyx(stdscr, l,c);
    while(c<(cols-to_right)){
        mvaddch(l,c++,' ');
    }
}


static void draw_line(int line){
    if(line==current_line_selected){
        config_set_face_attr(LineSelected,1);
    }else{
        config_set_face_attr(Line,1);
    }
    mvwprintw (stdscr,line,0, "[1/5] Somedue,Otherdude Blablabla (inbox unread) ");
    pad_line(0);
    if(line==current_line_selected){
        config_set_face_attr(LineSelected,0);
    }else{
        config_set_face_attr(Line,0);
    }
}

void draw_status_win(){
    config_set_face_attr(StatusWin,1);
    mvwprintw (stdscr,lines-1,0, "notmuch search tag:inbox");
    pad_line(8);
    wprintw (stdscr, "page 1/1");
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
    }
    return 0;
}
int action_previous_line(void * c_){
    if(current_line_selected<(lines-2)){
        ++current_line_selected;
        draw_line(current_line_selected);
        draw_line(current_line_selected-1);
    }
    return 0;
}

static void cleanup(){
    curs_set(1);
    clrtoeol();
    refresh();
    endwin();
}


int main(int argc,char**argv){
    initscr();
    curs_set(0);
    start_color();
    config_init();

    clear();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();

    cols=getmaxx(stdscr);
    lines=getmaxy(stdscr);

    draw_status_win();
    draw_line(0);
    draw_line(1);
    refresh();

    struct actions a;
    a.quit=action_quit;
    a.previous_line=action_previous_line;
    a.next_line=action_next_line;

    fd_set rfds;
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
    for(;;) {
        FD_ZERO (&rfds);
        FD_SET  (0,&rfds);
        int maxfd=0;

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

    }
    cleanup();
    return 0;
}
