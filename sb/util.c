/* See LICENSE file for license details. */

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "util.h"

char *progname;

static void
vwarn(const char *fmt, va_list ap)
{
	fprintf(stderr, "%s: ", progname);

	if (fmt == NULL) {
		perror(NULL);
		return;
	}

	vfprintf(stderr, fmt, ap);
	if (fmt[0] && fmt[strlen(fmt) - 1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	} else {
		fputc('\n', stderr);
	}
}

void
warn(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vwarn(fmt, ap);
	va_end(ap);
}

void
die(int status, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vwarn(fmt, ap);
	va_end(ap);

	exit(status);
}

time_t
ltime(void)
{
	static time_t tz;
	static bool once;

	if (!once) {
		const time_t t = time(NULL);
		const struct tm *const tm = localtime(&t);
		time_t tz_hour, tz_min;

		tz_hour = tm->tm_hour - HOUR(t);
		tz_min = tm->tm_min - MINUTE(t);
		tz = ((tz_hour * 60) + tz_min) * 60;
		once = true;
	}

	return time(NULL) + tz;
}

/* return -1 on failure and warn about truncation (which is also a failure) */
int
xsnprintf(char *str, size_t siz, const char *fmt, ...)
{
	va_list ap;
	int rv;

	va_start(ap, fmt);
	rv = vsnprintf(str, siz, fmt, ap);
	va_end(ap);

	if (rv < 0) {
		warn("snprintf:");
		return -1;
	} else if ((size_t)rv >= siz) {
		warn("snprintf: String truncation");
		return -1;
	}

	return rv;
}

/* return -1 on failure */
int
execcmd(char *output, size_t siz, const char *cmd)
{
	FILE *fp;

	if ((fp = popen(cmd, "r")) == NULL) {
		warn("popen '%s':", cmd);
		return -1;
	}
	if (fgets(output, siz, fp) == NULL) {
		pclose(fp);
		return -1;
	}
	pclose(fp);

	return 0;
}

/* return -1 on failure (or 0) */
intmax_t
fgetsn(const char *path)
{
	FILE *fp;
	char buf[4 * sizeof(intmax_t)];

	if ((fp = fopen(path, "r")) == NULL) {
		warn("fopen '%s':", path);
		return -1;
	}
	if (fgets(buf, sizeof(buf), fp) == NULL) {
		fclose(fp);
		return -1;
	}
	fclose(fp);

	return atoll(buf);
}

int
atob(const char *s)
{
	while (isspace(*s)) s++;
	if (strncasecmp(s, "true", STRLEN("true")) == 0)
		return true;
	else if (strncasecmp(s, "false", STRLEN("false")) == 0)
		return false;
	else
		return -1;
}
