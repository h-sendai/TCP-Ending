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
#include "logUtil.h"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: shutdown remote_host port\n");
        exit(1);
    }

    char *remote_host = argv[1];
    int  port         = strtol(argv[2], NULL, 0);

    int sockfd = tcp_socket();
    if (sockfd < 0) {
        err(1, "tcp_socket");
    }

    if (connect_tcp(sockfd, remote_host, port) < 0) {
        exit(1);
    }

    if (set_so_rcvbuf(sockfd, 64*1024) < 0) {
        exit(1);
    }

    int so_rcvbuf = get_so_rcvbuf(sockfd);
    if (so_rcvbuf < 0) {
        exit(1);
    }
    fprintfwt(stderr, "SO_RCVBUF: %d bytes\n", so_rcvbuf);

    unsigned char buf[64*1024];
    for (int i = 0; i < 10; ++i) {
        int n = read(sockfd, buf, sizeof(buf));
        if (n < 0) {
            err(1, "read");
        }
        fprintfwt(stderr, "read returns: %d bytes\n", n);
    }

    fprintfwt(stderr, "sleep 3 seconds\n");
    sleep(3);
    fprintfwt(stderr, "sleep 3 seconds done\n");

    int nbytes;
    ioctl(sockfd, FIONREAD, &nbytes);
    fprintfwt(stderr, "FIONREAD: %d bytes\n", nbytes);

    fprintfwt(stderr, "do shutdown(, SHUT_RD)\n");
    if (shutdown(sockfd, SHUT_RD) < 0) {
        err(1, "shutdown(sockfd, SHUT_RD)");
    }
    fprintfwt(stderr, "shutdown(, SHUT_RD) done\n");

    ioctl(sockfd, FIONREAD, &nbytes);
    fprintfwt(stderr, "FIONREAD: %d bytes\n", nbytes);

    fprintfwt(stderr, "waiting. hit enter to continue\n");
    int __attribute__((unused)) c = getchar();
    fprintfwt(stderr, "program resume\n");

    ioctl(sockfd, FIONREAD, &nbytes);
    fprintfwt(stderr, "FIONREAD: %d bytes\n", nbytes);

    fprintfwt(stderr, "do close(sockfd)\n");
    if (close(sockfd) < 0) {
        err(1, "close(sockfd)");
    }
    fprintfwt(stderr, "close(sockfd) done\n");

    return 0;
}
