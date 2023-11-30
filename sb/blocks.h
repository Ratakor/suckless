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

int music(char *output);
int cputemp(char *output);
int cpu(char *output);
int memory(char *output);
int battery(char *output);
int wifi(char *output);
int netspeed(char *output);
int localip(char *output);
int publicip(char *output);
int volume(char *output);
int mic(char *output);
int news(char *output);
int weather(char *output);
int daypercent(char *output);
int date(char *output);
int sb_time(char *output);
