/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet cin fdm=syntax : */

#ifndef NOTMUCH_READER_GUARD_WINDOW_H
#define NOTMUCH_READER_GUARD_WINDOW_H 1

#include <ncurses.h>

typedef struct {
	WINDOW *w;
	unsigned rows, cols;
} window_t;

static inline void
window_init(window_t *window, unsigned height, unsigned width, int y, int x)
{
	window->w = newwin(height, width, y, x);
	window->cols = width;
	window->rows = height;
}

#endif /* !NOTMUCH_READER_GUARD_WINDOW_H */
