#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <errno.h>

#define BUF_SIZE 500


int main() {
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    printf("Create socket fd: %s\n", strerror(errno));

    const struct sockaddr_in addr = {};
    bind(sock, &addr, (socklen_t) sizeof(addr));
    printf("Bind socket to addr: %s\n", strerror(errno));

    listen(sock, 16);
    printf("Start listening: %s\n", strerror(errno));

    getsockname(sock, (struct sockaddr *restrict) &addr, (socklen_t *restrict) sizeof(addr));
    printf("addr: %d\n", addr.sin_addr.s_addr);
    printf("addr: %d\n", addr.sin_port);

    accept(sock, (struct sockaddr *restrict) &addr, (socklen_t *restrict) sizeof(addr));
    printf("Accept message: %s\n", strerror(errno));
}