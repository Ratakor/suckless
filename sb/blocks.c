/* See LICENSE file for license details. */

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <linux/wireless.h>

#include "util.h"
#define BLOCKS_C
#include "config.h"

/* don't change any of these without changing the corresponding function */
#define NEWS_CMD       "newsboat -x print-unread"
#define MIC_CMD        "wpctl get-volume @DEFAULT_AUDIO_SOURCE@"
#define VOLUME_CMD     "wpctl get-volume @DEFAULT_AUDIO_SINK@"
#define PUBLICIP_URL   "ifconfig.me"
#define WEATHER_URL    "wttr.in?format=1"
#define CAPACITY_PATH  "/sys/class/power_supply/" BATTERY "/capacity"
#define STATUS_PATH    "/sys/class/power_supply/" BATTERY "/status"
#define CPUTEMP_PATH   "/sys/devices/virtual/thermal/" THERMAL_ZONE "/temp"
#define OPERSTATE(X)   "/sys/class/net/" X "/operstate"
#define NETSPEED_RX(X) "/sys/class/net/" X "/statistics/rx_bytes"
#define NETSPEED_TX(X) "/sys/class/net/" X "/statistics/tx_bytes"
#define MUSIC_PAUSE    "{\"command\":[\"get_property_string\",\"pause\"]}\n"
#define MUSIC_TITLE    "{\"command\":[\"get_property\",\"media-title\"]}\n"

#if BASE == 1000
static const char *const prefix[] = {
	"", "k", "M", "G", "T", "P", "E", "Z", "Y"
};
#elif BASE == 1024
static const char *const prefix[] = {
	"", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei", "Zi", "Yi"
};
#else
#error "BASE must be equal to 1000 or 1024"
#endif /* BASE */

int
music(char *output)
{
	static const struct sockaddr_un addr = {
		.sun_path = "/tmp/mpvsocket",
		.sun_family = AF_UNIX
	};
	static const char *const properties[] = { MUSIC_PAUSE, MUSIC_TITLE };
	char buf[2 * OUTPUT_MAX], *start, *end;
	ssize_t len;
	int i, fd;
	bool pause;

	for (i = 0; i < 2; i++) {
		if ((fd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
			warn("socket:");
			close(fd);
			return -1;
		}
		if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			/* warn("connect '%s':", addr.sun_path); */
			close(fd);
			output[0] = '\0';
			return 1;
		}
		if (send(fd, properties[i], strlen(properties[i]), 0) < 0) {
			warn("send '%s':", properties[i]);
			close(fd);
			return -1;
		}
		if ((len = recv(fd, buf, STRLEN(buf), MSG_PEEK)) < 0) {
			/* warn("recv:"); */
			close(fd);
			return -1;
		}
		buf[len] = '\0';

		if (close(fd) < 0) {
			warn("close:");
			return -1;
		}

		start = buf + STRLEN("{\"data\":\"");
		if (i == 0) {
			pause = (strncmp(start, "yes", STRLEN("yes")) == 0);
		} else {
			end = strchr(start, ',');
			if (end) *(end - 1) = '\0';
		}
	}

	/* use snprintf to not fail on truncation */
	if (pause)
		return snprintf(output, OUTPUT_MAX, MAGENTA" %s"NORM, start);
	return snprintf(output, OUTPUT_MAX, " %s", start);
}

int
cputemp(char *output)
{
	intmax_t temp;

	temp = fgetsn(CPUTEMP_PATH);
	if (temp <= 0)
		return -1;

	temp /= 1000;
	if (temp >= 70)
		return xsnprintf(output, OUTPUT_MAX, RED" %02d°C"NORM, temp);
	return xsnprintf(output, OUTPUT_MAX, " %02d°C", temp);
}

int
cpu(char *output)
{
	static long double a[7];
	long double b[7], sum;
	FILE *fp;

	memcpy(b, a, sizeof(b));
	if ((fp = fopen("/proc/stat", "r")) == NULL) {
		warn("fopen '%s':", "/proc/stat");
		return -1;
	}

	/* cpu user nice system idle iowait irq softirq */
	if (fscanf(fp, "%*s %Lf %Lf %Lf %Lf %Lf %Lf %Lf",
	                &a[0], &a[1], &a[2], &a[3], &a[4], &a[5], &a[6])
	                != 7) {
		fclose(fp);
		return -1;
	}
	fclose(fp);

	sum = (b[0] + b[1] + b[2] + b[3] + b[4] + b[5] + b[6]) -
	      (a[0] + a[1] + a[2] + a[3] + a[4] + a[5] + a[6]);

	if (sum == 0)
		return -1;

	return xsnprintf(output, OUTPUT_MAX, " %d%%", (int)(100 *
	                 ((b[0] + b[1] + b[2] + b[5] + b[6]) -
	                  (a[0] + a[1] + a[2] + a[5] + a[6])) / sum));
}

