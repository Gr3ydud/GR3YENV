#include <ncurses.h>
#include "core.h"

/* Set by core_request_quit(); checked once per loop iteration. */
static bool g_quit_requested = false;

void core_init(void)
{
    initscr();              /* enter curses mode */
    cbreak();               /* disable line buffering, no waiting for Enter */
    noecho();                /* don't echo typed characters to the screen */
    keypad(stdscr, TRUE);   /* enable arrow keys, function keys, etc. */
    curs_set(0);            /* hide the terminal cursor -- we draw our own UI */

    /* Non-blocking-ish input with a short timeout so the status bar
     * (once it exists) can redraw on its own tick even with no
     * keypress. 100ms is a reasonable starting point. */
    timeout(100);
}

void core_shutdown(void)
{
    endwin();
}

void core_request_quit(void)
{
    g_quit_requested = true;
}

void core_run(gr3y_screen_t *screen)
{
    if (screen == NULL) {
        return;
    }

    if (screen->on_enter != NULL) {
        screen->on_enter();
    }

    g_quit_requested = false;

    while (!g_quit_requested) {
        if (screen->on_draw != NULL) {
            screen->on_draw();
        }

        int ch = getch();

        /* timeout() with no input returns ERR -- that's expected and
         * just means "nothing happened this tick," not an error. */
        if (ch == ERR) {
            continue;
        }

        if (screen->on_input != NULL) {
            bool keep_running = screen->on_input(ch);
            if (!keep_running) {
                break;
            }
        }
    }
}
