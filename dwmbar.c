#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <X11/Xlib.h>

typedef struct {
	char* command;
	int signal;
} Module;

#include "modules.h"

#define MODULE_LENGTH	50
#define NUM_MODULES		sizeof(modules) / sizeof(modules[0])
#define BAR_LENGTH		MODULE_LENGTH * NUM_MODULES

static void bar_loop();
static void run_command();
static void run_commands();
static void setup_signals();
static int setup_X();
static void signal_handler();
static void update_bar_string();
static void write_bar();

static char bar[NUM_MODULES][MODULE_LENGTH] = {0};
static char bar_string[BAR_LENGTH];

static Display *dpy;
static int screen;
static Window root;

/* Runs the command in module, storing the first MODULE_LENGTH characters of
 * its output in outupt.
 */
void run_command(const Module *module, char* output) {
	// Fork and run the command, saving the result in output.
	FILE *p = popen(module->command, "r");
	if (!p)
		return;
	fgets(output, MODULE_LENGTH - strlen(delim), p);

	// Remove trailing newline if necessary.
	int i = strlen(output);
	if (output[i-1] == '\n')
		output[i-1] = '\0';

	// Add the delimiter to the end of the output.
	strcpy(&output[i-1], delim);

	pclose(p);
}

/* Traverses the modules array and runs each command, storing each output as
 * a string in the bar array. */
void run_commands() {
	const Module* current;
	for (unsigned int i = 0; i < NUM_MODULES; i++) {
		current = modules + i;
		run_command(current, bar[i]);
	}

	update_bar_string();
	write_bar();
}

/* Updates bar_string with the contents of bar. */
void update_bar_string() {
	// Throw away the old bar_string.
	bar_string[0] = '\0';

	for (unsigned int i = 0; i < NUM_MODULES; i++) {
		strcpy(&bar_string[strlen(bar_string)], bar[i]);
	}

	// Remove the final delimiter.
	int i = strlen(bar_string);
	bar_string[i - strlen(delim)] = '\0';
}

/* Writes bar_string to the screen. */
void write_bar() {
	XStoreName(dpy, root, bar_string);
	XFlush(dpy);
}

/* Updates the bar every second. */
void bar_loop() {
	setup_signals();

	while (1) {
		run_commands();
		sleep(1.0);
	}
}

/* Gets information about the X11 display. */
int setup_X() {
	dpy = XOpenDisplay(NULL);
	if (!dpy) {
		fprintf(stderr, "dwmbar: Failed to open display\n");
		return 0;
	}
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	return 1;
}

/* When the application recieves SIGUSR1, this handler runs the appropriate
 * commands and updates the bar accordingly.
 * */
void signal_handler(int sig) {
	for (unsigned int i = 0; i < NUM_MODULES; i++) {
		if (modules[i].signal == sig)
			run_command(&modules[i], bar[i]);
	}
	update_bar_string();
	write_bar();
}

/* Prepares signals for each module. */
void setup_signals() {
	// Initialize all signals to be ignored.
	struct sigaction dummy;
	dummy.sa_handler = SIG_IGN;
	sigemptyset(&dummy.sa_mask);
	dummy.sa_flags = 0;
	for (int i = SIGRTMIN; i <= SIGRTMAX; i++)
		sigaction(i, &dummy, NULL);

	// Add handlers to module-defined signals.
	struct sigaction sa;
	sa.sa_handler = signal_handler;
	sa.sa_flags = 0;
	for (unsigned int i = 0; i < NUM_MODULES; i++) {
		sigemptyset(&sa.sa_mask);
		sigaddset(&sa.sa_mask, SIGRTMIN + modules[i].signal);
		sigaction(SIGRTMIN + modules[i].signal, &sa, NULL);
	}
}

int main(int argc, char* argv[]) {
	if (!setup_X()) {
		return 1;
	}

	bar_loop();

	XCloseDisplay(dpy);
	return 0;
}
