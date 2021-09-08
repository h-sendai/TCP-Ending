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

    unsigned char buf[64*1024];
    for (int i = 0; i < 1000; ++i) {
        int n = read(sockfd, buf, sizeof(buf));
        if (n < 0) {
            err(1, "read");
        }
    }

    int nbytes;

    ioctl(sockfd, FIONREAD, &nbytes);
    fprintfwt(stderr, "FIONREAD: %d bytes\n", nbytes);

    fprintfwt(stderr, "going shutdown(, SHUT_WR)\n");
    if (shutdown(sockfd, SHUT_WR) < 0) {
        err(1, "shutdown(sockfd, SHUT_WR)");
    }
    fprintfwt(stderr, "shutdown(, SHUT_WR) done\n");

    ioctl(sockfd, FIONREAD, &nbytes);
    fprintfwt(stderr, "FIONREAD: %d bytes\n", nbytes);
    
    sleep(2);

    ioctl(sockfd, FIONREAD, &nbytes);
    fprintfwt(stderr, "FIONREAD: %d bytes\n", nbytes);

    fprintfwt(stderr, "going close()\n");
    if (close(sockfd) < 0) {
        err(1, "close");
    }
    fprintfwt(stderr, "close() done\n");

    fprintfwt(stderr, "program exit\n");
    return 0;
}