int
memory(char *output)
{
	FILE *fp;
	uintmax_t total, free, buffers, cached;
	double dtotal, used;
	size_t i, j;

	if ((fp = fopen("/proc/meminfo", "r")) == NULL) {
		warn("fopen '%s':", "/proc/meminfo");
		return -1;
	}

	if (fscanf(fp, "MemTotal: %ju kB\n"
	                "MemFree: %ju kB\n"
	                "MemAvailable: %ju kB\n"
	                "Buffers: %ju kB\n"
	                "Cached: %ju kB\n",
	                &total, &free, &buffers, &buffers, &cached) != 5) {
		fclose(fp);
		return -1;
	}
	fclose(fp);

	used = (total - free - buffers - cached) * BASE;
	dtotal = total * BASE;
	for (i = 0; i < LENGTH(prefix) && used >= BASE; i++)
		used /= BASE;
	for (j = 0; j < LENGTH(prefix) && dtotal >= BASE; j++)
		dtotal /= BASE;

	return xsnprintf(output, OUTPUT_MAX, "ﳔ %.1f%s/%.1f%s",
	                 used, prefix[i], dtotal, prefix[j]);
}

int
battery(char *output)
{
	static const char *const icons[] = {
		"󰂎", "󰁺", "󰁻", "󰁼", "󰁽", "󰁾", "󰁿", "󰂀", "󰂁", "󰂂", "󰁹",
		"󰢟", "󰢜", "󰂆", "󰂇", "󰂈", "󰢝", "󰂉", "󰢞", "󰂊", "󰂋", "󰂅"
	};
	const char *icon, *color;
	FILE *fp;
	char status[32];
	intmax_t capacity;
	bool charging;

	if ((capacity = fgetsn(CAPACITY_PATH)) < 0)
		return -1;

	if ((fp = fopen(STATUS_PATH, "r")) == NULL) {
		warn("fopen '%s':", STATUS_PATH);
		return -1;
	}
	if (fgets(status, sizeof(status), fp) == NULL) {
		fclose(fp);
		return -1;
	}
	fclose(fp);

	charging = (streql(status, "Charging\n"));
	icon = icons[(capacity / 10) + (charging * LENGTH(icons) / 2)];
	color = charging ? NORM : "";

	if (capacity >= 90)
		color = GREEN;
	else if (!charging) {
		if (capacity < 40 && capacity >= 30)
			color = YELLOW;
		else if (capacity < 30 && capacity >= 20)
			color = ORANGE;
		else if (capacity < 20)
			color = RED;
	}

	if (charging)
		return xsnprintf(output, OUTPUT_MAX, YELLOW"%s %s%d%%"NORM,
		                 icon, color, capacity);
	return xsnprintf(output, OUTPUT_MAX, "%s %s%d%%"NORM,
	                 icon, color, capacity);
}

int
wifi(char *output)
{
	char ssid[IW_ESSID_MAX_SIZE + 1] = "";
	const struct iwreq wreq = {
		.ifr_name = WIFI,
		.u.essid.length = sizeof(ssid),
		.u.essid.pointer = ssid
	};
	FILE *fp;
	char buf[128], *p;
	int i, fd, quality;

	if ((fp = fopen(OPERSTATE(WIFI), "r")) == NULL) {
		warn("fopen '%s':", OPERSTATE(WIFI));
		return -1;
	}
	p = fgets(buf, sizeof(buf), fp);
	fclose(fp);
	if (p == NULL || !streql(buf, "up\n"))
		return xsnprintf(output, OUTPUT_MAX, "󰤯");
	if ((fp = fopen("/proc/net/wireless", "r")) == NULL) {
		warn("fopen '/proc/net/wireless':");
		return -1;
	}
	/* skip to line 3 */
	for (i = 0; i < 3; i++) {
		if ((p = fgets(buf, sizeof(buf), fp)) == NULL)
			break;
	}
	fclose(fp);
	if (i != 3 || p == NULL || (p = strstr(buf, WIFI)) == NULL)
		return -1;
	p += sizeof(WIFI) + 1;
	if (sscanf(p, "%*d %d %*d", &quality) != 1)
		return -1;
	quality = (float)quality / 70 * 100;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		warn("socket 'AF_INET':");
		return -1;
	}
	if (ioctl(fd, SIOCGIWESSID, &wreq) < 0) {
		warn("ioctl 'SIOCGIWESSID':");
		close(fd);
		return -1;
	}
	close(fd);

	if (quality >= 70)
		return xsnprintf(output, OUTPUT_MAX, "󰤨 %s", ssid);
	if (quality >= 30)
		return xsnprintf(output, OUTPUT_MAX, "󰤢 %s", ssid);
	return xsnprintf(output, OUTPUT_MAX, "󰤟 %s", ssid);
}

