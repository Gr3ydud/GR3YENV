#ifndef GR3YOS_FILEMGR_H
#define GR3YOS_FILEMGR_H

#include "core.h"

/*
 * Returns a statically-allocated gr3y_screen_t for the file manager.
 * Intended to be run via core_run() from within another screen's
 * input handler (e.g. menu.c's [FILEMGR] entry), core_run() blocks
 * until the user backs out, then control returns to the caller.
 */
gr3y_screen_t *filemgr_get_screen(void);

#endif
