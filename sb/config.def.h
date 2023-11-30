/* See LICENSE file for license details. */

#define BATTERY      "BAT0"
#define ETH          "eth0"
#define WIFI         "wlan0"
#define THERMAL_ZONE "thermal_zone0"
#define BASE         1000

/* dwm colors (NORM SEL BLUE GREEN ORANGE RED YELLOW MAGENTA CYAN) */
#define COLORS       true
#include "blocks.h"

/* max size of a block */
#define OUTPUT_MAX   32

/* delimiter between each blocks */
#define DELIM        " " BLUE "|" NORM " "

#ifndef BLOCKS_C
static Block blocks[] = {
	/* name            func           active    interval */
	{ "music",         music,         false,    0,     },
	{ "cputemp",       cputemp,       false,    10,    },
	{ "cpu",           cpu,           false,    10,    },
	{ "memory",        memory,        false,    10,    },
	{ "battery",       battery,       false,    30,    },
	{ "wifi",          wifi,          false,    10,    },
	{ "netspeed",      netspeed,      false,    1,     },
	{ "localip",       localip,       false,    3600,  },
	{ "publicip",      publicip,      false,    3600,  },
	{ "volume",        volume,        false,    0,     },
	{ "mic",           mic,           false,    0,     },
	{ "news",          news,          false,    3600,  },
	{ "weather",       weather,       false,    18000, },
	{ "daypercent",    daypercent,    false,    1800,  },
	{ "date",          date,          false,    3600,  },
	{ "time",          sb_time,       true,     60,    },
};
#endif /* BLOCKS_C */
