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
    char msg[] = "Usage: client [-d] [-h] [-p PORT] [-b BUFSIZE] [-s SLEEP_USEC] [-t TIMEOUT] [-I] remote_host\n"
                 "-I: client close() immediately after send TCP FIN (shutdown(fd, SHUT_WR)";
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
    int has_sent_stop_request = 0;
    int immediately_close = 0;

    while ( (c = getopt(argc, argv, "b:dhp:s:t:I")) != -1) {
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
            case 'I':
                immediately_close = 1;
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
            got_alrm = 0;
            /* send data stop request */
            debug_print(stderr, "send stop request. do shutdown(,SHUT_WR)\n");
            if (shutdown(sockfd, SHUT_WR) < 0) {
                err(1, "shutdown(sockfd, SHUT_WR)");
            }
            debug_print(stderr, "shutdown(,SHUT_WR) done\n");
            if (immediately_close) {
                debug_print(stderr, "immediately close. do close()\n");
                close(sockfd);
                debug_print(stderr, "immediately close. close() done. exit()\n");
                exit(0);
            }
            /* read remaining data in for ( ; ; ) loop */
            has_sent_stop_request = 1;
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
        if (n == 0) {
            debug_print(stderr, "close() returns 0. (EOF)\n");
            break;
        }
        if (has_sent_stop_request) {
            debug_print(stderr, "after stop request read(): %d bytes\n", n);
        }
        if (sleep_usec > 0) {
            usleep(sleep_usec);
        }
    }
    
    debug_print(stderr, "do close()\n");
    if (close(sockfd) < 0) {
        err(1, "close");
    }
    debug_print(stderr, "close() done\n");
    
    return 0;
}
