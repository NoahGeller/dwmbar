/* The list of modules to be displayed (in order).
 * - command: A shell command to be run. stdout will be written to the bar.
 * - interval: Number of seconds between updates. Set to 0 for no periodic
 *   updates.
 * - signal: SIGUSR1 signal to trigger an update. 'pkill -RTMIN+n dwmbar'
 *   should do the trick. Set to 0 for no signal updates.
 */
static const Module modules[] = {
	/* Command			Interval	Signal */
	{ "bar_volume",		0,			1 },
	{ "bar_battery",	10,			0 },
	{ "bar_clock",		1,			0 },
};

/* Delimiter string to be placed between modules. */
static const char delim[] = " | ";
