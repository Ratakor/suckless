/* See LICENSE file for license details. */

#define LENGTH(X)      (sizeof(X) / sizeof(X[0]))
#define STRLEN(X)      (sizeof(X) - 1)
#define HOUR(t)        ((t / 3600) % 24)
#define MINUTE(t)      ((t / 60) % 60)
#define SECONDE(t)     (t % 60)
#define streql(s1, s2) (strcmp(s1, s2) == 0)

extern char *progname;

void warn(const char *fmt, ...);
void die(int status, const char *fmt, ...);
time_t ltime(void);
int xsnprintf(char *str, size_t siz, const char *fmt, ...);
int execcmd(char *output, size_t siz, const char *cmd);
intmax_t fgetsn(const char *path);
int atob(const char *s);
