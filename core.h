#ifndef GR3YOS_CORE_H
#define GR3YOS_CORE_H

#include <stdbool.h>

/*
 * A "screen" is any full-screen mode the environment can be in:
 * the main menu, the file manager, settings, debug, etc.
 *
 * core.c doesn't know what a screen *does* -- it just calls these
 * three functions at the right times. This keeps core decoupled
 * from menu/filemgr/settings/debug, so those modules can be built
 * and changed independently.
 */
typedef struct {
    const char *name;         /* short identifier, mainly for debug/logging */
    void (*on_enter)(void);   /* called once when this screen becomes active */
    void (*on_draw)(void);    /* called every frame to redraw this screen */
    bool (*on_input)(int ch); /* called with a keypress; return false to
                                  request returning to the previous screen */
} gr3y_screen_t;

/* Initializes ncurses and any core state. Call once at startup. */
void core_init(void);

/* Tears down ncurses cleanly. Call once before exit, and on any
 * fatal error path -- never let the program exit with ncurses
 * still active, or the terminal will be left in a broken state. */
void core_shutdown(void);

/* Pushes a new screen onto the active stack and runs it until it
 * requests to exit (on_input returns false) or the whole program
 * is told to quit via core_request_quit(). */
void core_run(gr3y_screen_t *screen);

/* Signals the main loop to stop after the current iteration.
 * Safe to call from within an on_input handler. */
void core_request_quit(void);

#endif
