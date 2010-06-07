/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet cin fdm=syntax : */

#ifndef NOTMUCH_READER_GUARD_STATUS_BAR_H
#define NOTMUCH_READER_GUARD_STATUS_BAR_H 1

#include <stdarg.h>

#include "window.h"

typedef struct {
	window_t window;
} status_bar_t;

void
status_bar_init(status_bar_t *bar, unsigned width, int y, int x);

void
status_bar_deinit(status_bar_t *bar);

void
status_bar_message(status_bar_t *bar, const char *fmt, ...);

#endif /* !NOTMUCH_READER_GUARD_STATUS_BAR_H */
