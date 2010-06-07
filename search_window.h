/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet cin fdm=syntax : */

#ifndef NOTMUCH_READER_GUARD_SEARCH_WINDOW_H
#define NOTMUCH_READER_GUARD_SEARCH_WINDOW_H 1

#include "window.h"

typedef struct {
	window_t window;
	unsigned current;
	unsigned maxy;
} search_window_t;

void
search_window_init(search_window_t *win);

void
search_window_deinit(search_window_t *win);

void
search_window_draw_line(search_window_t *win, unsigned index);

#endif /* !NOTMUCH_READER_GUARD_SEARCH_WINDOW_H */
