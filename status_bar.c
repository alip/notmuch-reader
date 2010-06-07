/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet cin fdm=syntax : */

#include <stdarg.h>
#include <curses.h>

#include "config.h"
#include "window.h"
#include "pad.h"
#include "status_bar.h"

void
status_bar_init(status_bar_t *bar, unsigned width, int y, int x)
{
	window_init(&bar->window, 1, width, y, x);

	leaveok(bar->window.w, FALSE);
	keypad(bar->window.w, TRUE);
}

void
status_bar_deinit(status_bar_t *bar)
{
	delwin(bar->window.w);
}

void
status_bar_message(status_bar_t *bar, const char *fmt, ...)
{
	va_list args;
	WINDOW *w = bar->window.w;

	wmove(w, 0, 0);
	wclrtoeol(w);
	config_set_face_attr(w, StatusWin, 1);

	va_start(args, fmt);
	vwprintw(w, fmt, args);
	va_end(args);

	pad_right(&bar->window, 0);

	wnoutrefresh(w);
	config_set_face_attr(w, StatusWin, 0);
}
