/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet cin fdm=syntax : */

#ifndef NOTMUCH_READER_GUARD_SCREEN_H
#define NOTMUCH_READER_GUARD_SCREEN_H 1

#include "window.h"
#include "status_bar.h"

typedef struct {
	search_window_t search_window;
	status_bar_t status_bar;
} screen_t;

extern screen_t screen;

#endif /* !NOTMUCH_READER_GUARD_SCREEN_H */
