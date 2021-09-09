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

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: ns-client remote_host port\n");
        exit(1);
    }

    int sockfd = tcp_socket();
    if (sockfd < 0) {
        exit(1);
    }

    char *remote_host = argv[1];
    int   port        = strtol(argv[2], NULL, 0);
    if (connect_tcp(sockfd, remote_host, port) < 0) {
        exit(1);
    }

    int nbytes;
    ioctl(sockfd, FIONREAD, &nbytes);
    fprintf(stderr, "FIONREAD: %d\n", nbytes);
    fprintf(stderr, "do input at nc terminal\n");
    fprintf(stderr, "then hit enter in this terminal to get number of bytes in this socket receive buffer\n");

    int __attribute__((unused)) c = getchar();

    ioctl(sockfd, FIONREAD, &nbytes);
    fprintf(stderr, "FIONREAD: %d\n", nbytes);

    if (close(sockfd) < 0) {
        err(1, "close");
    }

    return 0;
}
