#include <ncurses.h>
#include "core.h"

/*
 * Temporary placeholder screen, proof of concept
 * end to end before menu.c exists. Delete when menu.c is ready
 * and wire core_run() to the real menu screen instead.
 */

static void placeholder_draw(void)
{
    clear();
    mvprintw(0, 0, "GR3YOS \"Cerium\" -- core module test");
    mvprintw(2, 0, "Press q to quit.");
    refresh();
}

static bool placeholder_input(int ch)
{
    if (ch == 'q') {
        return false; /* exits core_run's loop */
    }
    return true;
}

int main(void)
{
    core_init();

    gr3y_screen_t placeholder = {
        .name = "placeholder",
        .on_enter = NULL,
        .on_draw = placeholder_draw,
        .on_input = placeholder_input,
    };

    core_run(&placeholder);

    core_shutdown();
    return 0;
}
