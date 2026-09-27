#ifndef GR3YOS_LAUNCHER_H
#define GR3YOS_LAUNCHER_H

/*
 * Runs the user's shell, suspending ncurses UI
 * while it's active and restoring it cleanly on return.
 *
 * Blocks until the shell exits (user types `exit`), then
 * control returns to whatever screen called this.
 */
void launcher_open_shell(void);

/*
 * Runs an arbitrary external command interactively (e.g. a text
 * editor, a game, a future gr3yOS application), same suspend/restore
 * behavior as launcher_open_shell(). Blocks until the command exits.
 *
 * command is passed to the shell as-is (via `sh -c`), so it can
 * include arguments, like launcher_run_command("").
 *
 * Returns the command's exit status, or -1 if it couldn't be run
 * at all (e.g. fork() failed).
 */
int launcher_run_command(const char *command);

#endif
