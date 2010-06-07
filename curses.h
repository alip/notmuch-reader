#include <curses.h>

//drawing
extern int lines,cols,current_line;

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
