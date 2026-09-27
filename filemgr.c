#include <ncurses.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "core.h"
#include "filemgr.h"

#define MAX_ENTRIES 512
#define MAX_NAME_LEN 256

typedef struct {
    char name[MAX_NAME_LEN];
    bool is_dir;
} fm_entry_t;

static char g_cwd[PATH_MAX];
static char g_start_cwd[PATH_MAX]; /* directory filemgr was entered from --
                                       going up past this exits back to menu */
static fm_entry_t g_entries[MAX_ENTRIES];
static int g_entry_count = 0;
static int g_selected = 0;

/* alphabetical, directories first -- makes long listings easier to scan */
static int entry_cmp(const void *a, const void *b)
{
    const fm_entry_t *ea = a;
    const fm_entry_t *eb = b;

    if (ea->is_dir != eb->is_dir) {
        return eb->is_dir - ea->is_dir; /* dirs (1) before files (0) */
    }
    return strcmp(ea->name, eb->name);
}

static void load_directory(const char *path)
{
    g_entry_count = 0;

    DIR *dir = opendir(path);
    if (dir == NULL) {
        return; /* on_draw will just show an empty listing */
    }

    struct dirent *de;
    while ((de = readdir(dir)) != NULL && g_entry_count < MAX_ENTRIES) {
        /* skip "." -- but keep ".." so the user can navigate upward */
        if (strcmp(de->d_name, ".") == 0) {
            continue;
        }

        fm_entry_t *entry = &g_entries[g_entry_count];
        strncpy(entry->name, de->d_name, MAX_NAME_LEN - 1);
        entry->name[MAX_NAME_LEN - 1] = '\0';
        entry->is_dir = (de->d_type == DT_DIR);

        g_entry_count++;
    }

    closedir(dir);

    qsort(g_entries, g_entry_count, sizeof(fm_entry_t), entry_cmp);
}

static void filemgr_on_enter(void)
{
    if (getcwd(g_start_cwd, sizeof(g_start_cwd)) == NULL) {
        strncpy(g_start_cwd, "/", sizeof(g_start_cwd));
    }
    strncpy(g_cwd, g_start_cwd, sizeof(g_cwd));

    g_selected = 0;
    load_directory(g_cwd);
}

static void filemgr_on_draw(void)
{
    clear();

    mvprintw(0, 0, "GR3YOS \"Cerium\" -- FILEMGR");
    mvprintw(1, 0, "%s", g_cwd);

    int row = 3;
    for (int i = 0; i < g_entry_count; i++) {
        if (i == g_selected) {
            attron(A_REVERSE);
        }

        if (g_entries[i].is_dir) {
            mvprintw(row, 0, "[%s]", g_entries[i].name);
        } else {
            mvprintw(row, 0, " %s", g_entries[i].name);
        }

        if (i == g_selected) {
            attroff(A_REVERSE);
        }

        row++;
    }

    mvprintw(row + 1, 0, "Enter: open dir   Backspace: up / exit   q: quit gr3yOS");
    refresh();
}

static void enter_selected(void)
{
    if (g_entry_count == 0) {
        return;
    }

    fm_entry_t *sel = &g_entries[g_selected];
    if (!sel->is_dir) {
        /* TODO: once launcher supports it, open the file here
         * (e.g. launcher_run_command with $EDITOR). For now,
         * files are inert -- selecting one does nothing. */
        return;
    }

    char new_path[PATH_MAX];
    if (strcmp(sel->name, "..") == 0) {
        /* going up: strip the last path component */
        strncpy(new_path, g_cwd, sizeof(new_path));
        char *slash = strrchr(new_path, '/');
        if (slash != NULL && slash != new_path) {
            *slash = '\0';
        } else {
            strncpy(new_path, "/", sizeof(new_path));
        }
    } else {
        snprintf(new_path, sizeof(new_path), "%s/%s",
                  strcmp(g_cwd, "/") == 0 ? "" : g_cwd, sel->name);
    }

    strncpy(g_cwd, new_path, sizeof(g_cwd));
    g_selected = 0;
    load_directory(g_cwd);
}

static bool filemgr_on_input(int ch)
{
    switch (ch) {
        case KEY_UP:
            if (g_entry_count > 0) {
                g_selected = (g_selected - 1 + g_entry_count) % g_entry_count;
            }
            break;

        case KEY_DOWN:
            if (g_entry_count > 0) {
                g_selected = (g_selected + 1) % g_entry_count;
            }
            break;

        case '\n':
        case KEY_ENTER:
            enter_selected();
            break;

        case KEY_BACKSPACE:
        case 127: /* some terminals send DEL instead of KEY_BACKSPACE */
        case '\b':
            if (strcmp(g_cwd, g_start_cwd) == 0) {
                return false; /* already at the starting dir -- exit filemgr */
            }
            /* otherwise, go up one directory in place */
            {
                char new_path[PATH_MAX];
                strncpy(new_path, g_cwd, sizeof(new_path));
                char *slash = strrchr(new_path, '/');
                if (slash != NULL && slash != new_path) {
                    *slash = '\0';
                } else {
                    strncpy(new_path, "/", sizeof(new_path));
                }
                strncpy(g_cwd, new_path, sizeof(g_cwd));
                g_selected = 0;
                load_directory(g_cwd);
            }
            break;

        case 'q':
            core_request_quit();
            break;

        default:
            break;
    }

    return true;
}

static gr3y_screen_t g_filemgr_screen = {
    .name     = "filemgr",
    .on_enter = filemgr_on_enter,
    .on_draw  = filemgr_on_draw,
    .on_input = filemgr_on_input,
};

gr3y_screen_t *filemgr_get_screen(void)
{
    return &g_filemgr_screen;
}
