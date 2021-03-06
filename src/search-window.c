#include <sys/select.h>
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <locale.h>

#include "conf.h"
#include "notmuch.h"
#include "curses.h"

struct nmr_notmuch_thread ** threads=0;
int thread_count=0; 
int expected_threads=0;

void nmr_search_draw_line(int line){
    if(line>lines-2)
        return;
    if(line<thread_count){

        struct nmr_notmuch_thread * th=threads[line];

        config_set_face_attr(line==current_line?LineSelected:Line,1);

        //format the matched/total column
        int expected_num_w=3;
        mvwprintw (stdscr,line,0, "%'*i/%'-*i",expected_num_w,
                th->matched,expected_num_w,th->total);

        //make room for short tags
        clip_addstr("     ");

        //print Authors
        config_set_face_attr(Authors,1);
        clip_addstr (th->authors);
        pad_left((cols+0.5)/4);
        wmove(stdscr,line,(cols+0.5)/4);
        config_set_face_attr(Authors,0);

        //format subject
        for(int i=0;i<th->tags_l;i++){
            if(strcmp(th->tags[i],"unread")==0){
                attron(WA_BOLD);
            }
        }
        clip_addstr("  ");
        clip_addstr (th->subject);
        clip_addstr("  ");
        attroff(WA_BOLD);

        //print long tags
        config_set_face_attr(line==current_line?TagSelected:Tag,1);
        for(int i=0;i<th->tags_l;i++){
            if(strcmp(th->tags[i],"attachment")==0){
                addstr_at("A",(expected_num_w*2)+4);
                continue;
            }
            if(strcmp(th->tags[i],"inbox")==0){
                addstr_at("I",(expected_num_w*2)+3);
                continue;
            }
            if(strcmp(th->tags[i],"unread")==0){
                addstr_at("U",(expected_num_w*2)+2);
                continue;
            }
            clip_addstr("+");
            clip_addstr(th->tags[i]);
            if(i+1<th->tags_l){
                clip_addstr(" ");
            }
        }
        config_set_face_attr(line==current_line?TagSelected:Tag,0);
        clip_addstr(" ");
    }else{
        mvwprintw (stdscr,line,0, "...");
    }

    pad_right(0);
    if(line==current_line){
        config_set_face_attr(LineSelected,0);
    }else{
        config_set_face_attr(Line,0);
    }
}

int nmr_search_action_down(void * c_){
    if(current_line>0){
        --current_line;
        nmr_search_draw_line(current_line);
        nmr_search_draw_line(current_line+1);
        draw_status_win();
    }
    return 0;
}
int nmr_search_action_up(void * c_){
    if(current_line<(thread_count-1)){
        ++current_line;
        nmr_search_draw_line(current_line);
        nmr_search_draw_line(current_line-1);
        draw_status_win();
    }
    return 0;
}