int
netspeed(char *output)
{
	static intmax_t rx, tx;
	static const char *pathrx, *pathtx;
	intmax_t tmprx, tmptx;
	double drx, dtx;
	size_t i, j;

	if (tx == 0 && rx == 0) {
		FILE *fp;
		char buf[4], *p;

		do {
			if ((fp = fopen(OPERSTATE(WIFI), "r")) != NULL) {
				p = fgets(buf, sizeof(buf), fp);
				fclose(fp);
				if (p != NULL && streql(buf, "up\n")) {
					pathrx = NETSPEED_RX(WIFI);
					pathtx = NETSPEED_TX(WIFI);
					break;
				}
			}
			if ((fp = fopen(OPERSTATE(ETH), "r")) != NULL) {
				p = fgets(buf, sizeof(buf), fp);
				fclose(fp);
				if (p != NULL && streql(buf, "up\n")) {
					pathrx = NETSPEED_RX(ETH);
					pathtx = NETSPEED_TX(ETH);
					break;
				}
			}
			return -1;
		} while (0);
	}

	tmprx = rx;
	tmptx = tx;
	if ((rx = fgetsn(pathrx)) < 0)
		return -1;
	if ((tx = fgetsn(pathtx)) < 0)
		return -1;

	drx = rx - tmprx;
	dtx = tx - tmptx;
	for (i = 0; i < LENGTH(prefix) && drx >= BASE; i++)
		drx /= BASE;
	for (j = 0; j < LENGTH(prefix) && dtx >= BASE; j++)
		dtx /= BASE;

	return xsnprintf(output, OUTPUT_MAX,
	                 BLUE "" NORM " %.1f%s%s" ORANGE "" NORM " %.1f%s",
	                 drx, prefix[i], DELIM, dtx, prefix[j]);
}

int
localip(char *output)
{
	struct ifaddrs *ifaddr, *ifa;
	char host[NI_MAXHOST];
	int s;

	if (getifaddrs(&ifaddr) < 0) {
		warn("getifaddrs:");
		return -1;
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;

		s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in6),
		                host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
		if ((ifa->ifa_addr->sa_family == AF_INET) &&
		                (streql(ifa->ifa_name, ETH) ||
		                 streql(ifa->ifa_name, WIFI))) {
			freeifaddrs(ifaddr);
			if (s != 0) {
				warn("getnameinfo: %s", gai_strerror(s));
				return -1;
			}
			return xsnprintf(output, OUTPUT_MAX, "ﯱ %s", host);
		}
	}

	freeifaddrs(ifaddr);
	return -1;
}

int
publicip(char *output)
{
	char buf[32];

	if (execcmd(buf, sizeof(buf), "curl -s " PUBLICIP_URL) < 0)
		return -1;

	return xsnprintf(output, OUTPUT_MAX, " %s", buf);
}

int
volume(char *output)
{
	char buf[32];
	int vol;

	if (execcmd(buf, sizeof(buf), VOLUME_CMD) < 0)
		return -1;

	vol = atof(buf + STRLEN("Volume: ")) * 100;

	if (strstr(buf, "MUTED"))
		return xsnprintf(output, OUTPUT_MAX, CYAN" %d%%"NORM, vol);
	return xsnprintf(output, OUTPUT_MAX, " %d%%", vol);
}

int
mic(char *output)
{
	char buf[32];

	if (execcmd(buf, sizeof(buf), MIC_CMD) < 0)
		return -1;

	if (strstr(buf, "MUTED"))
		return xsnprintf(output, OUTPUT_MAX, ORANGE""NORM);
	return xsnprintf(output, OUTPUT_MAX, "");
}

int
news(char *output)
{
	char buf[32];

	if (execcmd(buf, sizeof(buf), NEWS_CMD) < 0)
		return -1;
	if (buf[0] == 'E')
		return -1;

	return xsnprintf(output, OUTPUT_MAX, "📰 %d", atoi(buf));
}

int
weather(char *output)
{
	char buf[32];
	int i, j;

	if (execcmd(buf, sizeof(buf), "curl -s " WEATHER_URL) < 0)
		return -1;

	for (i = 0, j = 0; j < OUTPUT_MAX; i++) {
		switch (buf[i]) {
		case ' ':
			continue;
		case '\n':
			output[j] = '\0';
			break;
		case '+':
		case '-':
			output[j++] = ' ';
		default:
			output[j++] = buf[i];
			continue;
		}
		break;
	}

	return (j == OUTPUT_MAX) ? -1 : j;
}

int
daypercent(char *output)
{
	time_t t;
	int percent;

	t = ltime();
	percent = ((HOUR(t) * 60 + MINUTE(t)) * 100) / 1440;

	return xsnprintf(output, OUTPUT_MAX, " %d%%", percent);
}

int
date(char *output)
{
	time_t t;
	size_t rv;

	t = ltime();
	rv = strftime(output, OUTPUT_MAX, " %b %d (%a)", gmtime(&t));
	if (rv == 0) {
		warn("strftime: String truncation");
		return -1;
	}
	return rv;
}

int
sb_time(char *output)
{
	time_t t;

	t = ltime();
	if (HOUR(t) >= 22) {
		return xsnprintf(output, OUTPUT_MAX, ORANGE" %02d:%02d"NORM,
		                 HOUR(t), MINUTE(t));
	} else if (HOUR(t) <= 5) {
		return xsnprintf(output, OUTPUT_MAX, RED" %02d:%02d"NORM,
		                 HOUR(t), MINUTE(t));
	}
	return xsnprintf(output, OUTPUT_MAX, " %02d:%02d",
	                 HOUR(t), MINUTE(t));
}
