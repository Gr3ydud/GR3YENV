#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "launcher.h"

/*
 * ncurses uses/owns the whole terminal while active, it's tracking
 * cursor position, screen contents, and terminal modes itself.
 * A subprocess like a shell expects a "normal" terminal
 * and knows nothing about ncurses' internal state, so this
 * gives it that before continuing.
 * def_prog_mode()/reset_prog_mode() and
 * endwin()/refresh() are the main operators of this.
 */

static void suspend_curses(void)
{
    def_prog_mode();  /* save the current ncurses terminal state */
    endwin();         /* actually leave curses mode -- terminal is
                          now back to normal, subprocess-usable state */
}

static void resume_curses(void)
{
    reset_prog_mode(); /* restore the saved ncurses terminal state */
    refresh();          /* force a full redraw so the screen isn't
                            left showing whatever the subprocess printed */
}

static int run_and_wait(const char *path, char *const argv[])
{
    pid_t pid = fork();

    if (pid < 0) {
        return -1; /* fork failed -- out of resources, etc. */
    }

    if (pid == 0) {
        /* child process: replace ourselves with the target program */
        execvp(path, argv);
        /* execvp only returns on failure */
        _exit(127);
    }

    /* parent process: wait for the child to finish */
    int status = 0;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    return -1;
}

void launcher_open_shell(void)
{
    suspend_curses();

    const char *shell = getenv("SHELL");
    if (shell == NULL || shell[0] == '\0') {
        shell = "/bin/sh"; /* sane fallback if $SHELL isn't set */
    }

    char *argv[] = { (char *)shell, NULL };
    run_and_wait(shell, argv);

    resume_curses();
}

int launcher_run_command(const char *command)
{
    if (command == NULL) {
        return -1;
    }

    suspend_curses();

    /* Route through `sh -c` so callers can pass a full command line
     * with arguments, rather than us having to parse it ourselves. */
    char *argv[] = { "/bin/sh", "-c", (char *)command, NULL };
    int result = run_and_wait("/bin/sh", argv);

    resume_curses();

    return result;
}
