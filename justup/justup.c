#include <sys/time.h>
#include <stdio.h>
#include <sys/sysctl.h>

struct timeval	boottime;
time_t		now;

static void 	print_uptime(time_t *);

int
main(int argc, char *argv[])
{
	(void)time(&now);
	pr_header(&now);
	printf("\n");
}

static void
print_uptime(time_t *nowp)
{
	time_t uptime;
	int days, hrs, mins, secs;
	int mib[2];
	size_t size;
    	char buf[256];

   	if (strftime(buf, sizeof(buf),
   	NULL, localtime(nowp)) != 0)
	mib[0] = CTL_KERN;
	mib[1] = KERN_BOOTTIME;
	size = sizeof(boottime);
	if (sysctl(mib, 2, &boottime, &size, NULL, 0) != -1 &&
	    boottime.tv_sec != 0) {
		uptime = now - boottime.tv_sec;
		if (uptime > 60)
		uptime += 30;
		days = uptime / 86400;
		uptime %= 86400;
		hrs = uptime / 3600;
		uptime %= 3600;
		mins = uptime / 60;
		secs = uptime % 60;
		if (days > 0)
			(void)printf("%d day%s", days, days > 1 ? "s" : "");
		if (hrs > 0 && mins > 0)
			(void)printf("%2d:%02d", hrs, mins);
		else if (hrs > 0)
			(void)printf("%d hr%s", hrs, hrs > 1 ? "s" : "");
		else if (mins > 0)
			(void)printf("%d min%s", mins, mins > 1 ? "s" : "");
		else
			(void)printf("%d sec%s", secs, secs > 1 ? "s" : "");
	}
}
