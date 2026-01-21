#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <sys/signal.h>

#define WATCHDOG_IOCTL_BASE     'W'

struct watchdog_info {
    unsigned int options;          /* Options the card/driver supports */
    unsigned int firmware_version; /* Firmware version of the card */
    char identity[32];     /* Identity of the board */
};

#define WDIOC_GETSUPPORT        _IOR(WATCHDOG_IOCTL_BASE, 0, struct watchdog_info)
#define WDIOC_GETSTATUS         _IOR(WATCHDOG_IOCTL_BASE, 1, int)
#define WDIOC_GETBOOTSTATUS     _IOR(WATCHDOG_IOCTL_BASE, 2, int)
#define WDIOC_GETTEMP           _IOR(WATCHDOG_IOCTL_BASE, 3, int)
#define WDIOC_SETOPTIONS        _IOR(WATCHDOG_IOCTL_BASE, 4, int)
#define WDIOC_KEEPALIVE         _IOR(WATCHDOG_IOCTL_BASE, 5, int)
#define WDIOC_SETTIMEOUT        _IOWR(WATCHDOG_IOCTL_BASE, 6, int)
#define WDIOC_GETTIMEOUT        _IOR(WATCHDOG_IOCTL_BASE, 7, int)
#define WDIOC_SETPRETIMEOUT     _IOWR(WATCHDOG_IOCTL_BASE, 8, int)
#define WDIOC_GETPRETIMEOUT     _IOR(WATCHDOG_IOCTL_BASE, 9, int)
#define WDIOC_GETTIMELEFT       _IOR(WATCHDOG_IOCTL_BASE, 10, int)

#define WDIOF_OVERHEAT          0x0001  /* Reset due to CPU overheat */
#define WDIOF_FANFAULT          0x0002  /* Fan failed */
#define WDIOF_EXTERN1           0x0004  /* External relay 1 */
#define WDIOF_EXTERN2           0x0008  /* External relay 2 */
#define WDIOF_POWERUNDER        0x0010  /* Power bad/power fault */
#define WDIOF_CARDRESET         0x0020  /* Card previously reset the CPU */
#define WDIOF_POWEROVER         0x0040  /* Power over voltage */
#define WDIOF_SETTIMEOUT        0x0080  /* Set timeout (in seconds) */
#define WDIOF_MAGICCLOSE        0x0100  /* Supports magic close char */
#define WDIOF_PRETIMEOUT        0x0200  /* Pretimeout (in seconds), get/set */
#define WDIOF_KEEPALIVEPING     0x8000  /* Keep alive ping reply */

#define WDIOS_DISABLECARD       0x0001  /* Turn off the watchdog timer */
#define WDIOS_ENABLECARD        0x0002  /* Turn on the watchdog timer */
#define WDIOS_TEMPPANIC         0x0004  /* Kernel panic on temperature trip */

static int wdt_fd;
#define DEFAULT_PING_RATE    1
static void stop_signal()
{
    int val = 0, ret = 0 ;

    val = WDIOS_DISABLECARD ;
    ret = ioctl(wdt_fd, WDIOC_SETOPTIONS, &val) ;
    if (ret < 0)
        printf("ioctl WDIOC_GETSUPPORT failed with %d.\n", ret);

    printf("===watchdow will be closed===\n") ;
    close(wdt_fd) ;
    exit(0);

}

static int wdt_start(void)
{
    int val, ret;

    val = WDIOS_ENABLECARD ;
    ret = ioctl(wdt_fd, WDIOC_SETOPTIONS, &val) ;
    if (ret < 0)
        printf("ioctl WDIOC_SETOPTIONS failed with %d.\n", ret);

    return ret;
}

static int wdt_stop(void)
{
    int val, ret;

    val = WDIOS_DISABLECARD ;
    ret = ioctl(wdt_fd, WDIOC_SETOPTIONS, &val) ;
    if (ret < 0)
        printf("ioctl WDIOC_SETOPTIONS failed with %d.\n", ret);

    return ret;
}

static int wdt_set_timeout(int time_out)
{
    int ret;

    time_out = time_out < 3 ? 3 : time_out;
    ret = ioctl(wdt_fd, WDIOC_SETTIMEOUT, &time_out);
    if (ret < 0)
        printf("ioctl WDIOC_SETTIMEOUT failed.\n");

    return ret;
}

static int wdt_keep_alive(void)
{
    return ioctl(wdt_fd, WDIOC_KEEPALIVE, NULL);
}


int main(int argc, char *argv[])
{
    int f, ret;
    int time_out = 5;
    int ping_count = 5;

    // static int count = 0;
    struct watchdog_info wdt_info;
    // unsigned int ping_rate = DEFAULT_PING_RATE;

    signal(SIGINT, stop_signal) ;

    wdt_fd = open("/dev/watchdog0", O_RDWR);
    if (wdt_fd < 0) {
        printf("open /dev/watchdog0 failed.\n");
    }

    /* get watchdog infomation struct */
    ret = ioctl(wdt_fd, WDIOC_GETSUPPORT, &wdt_info);
    if (ret < 0)
        printf("ioctl WDIOC_GETSUPPORT failed.\n");
    else {
        printf("options = 0x%x,id = %s\n", wdt_info.options, wdt_info.identity);
    }

    while (1) {
        printf("0 help\n1 start\n2 keepalive\n3 set timeout\n4 close\n5 test\nplease input:\n");
        scanf("%d", &f);

        switch (f) {
        case 0:
            printf("0 help\n1 start\n2 keepalive\n3 set timeout\n4 close\n5 test\nplease input:\n");
            break;
        case 1:
            wdt_start();
            break;
        case 2:
            wdt_keep_alive();
            break;
        case 3:
            printf("please input timeout:\n");
            scanf("%d", &time_out);
            wdt_set_timeout(time_out);
            break;
        case 4:
            wdt_stop();
            break;
        case 5:
            printf("please input timeout(s):\n");
            scanf("%d", &time_out);
            printf("please input ping count:\n");
            scanf("%d", &ping_count);
            wdt_start();
            wdt_set_timeout(time_out);
            while (ping_count--) {
                wdt_keep_alive();
                sleep(time_out);
            }
            wdt_stop();
            printf("wdt_keep_alive ok\n");
        default:
            break;
        }
    }

    close(wdt_fd);
    return 0;
}
