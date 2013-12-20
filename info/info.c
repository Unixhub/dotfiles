/* 
 *
 *  Created by yrmt 
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <pwd.h>
#include <sys/sysctl.h>
#include <sys/statvfs.h>
#include <sqlite3.h>

#define C0  "\x1B[0m"    //Reset
#define C1  "\x1B[0;32m" //Green
#define C2  "\x1B[0;33m" //Yellow
#define C3  "\x1B[1;31m" //RED
#define C4  "\x1B[0;31m" //Red
#define C5  "\x1B[0;35m" //Purple
#define C6  "\x1B[0;36m" //Blue
#define RED C3
#define NOR C0

bool print_with_apple = false;

static const struct {
    const char *ctls;
    const char *names;
} values[] = {
    { "hw.model", "Model" },
    { "machdep.cpu.brand_string", "Processor" },
    { "hw.memsize", "Memory" },
    { "kern.ostype", "OS" },
    { "kern.osrelease", "Kernel" },
    { "kern.hostname", "Hostname" },
};

static void sysctls(int i);
static void envs(int i);
static void disk(void);
static void pkg(void);
static void print_uptime(time_t *nowp);

static void print_apple(void) {
    time_t now;
    time(&now);
    printf(C1"                :++++.        ");envs(3);
    printf(C1"               /+++/.         ");sysctls(0);
    printf(C1"       .:-::- .+/:-``.::-     ");sysctls(3); 
    printf(C2"    .:/++++++/::::/++++++/:`  ");sysctls(4);
    printf(C2"  .:///////////////////////:` ");sysctls(5);
    printf(C2"  ////////////////////////`   ");sysctls(2);
    printf(C3" -+++++++++++++++++++++++`    ");envs(2);
    printf(C3" /++++++++++++++++++++++/     ");envs(1);
    printf(C4" /sssssssssssssssssssssss.    ");sysctls(1);
    printf(C4" :ssssssssssssssssssssssss-   ");disk();
    printf(C5"  osssssssssssssssssssssssso/ ");pkg();
    printf(C5"  `syyyyyyyyyyyyyyyyyyyyyyyy+ ");print_uptime(&now);
    printf(C5"   `ossssssssssssssssssssss/  \n");
    printf(C6"     :ooooooooooooooooooo+.   \n");
    printf(C6"      `:+oo+/:-..-:/+o+/-     \n");
}
void help(void) {
    printf("Mac OS X Info script by yrmt dec. 2013\n"
            "\t-a shows a colored apple\n"
            "\t-h shows this help msg\n");
    exit(0);
}
static void sysctls(int i) {
    size_t len;
    uint64_t value64;
    if(i == 2) {
        len = sizeof(value64);
        sysctlbyname(values[2].ctls, &value64, &len, NULL, 0);
        printf(RED"%-10s: "NOR"%.f MB\n", values[2].names, value64/1e+06);
    } else {
        sysctlbyname(values[i].ctls, NULL, &len, NULL, 0);
        char *type = malloc(len);
        sysctlbyname(values[i].ctls, type, &len, NULL, 0);
        printf(RED"%-10s: "NOR"%s\n", values[i].names, type);
        free(type);
    }
}
static void envs(int i) {
    char *type;
    struct passwd *passwd;
    if(i == 1) {
        type = getenv("TERM");
        printf(RED"Terminal  : "NOR"%s\n", type);
    } else if(i ==2) {
        passwd = getpwuid(getuid());
        printf(RED"Shell     : "NOR"%s\n",passwd->pw_shell);
    } else if(i == 3) {
        passwd = getpwuid(getuid());
        printf(RED"User      : "NOR"%s\n",passwd->pw_name);
    }
}
static void pkg(void) {
    sqlite3 *db;
    int pkgs;
    sqlite3_stmt *s;
    char *sql = "SELECT COUNT (*) FROM LOCAL_PKG";

    if (sqlite3_open("/var/db/pkgin/pkgin.db", &db) != SQLITE_OK) {
        fprintf(stderr, "cannot open db");
    }

    if (sqlite3_prepare_v2(db, sql, -1, &s, NULL) == SQLITE_OK) {
        if (sqlite3_step(s) == SQLITE_ERROR) {
            fprintf(stderr, "error querying db");
        }

        pkgs = sqlite3_column_int(s,0);
    }
    sqlite3_close(db);
    printf(RED"Packages  :"NOR" %d\n", pkgs);
}
static void disk(void) {
    struct statvfs info;
    if (-1 == statvfs("/", &info))
        printf("failed to get disk info");
    else {
        unsigned long left  = (info.f_bavail * info.f_frsize);
        unsigned long total = (info.f_files * info.f_frsize);
        unsigned long used  = total - left;
        float perc  = (float)used / (float)total;
        printf(RED"Disk usage:"NOR" %.2f%%\n", perc * 100);
    }
}
static void print_uptime(time_t *nowp)
{
    struct timeval boottime;
    time_t uptime;
    int days, hrs, mins, secs;
    int mib[2];
    size_t size;
    char buf[256];

    if (strftime(buf, sizeof(buf), NULL, localtime(nowp)) != 0)
        mib[0] = CTL_KERN;
    mib[1] = KERN_BOOTTIME;
    size = sizeof(boottime);
    if (sysctl(mib, 2, &boottime, &size, NULL, 0) != -1 && 
            boottime.tv_sec != 0) {
        uptime = *nowp - boottime.tv_sec;
        if (uptime > 60)
            uptime += 30;
        days = uptime / 86400;
        uptime %= 86400;
        hrs = uptime / 3600;
        uptime %= 3600;
        mins = uptime / 60;
        secs = uptime % 60;
        printf(RED"Uptime    : "NOR);
        if (days > 0)
            printf("%d day %s", days, days > 1 ? "s" : "");
        if (hrs > 0 && mins > 0)
            printf("%2d:%02d", hrs, mins);
        else if (hrs > 0)
            printf("%d hr%s", hrs, hrs > 1 ? "s" : "");
        else if (mins > 0)
            printf("%d min%s", mins, mins > 1 ? "s" : "");
        else
            printf("%d sec%s", secs, secs > 1 ? "s" : "");
        printf("\n");
    }
}
int main(int argc, char **argv) {
    char c;
    if (argc >= 2) {
        while((c = getopt(argc, argv, "ha")) != -1) {
            switch(c) {
                case 'h':
                    help();
                    break;
                case 'a':
                    print_with_apple = true;
                    break;
                default:
                    help();
                    break;
            }
        }
    }
    if(print_with_apple) {
        print_apple();
    } else {
        time_t now;
        time(&now);
        for (int i=5;i>-1;i--) {
            sysctls(i);
        }
        envs(1);
        envs(2);
        envs(3);
        disk();
        pkg();
        print_uptime(&now);
    }
}
