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
#include <stdio.h>

//drawing
static int lines,cols,current_line_selected;
static int fg,bg;

static struct nmr_notmuch_thread ** threads=0;
static int thread_count=0; 
static int expected_threads=0;

static void addstr_at( const char *s, int at){
    if(at+1>=cols){
        return;
    }
    int l,c;
    getyx(stdscr, l,c);
    mvaddnstr(l,at,s,cols-at-1);
    wmove(stdscr,l,c);
}
static void clip_addstr(const char * s){
    int l,c;
    getyx(stdscr, l,c);
    if(c+1>=cols){
//        mvaddstr(l,cols-4,"...");
        return;
    }
    addnstr(s,cols-c-1);
}

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
    if(line<thread_count){

        struct nmr_notmuch_thread * th=threads[line];

        if(line==current_line_selected){
            config_set_face_attr(LineSelected,1);
        }else{
            config_set_face_attr(Line,1);
        }

        //format the matched/total column
        int expected_num_w=3;
        mvwprintw (stdscr,line,0, "%'*i/%'-*i",expected_num_w,
                th->matched,expected_num_w,th->total);

        clip_addstr("     ");

        config_set_face_attr(Authors,1);
        clip_addstr (th->authors);
        pad_left((cols+0.5)/4);
        wmove(stdscr,line,(cols+0.5)/4);
        config_set_face_attr(Authors,0);

        for(int i=0;i<th->tags_l;i++){
            if(strcmp(th->tags[i],"unread")==0){
                attron(WA_BOLD);
            }
        }
        clip_addstr("  ");
        clip_addstr (th->subject);
        clip_addstr("  ");
//        pad_right(cols/4);
        attroff(WA_BOLD);
        if(line==current_line_selected){
            config_set_face_attr(TagSelected,1);
        }else{
            config_set_face_attr(Tag,1);
        }
        for(int i=0;i<th->tags_l;i++){
            if(strcmp(th->tags[i],"attachment")==0){
                addstr_at("A",(expected_num_w*2)+3);
                continue;
            }
            if(strcmp(th->tags[i],"inbox")==0){
                addstr_at("I",(expected_num_w*2)+2);
                continue;
            }
            if(strcmp(th->tags[i],"unread")==0){
                addstr_at("U",(expected_num_w*2)+3);
                continue;
            }
            clip_addstr("+");
            clip_addstr(th->tags[i]);
            if(i+1<th->tags_l){
                clip_addstr(" ");
            }
        }
        if(line==current_line_selected){
            config_set_face_attr(TagSelected,0);
        }else{
            config_set_face_attr(Tag,0);
        }
//        pad_right((4*cols/5)-9);
        clip_addstr(" ");

        //        wprintw (stdscr," \t\t--  %s ",th->authors);
    }else{
        mvwprintw (stdscr,line,0, "...");
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
    draw_line(0);
    refresh();

    struct actions a;
    a.quit=action_quit;
    a.previous_line=action_previous_line;
    a.next_line=action_next_line;

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
