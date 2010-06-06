#include "config.h"
#include <curses.h>

int config_init(){
    init_pair(1,COLOR_BLACK,COLOR_GREEN);
    init_pair(2,COLOR_BLACK,COLOR_WHITE);
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
            break;
        case LineSelected:
            if(set){
                attron(COLOR_PAIR(1));
            }else {
                attroff(COLOR_PAIR(1));
            }
            break;
        case StatusWin:
            if(set){
                attron(COLOR_PAIR(2));
            }else {
                attroff(COLOR_PAIR(2));
            }
            break;
    };
}

