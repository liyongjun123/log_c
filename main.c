#include <unistd.h>
#include <stdio.h>
#include "log.h"

extern char *optarg;
extern int optopt;

int main(int argc, char *argv[])
{
    int ch;
    while ((ch = getopt(argc, argv, "d")) != -1)
    {
        switch (ch)
        {
        case 'd':
            set_option(PRINT_D);
            break;
        }
    }


    log_init();
    log_loop_init();

    int count = 2500;
    while (count--)
    {
        log_a("%s %d %f\n", "is running", count, 55.55);
        log_b("b\n");
        log_c("c\n");
        log_d("d\n");
    }

    return 0;
}
