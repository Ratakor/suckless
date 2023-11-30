#ifndef BLOCKS
#define BLOCKS "./blocks"
#endif /* BLOCKS */

static Block blocks[] = {
	/* Name               Command              Active   Update Interval  */
	{ "music",      BLOCKS "/sb_music",          0,           0,         },
	{ "cputemp",    BLOCKS "/sb_cputemp",        0,           10,        },
	{ "fanspeed",   BLOCKS "/sb_fanspeed",       0,           10,        },
	{ "memory",     BLOCKS "/sb_memory",         0,           10,        },
	{ "battery",    BLOCKS "/sb_battery",        0,           30,        },
	{ "wifi",       BLOCKS "/sb_wifi",           0,           10,        },
	{ "ethernet",   BLOCKS "/sb_ethernet",       0,           10,        },
	{ "localip",    BLOCKS "/sb_localip",        0,           3600,      },
	{ "publicip",   BLOCKS "/sb_publicip",       0,           3600,      },
	{ "volume",     BLOCKS "/sb_volume",         0,           0,         },
	{ "mic",        BLOCKS "/sb_mic",            0,           0,         },
	{ "news",       BLOCKS "/sb_news",           0,           3600,      },
	{ "weather",    BLOCKS "/sb_weather",        0,           18000,     },
	{ "daypercent", BLOCKS "/sb_daypercent",     0,           600,       },
	{ "date",       BLOCKS "/sb_date",           0,           3600,      },
	{ "time",       BLOCKS "/sb_time",           1,           60,        },
};

static char delim[] = " \03|\01 ";
