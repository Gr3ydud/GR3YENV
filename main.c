#include <ncurses.h>
#include "core.h"
#include "menu.h"

/*
 * main.c: entry point only. All real behavior lives in the screen
 * modules (menu, filemgr, settings, debug) -- this file just wires
 * core up to the first screen and hands off control.
 *
 * Expects menu.h to declare:
 *
 *     gr3y_screen_t *menu_get_screen(void);
 *
 * which returns a statically-allocated gr3y_screen_t for the main
 * menu (the [FILE-MAN] [TERMINAL] [APPLICATIONS] [SETTINGS] [DEBUG]
 * list). menu.c owns its own on_enter/on_draw/on_input and is free
 * to launch filemgr/settings/debug screens internally via launcher.
 */

int main(void)
{
    core_init();

    gr3y_screen_t *main_menu = menu_get_screen();
    core_run(main_menu);

    core_shutdown();
    return 0;
}
