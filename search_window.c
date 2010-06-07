/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet cin fdm=syntax : */

#include <ncurses.h>

#include "config.h"
#include "notmuch.h"
#include "pad.h"
#include "search_window.h"

void
search_window_init(search_window_t *win)
{
	window_init(&win->window, LINES - 2, COLS, 0, 0);

	leaveok(win->window.w, FALSE);
	keypad(win->window.w, TRUE);
	meta(win->window.w, TRUE);

	win->current = 0;
	win->maxy = getmaxy(win->window.w);
}

void
search_window_deinit(search_window_t *win)
{
	delwin(win->window.w);
}

void
search_window_draw_line(search_window_t *win, unsigned index)
{
	unsigned i;
	struct nmr_notmuch_thread *th;
	WINDOW *w = win->window.w;

	if (index > win->maxy - 2)
		return;

	config_set_face_attr(w, (index == win->current) ? LineSelected : Line, 1);

	if (index < thread_count) {
		th = threads[index];
		mvwprintw(w, index, 0, "[%i/%i]", th->matched, th->total);
	}
	else {
		mvwprintw(w, index, 0, "...");
		goto end;
	}

	pad_left(&win->window, 8);

	wprintw(w, "%s %s  ", th->authors, th->subject);

	waddch(w, '(');
	config_set_face_attr(w, Tag, 1);
	for (i = 0; i < th->tags_l;) {
		wprintw(w, "%s", th->tags[i]);
		if (++i < th->tags_l)
			waddch(w, ' ');
	}
	config_set_face_attr(w, Tag, 0);
	waddch(w, ')');

end:
	pad_right(&win->window, 0);
	wnoutrefresh(w);
	config_set_face_attr(w, (index == win->current) ? LineSelected : Line, 0);
}
