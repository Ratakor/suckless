/* See LICENSE file for license details. */

#if COLORS
#define NORM         "\x1"
#define SEL          "\x2"
#define BLUE         "\x3"
#define GREEN        "\x4"
#define ORANGE       "\x5"
#define RED          "\x6"
#define YELLOW       "\x7"
#define MAGENTA      "\x8"
#define CYAN         "\x9"
#else
#define NORM         ""
#define SEL          ""
#define BLUE         ""
#define GREEN        ""
#define ORANGE       ""
#define RED          ""
#define YELLOW       ""
#define MAGENTA      ""
#define CYAN         ""
#endif /* COLORS */

int music(char *output, bool signal);
int cputemp(char *output, bool signal);
int cpu(char *output, bool signal);
int memory(char *output, bool signal);
int battery(char *output, bool signal);
int wifi(char *output, bool signal);
int netspeed(char *output, bool signal);
int localip(char *output, bool signal);
int publicip(char *output, bool signal);
int volume(char *output, bool signal);
int mic(char *output, bool signal);
int news(char *output, bool signal);
int weather(char *output, bool signal);
int daypercent(char *output, bool signal);
int date(char *output, bool signal);
int sb_time(char *output, bool signal);
