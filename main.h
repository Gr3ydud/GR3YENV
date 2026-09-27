#ifndef GR3YOS_MENU_H
#define GR3YOS_MENU_H

#include "core.h"

/*
 * Returns a statically-allocated gr3y_screen_t for the main menu
 * ([FILE-MAN] [TERMINAL] [APPLICATIONS] [SETTINGS] [DEBUG]).
 *
 * Ownership stays with menu.c, callers just pass the pointer to
 * core_run() and don't need to free or modify it.
 */
gr3y_screen_t *menu_get_screen(void);

#endif
