#include <ncurses.h>
#include <string.h>
#include "core.h"
#include "menu.h"
#include "launcher.h"

/*
 * Each entry has a label (what's shown in brackets) and an action
 * function, called when the user presses Enter on it. For now these
 * actions are placeholders -- once launcher/filemgr/settings/debug
 * exist, swap the placeholder bodies for real calls into those
 * modules (e.g. launcher_open_shell(), filemgr_get_screen(), etc).
 */
typedef struct {
    const char *label;
    void (*action)(void);
} menu_entry_t;

static void action_filemgr(void)  { /* TODO: launcher -> filemgr_get_screen() */ }
static void action_terminal(void) { launcher_open_shell(); }
static void action_apps(void)     { /* TODO: launcher -> applications list */ }
static void action_settings(void) { /* TODO: launcher -> settings_get_screen() */ }
static void action_debug(void)    { /* TODO: launcher -> debug_get_screen() */ }

static menu_entry_t g_entries[] = {
    { "FILEMGR",     action_filemgr  },
    { "TERMINAL",     action_terminal },
    { "APPLICATIONS", action_apps     },
    { "SETTINGS",     action_settings },
    { "DEBUG",        action_debug    },
};

#define ENTRY_COUNT (int)(sizeof(g_entries) / sizeof(g_entries[0]))

static int g_selected = 0;

static void menu_on_enter(void)
{
    g_selected = 0;
}

static void menu_on_draw(void)
{
    clear();

    mvprintw(0, 0, "GR3YOS \"Cerium\"");

    for (int i = 0; i < ENTRY_COUNT; i++) {
        if (i == g_selected) {
            attron(A_REVERSE); /* highlight the currently selected entry */
        }
        mvprintw(1 + i, 0, "[%s]", g_entries[i].label);
        if (i == g_selected) {
            attroff(A_REVERSE);
        }
    }

    refresh();
}

static bool menu_on_input(int ch)
{
    switch (ch) {
        case KEY_UP:
            g_selected = (g_selected - 1 + ENTRY_COUNT) % ENTRY_COUNT;
            break;

        case KEY_DOWN:
            g_selected = (g_selected + 1) % ENTRY_COUNT;
            break;

        case '\n':
        case KEY_ENTER:
            if (g_entries[g_selected].action != NULL) {
                g_entries[g_selected].action();
            }
            break;

        case 'q':
            core_request_quit();
            break;

        default:
            break;
    }

    return true; /* menu never exits on its own via on_input's return value;
                    quitting the whole program goes through core_request_quit() */
}

static gr3y_screen_t g_menu_screen = {
    .name     = "menu",
    .on_enter = menu_on_enter,
    .on_draw  = menu_on_draw,
    .on_input = menu_on_input,
};

gr3y_screen_t *menu_get_screen(void)
{
    return &g_menu_screen;
}
