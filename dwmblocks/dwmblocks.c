#include <err.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <libconfig.h>

#define LENGTH(X)     (sizeof(X) / sizeof(X[0]))
#define STRLEN(X)     (sizeof(X) - 1)
#define CONFIG_FILE   "dwmblocks/config"
#define RESTARTSIG    SIGUSR1
#define CMDLENGTH     (27 + STRLEN(delim))
#define STATUSLENGTH  (LENGTH(blocks) * CMDLENGTH + 1)

typedef struct {
	const char *name;
	const char *command;
	int active;
	unsigned int interval;
} Block;

#include "config.h"

static void getcmd(Block *block);
static void *blockloop(void *block);
static void statusloop(void);
static void termhandler(int sig);
static void sighandler(int sig);
static void getcfg(void);

static Display *dpy;
static pthread_t thr[LENGTH(blocks)];
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static char statusbar[LENGTH(blocks)][CMDLENGTH];
static char statusstr[STATUSLENGTH];
static sig_atomic_t running;
static sig_atomic_t restart;

static void
getcmd(Block *block)
{
	FILE *cmd;
	char output[CMDLENGTH] = "";
	size_t len, i = block - blocks;

	cmd = popen(block->command, "r");
	if (cmd == NULL)
		return;
	fgets(output, CMDLENGTH - STRLEN(delim), cmd);
	pclose(cmd);
	len = strlen(output);
	if (len == 0)
		return;
	len = output[len - 1] == '\n' ? len - 1 : len;
	strcpy(output + len, delim);

	pthread_mutex_lock(&mutex);
	if (strcmp(output, statusbar[i]) == 0) {
		pthread_mutex_unlock(&mutex);
		return;
	}

	memcpy(statusbar[i], output, len + STRLEN(delim));
	statusbar[i][len + STRLEN(delim)] = '\0';
	statusstr[0] = '\0';
	for (i = 0; i < LENGTH(blocks); i++)
		strcat(statusstr, statusbar[i]);
	statusstr[strlen(statusstr) - STRLEN(delim)] = '\0';
	// TODO: can fail ?
	XStoreName(dpy, DefaultRootWindow(dpy), statusstr);
	XSync(dpy, False);
	pthread_mutex_unlock(&mutex);
}

static void *
blockloop(void *block)
{
	unsigned int interval = ((Block *)block)->interval;

	getcmd(block);
	if (interval == 0)
		pthread_exit(NULL);

	while (running) {
		sleep(interval);
		getcmd(block);
	}
	pthread_exit(NULL);
}

static void
statusloop(void)
{
	pthread_attr_t attr;
	size_t i;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	for (i = 0; i < LENGTH(blocks); i++) {
		statusbar[i][0] = '\0';
		if (blocks[i].active)
			pthread_create(thr + i, &attr, blockloop, blocks + i);
	}
	pthread_attr_destroy(&attr);

	while (running)
		pause();

	for (i = 0; i < LENGTH(blocks); i++) {
		if (blocks[i].active)
			pthread_cancel(thr[i]);
	}
}

static void
sighandler(int sig)
{
	if (blocks[sig - SIGRTMIN].active)
		getcmd(blocks + sig - SIGRTMIN);
}

static void
termhandler(int sig)
{
	restart = (sig == RESTARTSIG);
	running = 0;
}

static void
getcfg(void)
{
	config_t cfg;
	char path[PATH_MAX], *env;
	size_t i;

	env = getenv("XDG_CONFIG_HOME");
	if (env) {
		snprintf(path, sizeof(path), "%s/%s", env, CONFIG_FILE);
	} else {
		env = getenv("HOME");
		if (env == NULL)
			errx(1, "HOME is not defined");
		snprintf(path, sizeof(path), "%s/.config/%s", env, CONFIG_FILE);
	}

	config_init(&cfg);
	if (config_read_file(&cfg, path) == CONFIG_FALSE) {
		errx(1, "%s:%d: %s", path, config_error_line(&cfg),
		    config_error_text(&cfg));
	}
	for (i = 0; i < LENGTH(blocks); i++)
		config_lookup_bool(&cfg, blocks[i].name, &blocks[i].active);
	config_destroy(&cfg);
}

int
main(void)
{
	size_t i;

	dpy = XOpenDisplay(NULL);
	if (dpy == NULL)
		errx(1, "Failed to open display");
	signal(SIGTERM, termhandler);
	signal(SIGINT, termhandler);
	signal(RESTARTSIG, termhandler);
	for (i = 0; i < LENGTH(blocks); i++)
		signal(SIGRTMIN + i, sighandler);

start:
	getcfg();
	restart = 0;
	running = 1;
	statusloop();
	if (restart)
		goto start;

	XStoreName(dpy, DefaultRootWindow(dpy), "");
	XCloseDisplay(dpy);

	return 0;
}
