/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet cin fdm=syntax : */

#ifndef NOTMUCH_READER_GUARD_PAD_H
#define NOTMUCH_READER_GUARD_PAD_H 1

#include "window.h"

static void
pad_right(window_t *win, unsigned count)
{
	unsigned l, c;

	getyx(win->w, l, c);

	while (c < win->cols - count)
		mvwaddch(win->w, l, c++, ' ');
}

static void
pad_left(window_t *win, unsigned count)
{
	unsigned l, c;

	getyx(win->w, l, c);

	while (c < count)
		mvwaddch(win->w, l, c++, ' ');
}

#endif /* !NOTMUCH_READER_GUARD_PAD_H */
