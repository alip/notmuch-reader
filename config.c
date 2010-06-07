#include "config.h"
#include <curses.h>

int config_init(){
    init_pair(1,-1,-1);
    init_pair(2,COLOR_BLACK,COLOR_GREEN);
    init_pair(3,COLOR_BLACK,COLOR_WHITE);
    init_pair(4,COLOR_YELLOW,-1);
    init_pair(5,82,-1);
}

const char * config_notmuch_call(){
    return "notmuch";
}

int config_handle_key_action(int key,struct actions * actions, void * context){
    switch(key){
        case 'q':
            return actions->quit(context);
        case KEY_UP:
        case 'k':
            return actions->next_line(context);
        case KEY_DOWN:
        case 'j':
            return actions->previous_line(context);
    }
    return 0;
}

void config_set_face_attr(enum Face f,int set){
    switch (f){
        case Line:
            if(set){
                attron(COLOR_PAIR(1));
            }else {
                attroff(COLOR_PAIR(1));
            }
            break;
        case LineSelected:
            if(set){
                attron(COLOR_PAIR(2));
            }else {
                attroff(COLOR_PAIR(2));
            }
            break;
        case StatusWin:
            if(set){
                attron(COLOR_PAIR(3));
            }else {
                attroff(COLOR_PAIR(3));
            }
            break;
        case Tag:
            if(set){
                attron(COLOR_PAIR(4));
            }else {
                attroff(COLOR_PAIR(4));
            }
            break;
        case TagSelected:
            break;
        case Authors:
            break;
            {
                int a=COLOR_PAIR(5);
                if(set){
                    attron(a);
                }else{
                    attroff(a);
                }
                break;
            }
    };
}

