#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "my_socket.h"
#include "get_num.h"
#include "logUtil.h"
#include "my_signal.h"
#include "set_timer.h"

int debug = 0;
volatile sig_atomic_t got_alrm = 0;

void sig_alrm(int signo)
{
    got_alrm = 1;
    return;
}

int usage()
{
    char msg[] = "Usage: client [-d] [-h] [-p PORT] [-b BUFSIZE] [-s SLEEP_USEC] [-t TIMEOUT] remote_host";
    fprintf(stderr, "%s\n", msg);

    return 0;
}

int main(int argc, char *argv[])
{
    int c;
    int port = 1234;
    int bufsize = 4*1024;
    int sleep_usec = -1;
    char *timeout_str = "2.0";

    while ( (c = getopt(argc, argv, "b:dhp:s:t:")) != -1) {
        switch (c) {
            case 'h':
                usage();
                exit(0);
                break;
            case 'b':
                bufsize = get_num(optarg);
                break;
            case 'd':
                debug = 1;
                break;
            case 'p':
                port = strtol(optarg, NULL, 0);
                break;
            case 's':
                sleep_usec = strtol(optarg, NULL, 0);
                break;
            case 't':
                timeout_str = optarg;
                break;
            default:
                break;
        }
    }
    argc -= optind;
    argv += optind;
    if (argc != 1) {
        usage();
        exit(1);
    }
    char *remote_host = argv[0];
    struct timeval timeout;
    conv_str2timeval(timeout_str, &timeout);
    
    unsigned char *buf = malloc(bufsize);
    if (buf == NULL) {
        err(1, "malloc");
    }

    int sockfd = tcp_socket();
    if (sockfd < 0) {
        exit(1);
    }

    my_signal(SIGALRM, sig_alrm);
    set_timer(timeout.tv_sec, timeout.tv_usec, 0, 0);

    if (connect_tcp(sockfd, remote_host, port) < 0) {
        exit(1);
    }
    
    for ( ; ; ) {
        if (got_alrm) {

            debug_print(stderr, "going shutdown(,SHUT_WR)\n");
            if (shutdown(sockfd, SHUT_WR) < 0) {
                err(1, "shutdown(sockfd, SHUT_WR)");
            }
            debug_print(stderr, "shutdown(,SHUT_WR) done\n");

            for ( ; ; ) {
                int n = read(sockfd, buf, bufsize);
                if (n < 0) {
                    err(1, "read in got_alrm");
                }
                if (n == 0) {
                    break;
                }
                debug_print(stderr, "reading after got_alrm: %d bytes\n", n);
            }

#if 0
            debug_print(stderr, "going shutdown(,SHUT_RD)\n");
            if (shutdown(sockfd, SHUT_RD) < 0) {
                err(1, "shutdown(sockfd, SHUT_RD)");
            }
            debug_print(stderr, "shutdown(,SHUT_RD) done\n");
#endif

            usleep(1000);
            if (close(sockfd) < 0) {
                err(1, "close()");
            }
            exit(0);
        }
        int n = read(sockfd, buf, bufsize);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            else {
                err(1, "read");
            }
        }
        if (sleep_usec > 0) {
            usleep(sleep_usec);
        }
    }

}