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

int debug = 0;

int usage()
{
    char msg[] = "Usage: server [-d] [-h] [-p PORT] [-b BUFSIZE] [-s SLEEP_USEC]";
    fprintf(stderr, "%s\n", msg);

    return 0;
}

int main(int argc, char *argv[])
{
    int c;
    int port = 1234;
    int bufsize = 4*1024;
    int sleep_usec = -1;

    while ( (c = getopt(argc, argv, "b:dhp:s:")) != -1) {
        switch (c) {
            case 'h':
                usage();
                exit(0);
                break;
            case 'b':
                bufsize = get_num(optarg);
                break;
            case 'd':
                debug += 1;
                break;
            case 'p':
                port = strtol(optarg, NULL, 0);
                break;
            case 's':
                sleep_usec = strtol(optarg, NULL, 0);
                break;
            default:
                break;
        }
    }

    /* to write() return with error */
    /* default handler of SIGPIPE is exit the process */
    my_signal(SIGPIPE, SIG_IGN);

    fd_set rset, wset, allset;
    FD_ZERO(&rset);
    FD_ZERO(&wset);
    FD_ZERO(&allset);

    unsigned char *buf = (unsigned char *)malloc(bufsize);
    if (buf == NULL) {
        err(1, "malloc");
    }
    memset(buf, 'X', bufsize);

    int sockfd = accept_connection(port);
    if (sockfd < 0) {
        exit(1);
    }

    FD_SET(sockfd, &allset);

    for ( ; ; ) {
        rset = allset;
        wset = allset;
        if (select(sockfd + 1, &rset, &wset, NULL, NULL) < 0) {
            if (errno == EINTR) {
                continue;
            }
            else {
                err(1, "select");
            }
        }
        else { /* select() returns successfully */
            if (debug > 1) {
                debug_print(stderr, "select OK return\n");
            }

            if (FD_ISSET(sockfd, &rset)) {
                unsigned char read_buf[16];
                int n = read(sockfd, read_buf, sizeof(read_buf));
                if (n < 0) {
                    err(1, "read");
                }
                else if (n == 0) { /* client send FIN */
                    debug_print(stderr, "receive stop request.  do shutdown(,SHUT_WR)\n");
                    if (shutdown(sockfd, SHUT_WR) < 0) {
                        errwt(1, "shutdown(sockfd, SHUT_WR)");
                    }
                    debug_print(stderr, "shutdown(,SHUT_WR) done\n");
                    debug_print(stderr, "do close()\n");
                    if (close(sockfd) < 0) {
                        err(1, "close");
                    }
                    debug_print(stderr, "close() done\n");
                    exit(0);
                }
                else {
                    debug_print(stderr, "read data from client: %d bytes\n", n);
                }
            }

            if (FD_ISSET(sockfd, &wset)) {
                if (debug > 1) {
                    debug_print(stderr, "wset set\n");
                }

                int n = write(sockfd, buf, bufsize);
                if (debug > 1) {
                    debug_print(stderr, "write return: %d\n", n);
                }
                if (n < 0) {
                    err(1, "write error");
                }
                else if (n != bufsize) {
                    fprintfwt(stderr, "write partial: %d bytes\n", n);
                    exit(0);
                }
                if (debug > 1) {
                    debug_print(stderr, "write %d bytes\n", n);
                }

                if (sleep_usec > 0) {
                    usleep(sleep_usec);
                }
            }
        }
    }

    return 0;
}
