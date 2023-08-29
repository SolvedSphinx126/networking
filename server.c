#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <errno.h>

#define BUF_SIZE 500

int getPort(in_port_t port) {
    uint16_t a = (port << 8);
    uint16_t b = (port >> 8);
    return (int) a | b;
}

char * getIp(in_addr_t addr) {
    u_int8_t a = addr & 255;
    addr >>= 8;

    u_int8_t b = addr & 255;
    addr >>= 8;

    u_int8_t c = addr & 255;
    addr >>= 8;

    u_int8_t d = addr & 255;
    char* ret = malloc(sizeof(char) * 15);
    sprintf(ret, "%d.%d.%d.%d", a, b, c, d);
    return ret;
    // leaks memory don't care yet tho
}

int main() {

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    printf("Create socket fd: %s\n", strerror(errno));

    const struct sockaddr_in addr = {};
    
    bind(sock, (struct sockaddr *restrict) &addr, (socklen_t) sizeof(addr));
    printf("Bind socket to addr: %s\n", strerror(errno));

    listen(sock, 16);
    printf("Start listening: %s\n", strerror(errno));

    struct sockaddr_in addr2 = {};
    int sa_len;
    sa_len = sizeof(addr2);
    getsockname(sock, (struct sockaddr_in *) &addr2, (socklen_t *restrict) &sa_len);
    printf("addr: %s\n", getIp(addr2.sin_addr.s_addr));
    printf("port: %d\n", getPort(addr2.sin_port));
    printf("Get sock info: %s\n", strerror(errno));

    struct sockaddr_in client = {};
    sa_len = sizeof(client);
    accept(sock, (struct sockaddr *restrict) &client, (socklen_t *restrict) &sa_len);
    printf("Accept message: %s\n", strerror(errno));
}

