/* See LICENSE file for license details. */

#include <ctype.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <X11/Xlib.h>

#include "util.h"

#define VERSION        "0.1"
#define CONFIG_FILE    "sb/config"
#define RESTARTSIG     SIGUSR1
#define CMDLENGTH      (OUTPUT_MAX + STRLEN(DELIM))
#define STATUSLENGTH   (LENGTH(blocks) * CMDLENGTH + 1)

typedef struct {
	const char *name;
	int (*func)(char *);
	int active;
	unsigned int interval;
} Block;

static void run(Block *block);
static void *blockloop(void *block);
static void statusloop(void);
static void sighandler(int sig);
static void getcfg(void);
static void usage(void);

#include "config.h"

static Display *dpy;
static pthread_t thr[LENGTH(blocks)];
static pthread_mutex_t mtx;
static char statusbuf[LENGTH(blocks)][CMDLENGTH];
static char statusbar[STATUSLENGTH];
static volatile sig_atomic_t running;
static volatile sig_atomic_t restart;
static bool pflag;

void
run(Block *block)
{
	char output[CMDLENGTH] = "";
	size_t len, i = block - blocks;
	int rv;

	if ((rv = block->func(output)) < 0)
		return;

	len = rv;
	if (len >= OUTPUT_MAX)
		len = OUTPUT_MAX - 1;
	strcpy(output + len, DELIM);
	len += STRLEN(DELIM);

	pthread_mutex_lock(&mtx);
	if (streql(output, statusbuf[i])) {
		pthread_mutex_unlock(&mtx);
		return;
	}

	memcpy(statusbuf[i], output, len);
	statusbuf[i][len] = '\0';
	statusbar[0] = '\0';
	for (i = 0; i < LENGTH(blocks); i++)
		strcat(statusbar, statusbuf[i]);
	statusbar[strlen(statusbar) - STRLEN(DELIM)] = '\0';
	if (pflag) {
		puts(statusbar);
		fflush(stdout);
	} else {
		XStoreName(dpy, DefaultRootWindow(dpy), statusbar);
		XSync(dpy, False);
	}
	pthread_mutex_unlock(&mtx);
}

void *
blockloop(void *block)
{
	unsigned int interval = ((Block *)block)->interval;

	run(block);
	if (interval == 0)
		pthread_exit(NULL);

	while (running) {
		sleep(interval);
		run(block);
	}
	pthread_exit(NULL);
}

void
statusloop(void)
{
	pthread_attr_t attr;
	size_t i;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	for (i = 0; i < LENGTH(blocks); i++) {
		statusbuf[i][0] = '\0';
		if (blocks[i].active)
			pthread_create(&thr[i], &attr, blockloop, &blocks[i]);
	}
	pthread_attr_destroy(&attr);

	while (running)
		pause();

	for (i = 0; i < LENGTH(blocks); i++) {
		if (blocks[i].active)
			pthread_cancel(thr[i]);
	}
}

void
sighandler(int sig)
{
	switch (sig) {
	case RESTARTSIG:
		restart = true;
	case SIGTERM:
	case SIGINT:
	case SIGHUP:
		running = false;
		break;
	default:
		if (blocks[sig - SIGRTMIN].active)
			run(blocks + sig - SIGRTMIN);
	}
}

void
getcfg(void)
{
	FILE *fp;
	char path[4096], *env;
	char line[128], *p, *key;
	size_t i;
	int val, lc;

	if ((env = getenv("XDG_CONFIG_HOME")))
		xsnprintf(path, sizeof(path), "%s/" CONFIG_FILE, env);
	else if ((env = getenv("HOME")))
		xsnprintf(path, sizeof(path), "%s/.config/" CONFIG_FILE, env);
	else
		die(1, "what is this?");

	if ((fp = fopen(path, "r")) == NULL) {
		warn("fopen '%s':", path);
		return;
	}

	for (lc = 1; fgets(line, sizeof(line), fp); lc++) {
		if (line[0] == '\n' || line[0] == '#')
			continue;
		p = strchr(line, '#');
		if (p != NULL) *p = '\0';
		key = line;
		p = strchr(line, '=');
		if (p == NULL) die(1, "%s:%d: missing '='", path, lc);
		val = atob(p + 1);
		if (val < 0) die(1, "%s:%d: wrong value", path, lc);
		for (p = key; isalpha(*p); p++);
		*p = '\0';

		for (i = 0; i < LENGTH(blocks); i++) {
			if (streql(key, blocks[i].name)) {
				blocks[i].active = val;
				break;
			}
		}
	}

	fclose(fp);
}

void
usage(void)
{
	fprintf(stderr, "usage: %s [-bpv]\n", progname);
	exit(1);
}

int
main(int argc, char *argv[])
{
	struct sigaction sa;
	bool bflag = false;
	size_t i;

	progname = argv[0];
	for (i = 1; i < (size_t)argc; i++) {
		if (streql(argv[i], "-b"))
			bflag = true;
		else if (streql(argv[i], "-p"))
			pflag = true;
		else if (streql(argv[i], "-v"))
			die(0, VERSION);
		else
			usage();
	}

	if (bflag && daemon(1, 0) < 0)
		die(1, "failed to daemonize");
	if (!pflag && (dpy = XOpenDisplay(NULL)) == NULL)
		die(1, "cannot open display");

	sa.sa_handler = sighandler;
	sigfillset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGHUP, &sa, NULL);
	sigaction(RESTARTSIG, &sa, NULL);
	for (i = 0; i < LENGTH(blocks); i++)
		sigaction(SIGRTMIN + i, &sa, NULL);
	pthread_mutex_init(&mtx, NULL);

start:
	getcfg();
	restart = false;
	running = true;
	statusloop();
	if (restart)
		goto start;

	pthread_mutex_destroy(&mtx);
	if (!pflag) {
		XStoreName(dpy, DefaultRootWindow(dpy), "");
		XCloseDisplay(dpy);
	}

	return 0;
}
